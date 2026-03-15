import re
import csv
from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt
from bokeh.plotting import figure, show, output_file
from bokeh.palettes import turbo
from bokeh.models import Range1d, WheelZoomTool, BoxZoomTool


# foldery
input_folder = Path("../ready_logs")
csv_folder = Path("../csv")
chart_folder = Path("../chart")

csv_folder.mkdir(parents=True, exist_ok=True)
chart_folder.mkdir(parents=True, exist_ok=True)

for input_file in input_folder.glob("*.txt"):
    chart_file = chart_folder / (input_file.stem + ".html")
    csv_file = csv_folder / (input_file.stem + ".csv")

    if chart_file.exists() and csv_file.exists():
        print(f"Pominięto {input_file} – już istnieje")
        continue

    print(f"Analizowanie {input_file}")

    # parsowanie danych
    current_pressure = None
    data = {}

    with input_file.open("r", encoding="utf-8") as f:
        for line in f:
            # nowa seria pomiarów
            m = re.search(r"reference preassure:\s*(-?\d+)", line)
            if m:
                current_pressure = int(m.group(1))
                continue

            # measurement + parameters
            m = re.search(
                r"Measurement result:\s*(-?\d+)\s+for configuration: interface:([a-zA-Z0-9_]+), parameters:\s*([0-9,\s]+)",
                line
            )
            if m:
                result = int(m.group(1))
                interface = m.group(2).strip()
                params = m.group(3).strip().replace(" ", "")  # usuwa spacje w parametrach
                key = f"{interface} {params}"  # łączenie interfejsu z parametrami

                if key not in data:
                    data[key] = []

                data[key].append((current_pressure, result))

    # sortowanie po pierwszym elemencie (ciśnieniu)
    for key in data:
        data[key].sort(key=lambda x: x[0])

    csv_data: dict[int, dict[str, list[int]]] = {}

    for key in data:
        for pressure, result in data[key]:
            if pressure not in csv_data:
                csv_data[pressure] = {}

            if key not in csv_data[pressure]:
                csv_data[pressure][key] = []

            csv_data[pressure][key].append(result)


    all_keys = sorted(data.keys())
    all_pressures = sorted(csv_data.keys())

    with csv_file.open("w", newline="", encoding="utf-8-sig") as f:
        writer = csv.writer(f, delimiter=";")
        writer.writerow(["reference_pressure"] + all_keys)

        for p in all_pressures:
            # ile wierszy potrzebujemy dla tego ciśnienia
            max_len = max(len(csv_data[p].get(k, [])) for k in all_keys)

            for i in range(max_len):
                # tylko pierwszy wiersz dla danego ciśnienia pokazuje wartość ciśnienia
                row = [p]

                for k in all_keys:
                    values = csv_data[p].get(k, [])
                    if i < len(values):
                        row.append(values[i])
                    else:
                        row.append("")  # pusta komórka jeśli brak pomiaru

                writer.writerow(row)
    
    print(f"saved {csv_file}")


    #przygotowanie wykresu Bokeh
    p = figure(
        title="Wyniki pomiarów",
        x_axis_label="Reference Pressure",
        y_axis_label="Measurement Result",
        sizing_mode="stretch_both",
        tools="pan,reset,save",  # inne narzędzia
    )
    box_zoom_y = BoxZoomTool(dimensions="height")
    p.add_tools(box_zoom_y)

    # Wheel zoom tylko po Y
    wheel_zoom_y = WheelZoomTool(dimensions="height")
    p.add_tools(wheel_zoom_y)

    # Aktywacja scrolla i box zoom
    p.toolbar.active_scroll = wheel_zoom_y  

    colors = turbo(30)

    for i, key in enumerate(all_keys):
        x_vals = []
        y_vals = []

        for pressure in all_pressures:
            values = csv_data[pressure].get(key, [])
            for v in values:
                x_vals.append(pressure)
                y_vals.append(v)

        # zamiast circle() używamy scatter()
        p.scatter(x_vals, y_vals, size=8, color=colors[i % len(colors)], marker="circle", legend_label=key)
        p.line(x_vals, y_vals, color=colors[i % len(colors)], line_width=2, legend_label=key)

    output_file(chart_file)
    print(f"saved {chart_file}")
    p.legend.click_policy = "hide"
    show(p)