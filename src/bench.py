#!/usr/bin/env python3
import os
import subprocess
import glob
import time
from concurrent.futures import ThreadPoolExecutor
from collections import defaultdict

# --- Configuration ---
TIMEOUT_SEC = 10.0
BIN_DIR = "bin"
TEST_DIR = "tests"
MODEL_SOL = "msths1"
GENERATOR = "generator"


# --- Colors ---
class C:
    GREEN = "\033[92m"
    RED = "\033[91m"
    YELLOW = "\033[93m"
    RESET = "\033[0m"


def colorize(text, status):
    if status == "OK":
        return f"{C.GREEN}{text}{C.RESET}"
    elif status == "WA":
        return f"{C.RED}{text}{C.RESET}"
    elif status == "TIMEOUT":
        return f"{C.YELLOW}{text}{C.RESET}"
    return text


def compile_project():
    print("Compiling...")
    subprocess.run(["make", "all"], check=True)


def get_files():
    exes = [
        f
        for f in glob.glob(os.path.join(BIN_DIR, "*"))
        if os.access(f, os.X_OK) and not os.path.isdir(f)
    ]
    solutions = [p for p in exes if os.path.basename(p) != GENERATOR]
    tests = sorted(glob.glob(os.path.join(TEST_DIR, "*.in")))
    return solutions, tests


def run_process(cmd, input_file):
    start = time.time()
    try:
        with open(input_file, "r") as f:
            proc = subprocess.run(
                [cmd], stdin=f, capture_output=True, text=True, timeout=TIMEOUT_SEC
            )
            duration = time.time() - start
            return proc.stdout.strip(), duration, "OK"
    except subprocess.TimeoutExpired:
        return "", TIMEOUT_SEC, "TIMEOUT"
    except Exception:
        return "", 0.0, "ERROR"


def main():
    compile_project()
    solutions, tests = get_files()

    model_path = os.path.join(BIN_DIR, MODEL_SOL)
    if model_path not in solutions:
        print(f"{C.RED}Error: Model solution {MODEL_SOL} not found.{C.RESET}")
        return

    # 1. Generate References
    print(f"Generating references using {MODEL_SOL}...")
    references = {}
    with ThreadPoolExecutor() as executor:
        future_to_test = {executor.submit(run_process, model_path, t): t for t in tests}
        for future in future_to_test:
            test_path = future_to_test[future]
            out, _, status = future.result()
            references[test_path] = out

    # 2. Benchmark
    print(
        f"Benchmarking {len(solutions)} solutions on {len(tests)} tests (Parallel)..."
    )

    tasks = []
    results = []

    with ThreadPoolExecutor(max_workers=os.cpu_count()) as executor:
        for sol in solutions:
            for test in tests:
                f = executor.submit(run_process, sol, test)
                f.meta_sol = sol
                f.meta_test = test
                tasks.append(f)

        for future in tasks:
            sol_path = future.meta_sol
            test_path = future.meta_test
            output, duration, run_status = future.result()

            expected = references.get(test_path, "")
            final_status = run_status
            if run_status == "OK" and output != expected:
                final_status = "WA"

            # Print Progress
            sol_name = os.path.basename(sol_path)
            test_name = os.path.basename(test_path)
            print(
                f"[{colorize(final_status, final_status)}] {sol_name} @ {test_name} ({duration:.2f}s)"
            )

            results.append(
                {
                    "sol": sol_name,
                    "test": test_name,
                    "time": duration,
                    "status": final_status,
                }
            )

    # 3. Aggregate
    aggregated = defaultdict(lambda: defaultdict(list))
    for r in results:
        try:
            n = int(r["test"].split("_")[1])
        except:
            n = 0
        aggregated[r["sol"]][n].append(r)

    # 4. Print Table
    all_ns = sorted(list(set(k for sol in aggregated for k in aggregated[sol])))

    # Increased width to fit "OK (av:10.00 mx:10.00)"
    col_width = 28
    first_col_width = 20

    # Header
    header_cells = [f"n={n}".ljust(col_width) for n in all_ns]
    print("\n" + "=" * (first_col_width + 3 + len(all_ns) * (col_width + 3)))
    print(f"{'Solution'.ljust(first_col_width)} | " + " | ".join(header_cells))
    print("-" * (first_col_width + 3 + len(all_ns) * (col_width + 3)))

    for sol in sorted(aggregated.keys()):
        row_str = f"{sol.ljust(first_col_width)} | "

        for n in all_ns:
            group = aggregated[sol].get(n, [])
            if not group:
                row_str += f"{'-'.ljust(col_width)} | "
                continue

            stats = [x["status"] for x in group]
            times = [x["time"] for x in group]

            if "TIMEOUT" in stats:
                cell_status = "TIMEOUT"
            elif "WA" in stats:
                cell_status = "WA"
            else:
                cell_status = "OK"

            max_time = max(times)
            avg_time = sum(times) / len(times)

            # Format: STATUS (av:0.00 mx:0.00)
            text = f"{cell_status} (av:{avg_time:.2f} mx:{max_time:.2f})"

            # Pad text to maintain alignment
            padded_text = text.ljust(col_width)
            # Colorize output
            row_str += f"{colorize(padded_text, cell_status)} | "

        print(row_str)


if __name__ == "__main__":
    main()
