import os
from pathlib import Path
import re

BASE_DIR = Path(__file__).resolve().parent.parent
INPUT_DIR = BASE_DIR / "tmp"
OUTPUT_DIR = BASE_DIR / "ready_logs"

def process_file(input_path, output_path):
    with open(input_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    new_lines = []
    i = 0

    while i < len(lines):
        line = lines[i]

        # znajdź początek bloku (reference preassure)
        if "reference preassure:" in line:
            block = [line]
            i += 1

            # zbierz linie aż do 4 pomiarów
            measurements = []
            while i < len(lines) and len(measurements) < 4:
                block.append(lines[i])
                if "Measurement result:" in lines[i]:
                    measurements.append(lines[i])
                i += 1

            if len(measurements) == 4:
                # wyciągnij temperaturę z 1 pomiaru
                match = re.search(r"Measurement result:\s*(-?\d+)", measurements[0])
                if match:
                    temperature = match.group(1)

                    # podmień reference preassure
                    block[0] = re.sub(
                        r"reference preassure:\s*-?\d+",
                        f"reference preassure: {temperature}",
                        block[0]
                    )

                    # zostaw tylko 4-ty pomiar
                    last_measurement = measurements[3]

                    # usuń wszystkie pomiary z bloku
                    block = [l for l in block if "Measurement result:" not in l]

                    # dodaj tylko ostatni
                    block.append(last_measurement)

            new_lines.extend(block)

        else:
            new_lines.append(line)
            i += 1

    with open(output_path, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)


def main():
    if not INPUT_DIR.is_dir():
        raise FileNotFoundError(f"Input directory does not exist: {INPUT_DIR}")

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    for filename in os.listdir(INPUT_DIR):
        input_path = os.path.join(INPUT_DIR, filename)
        output_path = os.path.join(OUTPUT_DIR, filename)

        if os.path.isfile(input_path):
            try:
                process_file(input_path, output_path)
                print(f"[OK] {filename}")
            except Exception as e:
                print(f"[ERROR] {filename}: {e}")


if __name__ == "__main__":
    main()