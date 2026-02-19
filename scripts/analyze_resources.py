import sys
import re
import os
import json

def parse_map_file(map_file):
    if not os.path.exists(map_file):
        print(f"Error: File {map_file} not found.")
        return None

    # Modules definition
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

    # Regex for detailed line: .section 0xAddr 0xSize Path
    # Matches: .text 0x0800... 0x... ...
    # Also matches: .text.foo 0x0800... 0x... ...
    regex_full = re.compile(r'^\s*(\.[a-zA-Z0-9_.]+)\s+0x[0-9a-fA-F]+\s+(0x[0-9a-fA-F]+)\s+(.+)$')
    
    # Regex for just section name: .section
    regex_section = re.compile(r'^\s*(\.[a-zA-Z0-9_.]+)\s*$')
    
    # Regex for continuation: 0xAddr 0xSize Path
    regex_cont = re.compile(r'^\s+0x[0-9a-fA-F]+\s+(0x[0-9a-fA-F]+)\s+(.+)$')

    current_section = None

    with open(map_file, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = line.rstrip()
            
            # Check full match first
            match = regex_full.match(line)
            if match:
                section_name = match.group(1)
                size_hex = match.group(2)
                path = match.group(3)
                
                # Update current section in case next lines are just "0x... 0x..." (rare but possible?)
                # Actually, usually full match means done. But keep it for context if needed.
                current_section = section_name 
                process_entry(modules, section_name, size_hex, path)
                continue

            # Check section only
            match = regex_section.match(line)
            if match:
                current_section = match.group(1)
                continue

            # Check continuation
            match = regex_cont.match(line)
            if match and current_section:
                size_hex = match.group(1)
                path = match.group(2)
                process_entry(modules, current_section, size_hex, path)
                continue

    return modules

def process_entry(modules, section_name, size_hex, path):
    try:
        size = int(size_hex, 16)
    except ValueError:
        return

    if size == 0:
        return

    # Categorize
    matched_category = "Other"
    for cat, data in modules.items():
        if cat == "Other": continue
        for pat in data["patterns"]:
            if pat in path:
                matched_category = cat
                break
        if matched_category != "Other":
            break
            
    # Add to totals
    # ROM = .text* + .rodata*
    # RAM = .data* + .bss*
    
    if section_name.startswith(".text") or section_name.startswith(".rodata") or section_name == ".isr_vector":
        modules[matched_category]["rom"] += size
    elif section_name.startswith(".data") or section_name.startswith(".bss"):
        if section_name.startswith(".data"):
             modules[matched_category]["rom"] += size # Data init is in ROM
        modules[matched_category]["ram"] += size

def print_report(modules):
    print(f"{'Module':<25} | {'Flash (Bytes)':<15} | {'RAM (Bytes)':<15}")
    print("-" * 60)
    
    total_rom = 0
    total_ram = 0

    for name, data in modules.items():
        print(f"{name:<25} | {data['rom']:<15,} | {data['ram']:<15,}")
        total_rom += data['rom']
        total_ram += data['ram']
    
    print("-" * 60)
    print(f"{'TOTAL':<25} | {total_rom:<15,} | {total_ram:<15,}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python analyze_resources.py <map_file>")
        sys.exit(1)
    
    map_file = sys.argv[1]
    data = parse_map_file(map_file)
    if data:
        print_report(data)
