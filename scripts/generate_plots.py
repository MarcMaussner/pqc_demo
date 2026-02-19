import re
import sys
import matplotlib.pyplot as plt

def parse_uart_log(filepath):
    results = []
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except UnicodeDecodeError:
        with open(filepath, 'r', encoding='latin-1') as f:
            content = f.read()

    # Regex to capture: UART >> Algorithm: Operation took Cycles cycles, Stack: Stack bytes
    pattern = r"(?:UART >>\s*)+(.*?): (.*?) took (\d+) cycles, Stack: (\d+) bytes"
    matches = re.findall(pattern, content)

    for match in matches:
        alg, op, cycles, stack = match
        cycles = int(cycles)
        stack = int(stack)
        time_ms = (cycles / 216000000) * 1000  # Assuming 216 MHz clk
        results.append({
            "Algorithm": alg.strip(),
            "Operation": op.strip(),
            "Cycles": cycles,
            "Time_ms": round(time_ms, 2),
            "Stack_Bytes": stack
        })
    
    return results

def generate_plots(data, output_prefix="benchmark"):
    if not data:
        print("No data to plot.")
        return

    # Extract unique labels and values for plotting
    # Group by Algorithm + Operation
    labels = []
    cycles = []
    stack = []
    
    # Simple deduplication / averaging if needed, but for now just take unique entries or last one
    # Or plot all occurrences
    # Let's aggregate by Alg+Op
    from collections import defaultdict
    agg_cycles = defaultdict(list)
    agg_stack = defaultdict(list)
    
    for r in data:
        key = f"{r['Algorithm']} {r['Operation']}"
        agg_cycles[key].append(r['Cycles'])
        agg_stack[key].append(r['Stack_Bytes'])
    
    # Calculate means
    sorted_keys = sorted(agg_cycles.keys())
    labels = sorted_keys
    mean_cycles = [sum(agg_cycles[k])/len(agg_cycles[k]) for k in sorted_keys]
    mean_stack = [sum(agg_stack[k])/len(agg_stack[k]) for k in sorted_keys]

    # Plot Cycles
    plt.figure(figsize=(12, 6))
    plt.bar(labels, mean_cycles, color='skyblue')
    plt.title('Cryptographic Performance (Cycle Count)', fontsize=14)
    plt.ylabel('CPU Cycles (Lower is Better)')
    plt.xticks(rotation=45, ha='right')
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_performance.png")
    print(f"Generated {output_prefix}_performance.png")

    # Plot Stack
    plt.figure(figsize=(12, 6))
    plt.bar(labels, mean_stack, color='salmon')
    plt.title('Memory Usage (Stack)', fontsize=14)
    plt.ylabel('Stack Usage (Bytes) (Lower is Better)')
    plt.xticks(rotation=45, ha='right')
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_stack.png")
    print(f"Generated {output_prefix}_stack.png")

def print_table(data):
    print(f"{'Algorithm':<20} | {'Operation':<20} | {'Cycles':<10} | {'Time (ms)':<10} | {'Stack (B)':<10}")
    print("-" * 80)
    for r in data:
        print(f"{r['Algorithm']:<20} | {r['Operation']:<20} | {r['Cycles']:<10} | {r['Time_ms']:<10} | {r['Stack_Bytes']:<10}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python generate_plots.py <log_file>")
        sys.exit(1)
    
    log_file = sys.argv[1]
    data = parse_uart_log(log_file)
    
    print_table(data)
    
    try:
        generate_plots(data)
    except Exception as e:
        print(f"Plot generation failed: {e}")
