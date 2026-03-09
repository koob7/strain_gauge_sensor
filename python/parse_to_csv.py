import re
import csv
from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt

# foldery
input_folder = Path("../logs")
csv_folder = Path("../csv")
png_folder = Path("../png")
csv_folder.mkdir(parents=True, exist_ok=True)
png_folder.mkdir(parents=True, exist_ok=True)

for input_file in input_folder.glob("*.txt"):
    csv_file = csv_folder / (input_file.stem + ".csv")
    
    if csv_file.exists():
        print(f"Pominięto {csv_file} – już istnieje")
        continue

    # parsowanie danych
    rows = []
    current_pressure = None
    current_measurements = {}
    all_params = []

    with input_file.open("r", encoding="utf-8") as f:
        for line in f:
            # nowa seria pomiarów
            m = re.search(r"reference preassure:\s*(-?\d+)", line)
            if m:
                if current_pressure is not None:
                    row = [current_pressure] + [current_measurements.get(p, "") for p in all_params]
                    rows.append(row)
                current_pressure = int(m.group(1))
                current_measurements = {}
                continue

            # measurement + parameters
            m = re.search(r"Measurement result:\s*(-?\d+).*parameters:\s*([0-9,\s]+)", line)
            if m:
                result = int(m.group(1))
                params = m.group(2).strip()
                column_name = f"parameters:{params}"
                if column_name not in all_params:
                    all_params.append(column_name)
                current_measurements[column_name] = result

    # dodaj ostatnią serię
    if current_pressure is not None:
        row = [current_pressure] + [current_measurements.get(p, "") for p in all_params]
        rows.append(row)

    if rows:
        # zapis CSV
        with csv_file.open("w", newline="", encoding="utf-8-sig") as f:
            writer = csv.writer(f, delimiter=';')
            headers = ["reference_pressure"] + all_params
            writer.writerow(headers)
            writer.writerows(rows)
        print(f"Zapisano CSV: {csv_file}")

        # generowanie wykresu
        df = pd.read_csv(csv_file, delimiter=';', encoding='utf-8-sig')
        x = df['reference_pressure']
        plt.figure(figsize=(10, 6))
        for col in all_params:
            plt.plot(x, df[col], marker='o', label=col)
        plt.xlabel('Reference Pressure')
        plt.ylabel('Measurement Result')
        plt.title(f'Pomiar dla {input_file.stem}')
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.grid(True)
        plt.tight_layout()
        
        # zapisz do folderu PNG
        png_file = png_folder / (input_file.stem + ".png")
        plt.savefig(png_file)
        plt.close()
        print(f"Zapisano wykres PNG: {png_file}")
    else:
        print(f"Brak danych w {input_file}, plik pominięty")