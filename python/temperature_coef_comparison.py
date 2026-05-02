import re
from pathlib import Path

from bokeh.models import BoxZoomTool, WheelZoomTool
from bokeh.palettes import turbo
from bokeh.plotting import figure, output_file, save


BASE_DIR = Path(__file__).resolve().parent.parent
INPUT_DIR = BASE_DIR / "temperature_characteristics"
OUTPUT_DIR = BASE_DIR / "chart"
OUTPUT_FILE = OUTPUT_DIR / "temperature_coef_comparision.html"


def parse_measurements(file_path: Path) -> list[tuple[int, int]]:
	"""Zwraca listę punktów (reference_pressure, measurement_result)."""
	points: list[tuple[int, int]] = []
	current_pressure: int | None = None

	with file_path.open("r", encoding="utf-8") as file:
		for line in file:
			pressure_match = re.search(r"reference preassure:\s*(-?\d+)", line)
			if pressure_match:
				current_pressure = int(pressure_match.group(1))
				continue

			measurement_match = re.search(r"Measurement result:\s*(-?\d+)", line)
			if measurement_match and current_pressure is not None:
				points.append((current_pressure, int(measurement_match.group(1))))

	points.sort(key=lambda item: item[0])
	return points


def build_chart(series: dict[str, list[tuple[int, int]]], output_file_path: Path) -> None:
	plot = figure(
		title="Temperature Coef Comparison",
		x_axis_label="Reference Pressure",
		y_axis_label="Measurement Result",
		sizing_mode="stretch_both",
		tools="pan,reset,save",
	)

	box_zoom_y = BoxZoomTool(dimensions="height")
	wheel_zoom_y = WheelZoomTool(dimensions="height")
	plot.add_tools(box_zoom_y, wheel_zoom_y)
	plot.toolbar.active_scroll = wheel_zoom_y

	colors = turbo(max(3, len(series)))

	for index, (name, points) in enumerate(sorted(series.items())):
		if not points:
			continue

		x_values = [x for x, _ in points]
		y_values = [y for _, y in points]

		color = colors[index % len(colors)]
		plot.line(x_values, y_values, line_width=2, color=color, legend_label=name)
		plot.scatter(x_values, y_values, size=7, marker="circle", color=color, legend_label=name)

	plot.legend.click_policy = "hide"
	output_file(output_file_path)
	save(plot)


def main() -> None:
	if not INPUT_DIR.is_dir():
		raise FileNotFoundError(f"Input directory does not exist: {INPUT_DIR}")

	input_files = sorted(path for path in INPUT_DIR.glob("*.txt") if path.is_file())
	if not input_files:
		print(f"[INFO] Brak plikow .txt w katalogu: {INPUT_DIR}")
		return

	OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

	all_series: dict[str, list[tuple[int, int]]] = {}
	for input_file in input_files:
		points = parse_measurements(input_file)
		if points:
			all_series[input_file.stem] = points
			print(f"[OK] {input_file.name}: {len(points)} punktow")
		else:
			print(f"[WARN] {input_file.name}: brak poprawnych punktow")

	if not all_series:
		print("[INFO] Nie znaleziono danych do narysowania wykresu.")
		return

	build_chart(all_series, OUTPUT_FILE)
	print(f"[OK] Zapisano wykres: {OUTPUT_FILE}")


if __name__ == "__main__":
	main()
