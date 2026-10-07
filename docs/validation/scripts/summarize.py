"""Markdown tables for the notes: Radiance reference vs the engines, all cases.

usage: summarize.py <runs dir> <WINDOW results dir> [case ...]
       e.g. summarize.py runs "D:\\Documents\\Results Change\\results" v0 v1 v4 v5
Each case needs runs/<case> and runs/<case>_mirror (Output.xml) and the WINDOW debug
matrices results/<case>/{Tarcog,WinCalc}. Legacy and WCE agree to 4e-7, so one engine
column is printed.
"""

from __future__ import annotations

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
from compare_radiance import (PROPS, RING_EDGES, RING_PATCHES, candidates, dir_hem,  # noqa: E402
                              hemispherical, klems_lambda, load_bsdf_xml, load_text)

ORIENTATION = "  phi+  0 mirror"
LABELS = {"v0": "v0, R 0.5 / 0.5, T 0", "v1": "v1, Rf 0.8 / Rb 0.2, T 0",
          "v4": "v4, R 0.5 / 0.5, T 0.2", "v5": "v5, Rf 0.7 / Rb 0.2, T 0.2"}


def radiance_matrices(runs: Path, case: str) -> dict[str, np.ndarray]:
    window_side = load_bsdf_xml(runs / case / "Output.xml")
    mirrored = load_bsdf_xml(runs / f"{case}_mirror" / "Output.xml")
    raw = {"tf": mirrored["tb"], "rf": mirrored["rb"], "tb": window_side["tb"], "rb": window_side["rb"]}
    return {p: dict(candidates(m))[ORIENTATION] for p, m in raw.items()}


def engine_matrices(results: Path, case: str, wavelength: str = "0.540000") -> dict[str, np.ndarray]:
    return {p: load_text(results / case / "WinCalc" / "Solar" / "Layer_2" / f"{p.capitalize()}_{wavelength}.csv")
            for p in PROPS}


def main() -> int:
    runs, results = Path(sys.argv[1]), Path(sys.argv[2])
    cases = sys.argv[3:] or ["v0", "v1", "v4", "v5"]
    lam = klems_lambda()
    ring = np.repeat(np.arange(len(RING_PATCHES)), RING_PATCHES)
    centres = [0.0] + [(a + b) / 2 for a, b in zip(RING_EDGES[1:], RING_EDGES[2:])]

    print("### Integrated values, Radiance / engines (normal-incidence dir-hem, hemispherical)\n")
    print("| Case | Tf | Tb | Rf | Rb |")
    print("|---|---|---|---|---|")
    for case in cases:
        rad, eng = radiance_matrices(runs, case), engine_matrices(results, case)
        cells = []
        for p in PROPS:
            cells.append(f"{dir_hem(rad[p], lam)[0]:.4f} / {dir_hem(eng[p], lam)[0]:.4f}<br>"
                         f"{hemispherical(rad[p], lam):.4f} / {hemispherical(eng[p], lam):.4f}")
        print(f"| {LABELS.get(case, case)} | " + " | ".join(cells) + " |")

    print("\n### Diffuse part of the per-patch dir-hem, max |Radiance - engines| per incoming ring\n")
    print("| Case | Property | " + " | ".join(f"{c:.1f} deg" for c in centres) + " |")
    print("|---|---|" + "---|" * len(centres))
    for case in cases:
        rad, eng = radiance_matrices(runs, case), engine_matrices(results, case)
        for p in PROPS:
            r_diff = dir_hem(rad[p], lam) - np.diag(rad[p]) * lam
            e_diff = dir_hem(eng[p], lam) - np.diag(eng[p]) * lam
            diff = np.abs(r_diff - e_diff)
            cells = " | ".join(f"{diff[ring == r].max():.3f}" for r in range(len(RING_PATCHES)))
            print(f"| {case} | {p.capitalize()} | {cells} |")

    print("\n### Direct-direct part, max |Radiance - engines| per incoming ring (same geometry in every case)\n")
    rad, eng = radiance_matrices(runs, cases[0]), engine_matrices(results, cases[0])
    print("| Property | " + " | ".join(f"{c:.1f} deg" for c in centres) + " |")
    print("|---|" + "---|" * len(centres))
    for p in ("tf", "tb"):
        diff = np.abs(np.diag(rad[p]) * lam - np.diag(eng[p]) * lam)
        print(f"| {p.capitalize()} | " + " | ".join(f"{diff[ring == r].max():.3f}" for r in range(len(RING_PATCHES))) + " |")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
