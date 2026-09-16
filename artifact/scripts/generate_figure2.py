#!/usr/bin/env python3
"""Convert the CE Figure-2 benchmark data into CSV and a three-panel plot."""

from __future__ import annotations

import argparse
import csv
from pathlib import Path

import matplotlib.pyplot as plt

COLUMNS = [
    "n", "ell", "keygen_sec", "enc_sec", "condenc_sec", "conddec_sec",
    "reg_ciphertext_kb", "cond_ciphertext_kb", "proof_size_kb",
]


def read_dat(path: Path) -> list[dict[str, float]]:
    """Read whitespace-separated benchmark rows, ignoring headings/comments."""
    rows: list[dict[str, float]] = []
    for lineno, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        fields = line.split()
        if len(fields) < len(COLUMNS):
            # This is normally a textual column heading.
            continue
        try:
            values = [float(value) for value in fields[: len(COLUMNS)]]
        except ValueError:
            continue
        row = dict(zip(COLUMNS, values))
        row["n"] = int(row["n"])
        row["ell"] = int(row["ell"])
        rows.append(row)
    if not rows:
        raise ValueError(f"No numeric benchmark rows found in {path}")
    return rows


def write_csv(rows: list[dict[str, float]], path: Path) -> None:
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=COLUMNS)
        writer.writeheader()
        writer.writerows(rows)


def common_values(a: list[dict[str, float]], b: list[dict[str, float]], key: str) -> list[int]:
    return sorted({int(row[key]) for row in a} & {int(row[key]) for row in b})


def lookup(rows: list[dict[str, float]], x_key: str, x: int, fixed_key: str, fixed: int, y: str):
    vals = [row[y] for row in rows if int(row[x_key]) == x and int(row[fixed_key]) == fixed]
    return vals[0] if vals else None


def plot(ours: list[dict[str, float]], ab24: list[dict[str, float]], output: Path) -> None:
    plt.rcParams.update({"font.size": 12, "axes.grid": True, "grid.alpha": 0.35})
    # One panel per row keeps the data and legends readable in the artifact PDF.
    fig, axes = plt.subplots(3, 1, figsize=(8.4, 14.5), constrained_layout=True)
    ours_style = dict(color="#1557ff", marker="o", linewidth=1.6, markersize=6)
    ab_style = dict(color="black", marker="D", linewidth=1.2, markersize=5.5, fillstyle="none")

    # (a) Conditional decryption as a function of n, one curve per ell.
    ax = axes[0]

    for label, rows, style in (
            ("AB24", ab24, ab_style),
            ("Ours", ours, ours_style),
    ):
        for ell in sorted({int(row["ell"]) for row in rows}):
            selected = sorted(
                (row for row in rows if int(row["ell"]) == ell),
                key=lambda row: int(row["n"]),
            )

            ns = [int(row["n"]) for row in selected]
            times = [row["conddec_sec"] for row in selected]

            ax.plot(ns, times, label=f"{label}, ell={ell}", **style)
    ax.set_yscale("log")
    ax.set_xlabel("Input length n")
    ax.set_ylabel("Conditional-decryption time (sec.)")
    ax.set_title("(a) Conditional Decryption: Varying the Input Length", pad=10)
    ax.legend(loc="upper left", ncol=2, frameon=True, fontsize=9)


    # (b) Conditional decryption as a function of ell, one curve per n.
    ax = axes[1]

    for label, rows, style in (
            ("AB24", ab24, ab_style),
            ("Ours", ours, ours_style),
    ):
        for n in sorted({int(row["n"]) for row in rows}):
            selected = sorted(
                (row for row in rows if int(row["n"]) == n),
                key=lambda row: int(row["ell"]),
            )

            ells = [int(row["ell"]) for row in selected]
            times = [row["conddec_sec"] for row in selected]

            ax.plot(ells, times, label=f"{label}, n={n}", **style)

    ax.set_yscale("log")
    ax.set_xlabel("Maximum Hamming distance ell")
    ax.set_xlim(0, 16)
    ax.set_xticks(range(0, 17, 2))
    ax.set_ylabel("Conditional-decryption time (sec.)")
    ax.set_title("(b) Conditional Decryption: Varying the Hamming Distance", pad=10)
    ax.legend(loc="upper right", ncol=2, frameon=True, fontsize=9)

    # (c) Ciphertext sizes. Both data files are retained; use points that exist
    # in each benchmark, with ell=2 preferred as the common paper setting.
    ax = axes[2]
    preferred_ell = 2
    for label, rows, color, marker in (("Ours", ours, "#1557ff", "o"), ("AB24", ab24, "#cc2222", "D")):
        selected = [r for r in rows if int(r["ell"]) == preferred_ell]
        if not selected:
            selected = rows
        selected = sorted(selected, key=lambda r: int(r["n"]))
        # Retain one representative point for each n.
        by_n = {int(r["n"]): r for r in selected}
        ns = sorted(by_n)
        ax.plot(ns, [by_n[n]["reg_ciphertext_kb"] for n in ns],
                color=color, marker=marker, fillstyle="none", label=f"Reg. Enc. ({label})")
        ax.plot(ns, [by_n[n]["cond_ciphertext_kb"] for n in ns],
                color=color, marker=marker, label=f"Cond. Enc. ({label})")
    ax.set_xlabel("Input length n")
    ax.set_ylabel("Ciphertext size (KB)")
    ax.set_title("(c) Regular and Conditional-Encryption Ciphertext Size", pad=10)
    ax.legend(loc="upper left", ncol=2, frameon=True, fontsize=9)
    fig.savefig(output / "figure2.pdf", bbox_inches="tight")
    fig.savefig(output / "figure2.png", dpi=300, bbox_inches="tight")
    plt.close(fig)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--ours", type=Path, required=True)
    parser.add_argument("--ab24", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)
    ours, ab24 = read_dat(args.ours), read_dat(args.ab24)
    write_csv(ours, args.output_dir / "figure2_ours.csv")
    write_csv(ab24, args.output_dir / "figure2_ab24.csv")
    plot(ours, ab24, args.output_dir)


if __name__ == "__main__":
    main()
