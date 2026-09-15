#!/usr/bin/env python3
"""
Generate CSV and Markdown versions of Table 1 from RSS-fPAKE and RRSS-fPAKE
statistics files.

Examples:
    python3 artifact/scripts/generate_table1.py --levels 128
    python3 artifact/scripts/generate_table1.py
"""

import argparse
import csv
import re
from pathlib import Path


NUMBER = r"[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?"

TIMING_RE = re.compile(
    rf"^(?P<input_type>\S+)\s+"
    rf"calc\((?P<role>sender|receiver)\):\s+"
    rf"(?P<calc>{NUMBER})\s+±\s+{NUMBER}\s+"
    rf"net\((?:sender|receiver)\):\s+"
    rf"(?P<net>{NUMBER})\s+±\s+{NUMBER}\s+"
    rf"overall\([^)]*\):\s+"
    rf"(?P<overall>{NUMBER})\s+±\s+{NUMBER}\s*$",
    re.IGNORECASE,
)

COMM_RE = re.compile(
    rf"^(?P<input_type>\S+)\s+"
    rf"(?P<role>Sender|Receiver)\s+Communication\s+Overhead:\s+"
    rf"(?P<communication>{NUMBER})\s+±\s+{NUMBER}\s*$",
    re.IGNORECASE,
)

INPUT_ORDER = ["acc_h-bar", "acc_h-gyrW"]
ROLE_ORDER = ["sender", "receiver"]

ALGORITHMS = [
    {
        "name": "fPAKE RSS",
        "folder_prefix": "RSSFPAKE",
    },
    {
        "name": "fPAKE RRSS",
        "folder_prefix": "RRSSFPAKE",
    },
]


def parse_statistics(path: Path) -> dict:
    """Parse one statistics.txt file into input-type and role measurements."""
    values = {}

    with path.open("r", encoding="utf-8") as file:
        for raw_line in file:
            line = raw_line.strip()

            timing_match = TIMING_RE.match(line)
            if timing_match:
                input_type = timing_match.group("input_type")
                role = timing_match.group("role").lower()

                values.setdefault(input_type, {})
                values[input_type].setdefault(role, {})

                values[input_type][role]["calc"] = float(
                    timing_match.group("calc")
                )
                values[input_type][role]["net"] = float(
                    timing_match.group("net")
                )
                values[input_type][role]["overall"] = float(
                    timing_match.group("overall")
                )
                continue

            comm_match = COMM_RE.match(line)
            if comm_match:
                input_type = comm_match.group("input_type")
                role = comm_match.group("role").lower()

                values.setdefault(input_type, {})
                values[input_type].setdefault(role, {})

                values[input_type][role]["communication"] = float(
                    comm_match.group("communication")
                )

    return values


def validate_measurements(values: dict, path: Path) -> None:
    """Ensure every required table entry is present."""
    required_fields = {"calc", "net", "overall", "communication"}

    for input_type in INPUT_ORDER:
        if input_type not in values:
            raise ValueError(
                f"Missing input type '{input_type}' in {path}"
            )

        for role in ROLE_ORDER:
            if role not in values[input_type]:
                raise ValueError(
                    f"Missing role '{role}' for '{input_type}' in {path}"
                )

            missing = required_fields - values[input_type][role].keys()
            if missing:
                raise ValueError(
                    f"Missing {sorted(missing)} for '{input_type}', "
                    f"role '{role}' in {path}"
                )


def format_number(value: float) -> str:
    return f"{value:.4f}"


def write_csv(rows: list[dict], output_path: Path) -> None:
    fieldnames = [
        "Algorithm",
        "Sec.",
        "Type",
        "Role",
        "Run time (calc)",
        "Comm. time (net)",
        "Overall time",
        "Role comm. ovhd. (KiB)",
        "Total comm. ovhd. (KiB)",
    ]

    with output_path.open("w", newline="", encoding="utf-8") as file:
        writer = csv.DictWriter(file, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def write_markdown(rows: list[dict], output_path: Path) -> None:
    headers = [
        "Algorithm",
        "Sec.",
        "Type",
        "Role",
        "Run time (calc)",
        "Comm. time (net)",
        "Overall time",
        "Role comm. ovhd. (KiB)",
        "Total comm. ovhd. (KiB)",
    ]

    with output_path.open("w", encoding="utf-8") as file:
        file.write("# Reproduced Table 1: fPAKE Performance\n\n")
        file.write(
            "This table is generated from the benchmark `statistics.txt` "
            "files. Timing values are in seconds. Communication overhead "
            "values are in KiB.\n\n"
        )

        file.write("| " + " | ".join(headers) + " |\n")
        file.write("|" + "|".join(["---"] * len(headers)) + "|\n")

        for row in rows:
            file.write(
                "| "
                + " | ".join(str(row[header]) for header in headers)
                + " |\n"
            )

        file.write(
            "\n`Total comm. ovhd.` is the sum of Sender and Receiver "
            "communication overhead for the same algorithm, security level, "
            "and input type.\n"
        )


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate Table 1 CSV and Markdown files."
    )
    parser.add_argument(
        "--levels",
        nargs="+",
        choices=["128", "244"],
        default=["128", "244"],
        help="Security levels to include. Default: 128 244.",
    )
    args = parser.parse_args()

    repository_root = Path(__file__).resolve().parents[2]
    reproduced_dir = repository_root / "artifact" / "results" / "reproduced"
    reproduced_dir.mkdir(parents=True, exist_ok=True)

    rows = []

    for algorithm in ALGORITHMS:
        for level in args.levels:
            statistics_path = (
                    reproduced_dir
                    / f"{algorithm['folder_prefix']}-{level}"
                    / "statistics.txt"
            )

            if not statistics_path.is_file():
                raise FileNotFoundError(
                    f"Missing statistics file: {statistics_path}\n"
                    f"Run the corresponding benchmark before generating "
                    f"Table 1."
                )

            values = parse_statistics(statistics_path)
            validate_measurements(values, statistics_path)

            for input_type in INPUT_ORDER:
                total_communication = (
                        values[input_type]["sender"]["communication"]
                        + values[input_type]["receiver"]["communication"]
                )

                for role in ROLE_ORDER:
                    measurement = values[input_type][role]

                    rows.append(
                        {
                            "Algorithm": algorithm["name"],
                            "Sec.": level,
                            "Type": input_type,
                            "Role": role.capitalize(),
                            "Run time (calc)": format_number(
                                measurement["calc"]
                            ),
                            "Comm. time (net)": format_number(
                                measurement["net"]
                            ),
                            "Overall time": format_number(
                                measurement["overall"]
                            ),
                            "Role comm. ovhd. (KiB)": format_number(
                                measurement["communication"]
                            ),
                            "Total comm. ovhd. (KiB)": format_number(
                                total_communication
                            ),
                        }
                    )

    csv_path = reproduced_dir / "table1.csv"
    markdown_path = reproduced_dir / "table1.md"

    write_csv(rows, csv_path)
    write_markdown(rows, markdown_path)

    print(f"Created: {csv_path}")
    print(f"Created: {markdown_path}")


if __name__ == "__main__":
    main()