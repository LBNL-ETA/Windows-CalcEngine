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
          "v2": "v2, Rf 0.2 / Rb 0.8, T 0", "v3": "v3, R 0.5 / 0.5, T 0, curved (rise 1 mm)",
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
    cases = sys.argv[3:] or ["v0", "v1", "v2", "v3", "v4", "v5"]
    lam = klems_lambda()
    ring = np.repeat(np.arange(len(RING_PATCHES)), RING_PATCHES)
    centres = [0.0] + [(a + b) / 2 for a, b in zip(RING_EDGES[1:], RING_EDGES[2:])]

    def legacy_matrices(case: str) -> dict[str, np.ndarray]:
        return {p: load_text(results / case / "Tarcog" / f"Layer2_{p.capitalize()}_0.540000.csv") for p in PROPS}

    data = {case: (radiance_matrices(runs, case), engine_matrices(results, case), legacy_matrices(case)) for case in cases}
    names = {"tf": "Front transmittance Tf", "tb": "Back transmittance Tb",
             "rf": "Front reflectance Rf", "rb": "Back reflectance Rb"}

    print("### Integrated values: Radiance reference against WCE\n")
    print("Normal incidence is the directional-hemispherical value for the central Klems patch; "
          "hemispherical is the diffuse-diffuse value. Difference = WCE - Radiance. "
          "For the flat cases legacy equals WCE to 4e-7; the curved case is tabulated separately below.\n")
    for p in PROPS:
        print(f"**{names[p]}**\n")
        print("| Case | Normal: Radiance | Normal: WCE | Difference | Hemispherical: Radiance | Hemispherical: WCE | Difference |")
        print("|---|---:|---:|---:|---:|---:|---:|")
        for case in cases:
            rad, wce, _ = data[case]
            rn, wn = dir_hem(rad[p], lam)[0], dir_hem(wce[p], lam)[0]
            rh, wh = hemispherical(rad[p], lam), hemispherical(wce[p], lam)
            print(f"| {LABELS.get(case, case)} | {rn:.4f} | {wn:.4f} | {wn - rn:+.4f} | {rh:.4f} | {wh:.4f} | {wh - rh:+.4f} |")
        print()

    # legacy and WCE differ per patch for every case (angular distribution); list only the
    # cases whose integrated values differ, i.e. the curved slats
    curved = [c for c in cases
              if max(abs(hemispherical(data[c][1][p], lam) - hemispherical(data[c][2][p], lam)) for p in PROPS) >= 1e-3]
    for case in curved:
        rad, wce, leg = data[case]
        print(f"### {LABELS.get(case, case)}: Radiance against WCE and legacy\n")
        print("| Quantity | Radiance | WCE | WCE - Radiance | Legacy | Legacy - Radiance |")
        print("|---|---:|---:|---:|---:|---:|")
        for p in PROPS:
            rn, wn, ln = dir_hem(rad[p], lam)[0], dir_hem(wce[p], lam)[0], dir_hem(leg[p], lam)[0]
            print(f"| {names[p]}, normal incidence | {rn:.4f} | {wn:.4f} | {wn - rn:+.4f} | {ln:.4f} | {ln - rn:+.4f} |")
        for p in PROPS:
            rh, wh, lh = hemispherical(rad[p], lam), hemispherical(wce[p], lam), hemispherical(leg[p], lam)
            print(f"| {names[p]}, hemispherical | {rh:.4f} | {wh:.4f} | {wh - rh:+.4f} | {lh:.4f} | {lh - rh:+.4f} |")
        print()

    header = "| Case | " + " | ".join(f"{c:.1f} deg" for c in centres) + " |"
    rule = "|---|" + "---:|" * len(centres)
    print("### Diffuse part of the directional-hemispherical value, per incoming ring\n")
    print("Largest |WCE - Radiance| over the patches of each ring (ring centre theta in the header). "
          "The diffuse part is the directional-hemispherical value minus the direct-direct term.\n")
    for p in PROPS:
        print(f"**{names[p]}**\n")
        print(header); print(rule)
        for case in cases:
            rad, wce, _ = data[case]
            diff = np.abs((dir_hem(rad[p], lam) - np.diag(rad[p]) * lam) - (dir_hem(wce[p], lam) - np.diag(wce[p]) * lam))
            print(f"| {LABELS.get(case, case)} | " + " | ".join(f"{diff[ring == r].max():.3f}" for r in range(len(RING_PATCHES))) + " |")
        print()

    print("### Direct-direct term, per incoming ring\n")
    print("Largest |WCE - Radiance| of the direct-direct contribution (diagonal element times lambda). "
          "The flat cases share one geometry, so the flat control and the curved case suffice.\n")
    print("| Case | Property | " + " | ".join(f"{c:.1f} deg" for c in centres) + " |")
    print("|---|---|" + "---:|" * len(centres))
    for case in [c for c in cases if c in ("v0", "v3")]:
        rad, wce, _ = data[case]
        for p in ("tf", "tb"):
            diff = np.abs(np.diag(rad[p]) * lam - np.diag(wce[p]) * lam)
            print(f"| {LABELS.get(case, case)} | {names[p]} | " + " | ".join(f"{diff[ring == r].max():.3f}" for r in range(len(RING_PATCHES))) + " |")
    print()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
