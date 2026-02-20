import re
import sys
import os

def parse_uart_log(filepath):
    if not os.path.exists(filepath):
        print(f"Warning: {filepath} not found.")
        return []
    with open(filepath, 'rb') as f:
        content = f.read().decode('utf-8', errors='ignore')

    pattern = r"(?:UART >>\s*)+(.*?): (.*?) took (\d+) cycles, Stack: (\d+) bytes"
    matches = re.findall(pattern, content)

    results = []
    for match in matches:
        alg, op, cycles, stack = match
        cycles = int(cycles)
        stack = int(stack)
        time_ms = (cycles / 216000000) * 1000
        results.append({
            "Algorithm": alg.strip(),
            "Operation": op.strip(),
            "Cycles": cycles,
            "Time_ms": round(time_ms, 2),
            "Stack_Bytes": stack
        })
    return results

def parse_map_file(map_file):
    if not os.path.exists(map_file):
        print(f"Warning: {map_file} not found.")
        return None

    modules = {
        "mbedTLS (RSA)": {"rom": 0, "ram": 0, "patterns": ["mbedtls", "RSA/Src"]},
        "ML-KEM-512": {"rom": 0, "ram": 0, "patterns": ["ml-kem-512"]},
        "ML-DSA-44": {"rom": 0, "ram": 0, "patterns": ["ml-dsa-44"]},
        "Falcon-512": {"rom": 0, "ram": 0, "patterns": ["falcon-512"]},
        "SPHINCS+": {"rom": 0, "ram": 0, "patterns": ["sphincs"]},
        "STM32 HAL": {"rom": 0, "ram": 0, "patterns": ["STM32F7xx_HAL_Driver"]},
        "Core & System": {"rom": 0, "ram": 0, "patterns": ["Core", "startup", "libc", "libgcc", "libnosys"]}, 
        "Other": {"rom": 0, "ram": 0, "patterns": []}
    }

    regex_full = re.compile(r'^\s*(\.[a-zA-Z0-9_.]+)\s+0x[0-9a-fA-F]+\s+(0x[0-9a-fA-F]+)\s+(.+)$')
    regex_section = re.compile(r'^\s*(\.[a-zA-Z0-9_.]+)\s*$')
    regex_cont = re.compile(r'^\s+0x[0-9a-fA-F]+\s+(0x[0-9a-fA-F]+)\s+(.+)$')

    current_section = None
    with open(map_file, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = line.rstrip()
            match = regex_full.match(line)
            if match:
                section_name, size_hex, path = match.groups()
                current_section = section_name 
                process_entry(modules, section_name, size_hex, path)
            elif (match := regex_section.match(line)):
                current_section = match.group(1)
            elif (match := regex_cont.match(line)) and current_section:
                size_hex, path = match.groups()
                process_entry(modules, current_section, size_hex, path)

    return modules

def process_entry(modules, section_name, size_hex, path):
    try:
        size = int(size_hex, 16)
    except ValueError: return
    if size == 0: return

    matched_category = "Other"
    for cat, data in modules.items():
        if cat == "Other": continue
        if any(pat in path for pat in data["patterns"]):
            matched_category = cat
            break
            
    if section_name.startswith(".text") or section_name.startswith(".rodata") or section_name == ".isr_vector":
        modules[matched_category]["rom"] += size
    elif section_name.startswith(".data") or section_name.startswith(".bss"):
        if section_name.startswith(".data"):
             modules[matched_category]["rom"] += size
        modules[matched_category]["ram"] += size

def generate_performance_table(results):
    # Filter for PQC comparisons
    pqc_results = [r for r in results if not r['Algorithm'].startswith("RSA")]
    # Note: Clean C vs Assembly is hard to automate without labels in UART.
    # We will just print the latest measurements as "Latest".
    header = "| Algorithm | Operation | Clock Cycles | Time (ms) | Speedup |\n| :--- | :--- | ---: | ---: | :--- |"
    rows = []
    for r in results:
        rows.append(f"| {r['Algorithm']} | {r['Operation']} | {r['Cycles']:,} | {r['Time_ms']:,} | - |")
    return header + "\n" + "\n".join(rows)

def generate_stack_table(results):
    header = "| Algorithm | Operation | Peak Stack (Bytes) |\n| :--- | :--- | ---: |"
    rows = []
    seen = set()
    for r in results:
        key = (r['Algorithm'], r['Operation'])
        if key in seen: continue
        seen.add(key)
        rows.append(f"| {r['Algorithm']} | {r['Operation']} | {r['Stack_Bytes']:,} |")
    return header + "\n" + "\n".join(rows)

def generate_resource_table(modules):
    total_rom = sum(m['rom'] for m in modules.values())
    total_ram = sum(m['ram'] for m in modules.values())
    
    rom_kb = total_rom / 1024
    ram_kb = total_ram / 1024
    
    rom_util = (total_rom / (2048 * 1024)) * 100
    ram_util = (total_ram / (512 * 1024)) * 100
    
    header = "| Resource | Size (Bytes) | Size (KB) | Capacity (STM32F769) | Utilization |\n| :--- | :--- | :--- | :--- | :--- |"
    rows = [
        f"| **Flash (ROM)** | **{total_rom:,}** | **~{rom_kb:.1f} KB** | 2,048 KB | ~{rom_util:.1f}% |",
        f"| **RAM (Static)** | **{total_ram:,}** | **~{ram_kb:.1f} KB** | 512 KB | ~{ram_util:.1f}% |"
    ]
    return header + "\n" + "\n".join(rows)

def update_report(report_path, log_path, map_path):
    results = parse_uart_log(log_path)
    modules = parse_map_file(map_path)
    
    with open(report_path, 'r', encoding='utf-8') as f:
        content = f.read()

    replacements = {
        "PERFORMANCE_TABLE": generate_performance_table(results),
        "STACK_TABLE": generate_stack_table(results),
        "RESOURCE_TABLE": generate_resource_table(modules) if modules else None
    }

    for marker, table in replacements.items():
        if table is None: continue
        start = f"<!-- {marker}_START -->"
        end = f"<!-- {marker}_END -->"
        pattern = re.escape(start) + r".*?" + re.escape(end)
        content = re.sub(pattern, f"{start}\n{table}\n{end}", content, flags=re.DOTALL)

    with open(report_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Updated {report_path}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python update_report.py <log_file> <map_file>")
        sys.exit(1)
    
    report_file = "docs/FINAL_REPORT.md"
    log_file = sys.argv[1]
    map_file = sys.argv[2]
    update_report(report_file, log_file, map_file)
