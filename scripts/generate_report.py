import re
import sys

def parse_uart_log(filepath):
    with open(filepath, 'rb') as f:
        content = f.read().decode('utf-8', errors='ignore')

    # Regex to find: (UART >> )+ ALGORITHM: OPERATION took CYCLES cycles, Stack: STACK bytes
    # Matches 'UART >> RSA: ...' or 'UART >> UART >> RSA: ...'
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
            "Time (ms)": round(time_ms, 2),
            "Stack (Bytes)": stack
        })
    
    return results

def print_markdown_table(results):
    if not results:
        print("No results found.")
        return

    print("| Algorithm | Operation | Clock Cycles | Time (ms) | Peak Stack (B) |")
    print("|---|---|---|---|---|")
    for r in results:
        print(f"| {r['Algorithm']} | {r['Operation']} | {r['Cycles']:,} | {r['Time (ms)']:,} | {r['Stack (Bytes)']:,} |")

if __name__ == "__main__":
    log_file = sys.argv[1] if len(sys.argv) > 1 else "benchmarks_final_fixed_utf8.txt"
    data = parse_uart_log(log_file)
    print_markdown_table(data)
