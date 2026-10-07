"""Markdown tables for the notes: Radiance reference against the engines, all cases.

usage: summarize.py <runs dir> <WINDOW results dir> [case ...] [--wce-dumps=<dir>]

Each case needs runs/<case> and runs/<case>_mirror (Output.xml). The WCE column comes
from <dir>/wce_<reference file>.csv when the unit tests were run with
WCE_VENETIAN_DUMP_DIR=<dir> (same layout as the reference files), otherwise from WINDOW's
debug matrices results/<case>/WinCalc. The legacy column exists only where WINDOW's
results/<case>/Tarcog dump exists, and is shown only where it differs from WCE in the
integrated values (curved slats).
"""

from __future__ import annotations

import math
import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
from compare_radiance import (PROPS, RING_EDGES, RING_PATCHES, candidates, dir_hem,  # noqa: E402
                              klems_lambda, load_bsdf_xml, load_text)

ORIENTATION = "  phi+  0 mirror"
LABELS = {"v0": "v0, R 0.5 / 0.5, T 0", "v1": "v1, Rf 0.8 / Rb 0.2, T 0",
          "v2": "v2, Rf 0.2 / Rb 0.8, T 0", "v3": "v3, R 0.5 / 0.5, T 0, curved (rise 1 mm)",
          "v4": "v4, R 0.5 / 0.5, T 0.2", "v5": "v5, Rf 0.7 / Rb 0.2, T 0.2",
          "t0": "t0, tilt 0, R 0.5", "tm45": "tm45, tilt -45, Rf 0.8 / Rb 0.2",
          "t80": "t80, tilt 80, R 0.5", "wide": "wide, spacing 16 mm, R 0.5",
          "curved3": "curved3, rise 3 mm, Rf 0.8 / Rb 0.2", "t0trans": "t0trans, tilt 0, T 0.2, Rf 0.7 / Rb 0.2"}
REFERENCE_FILES = {"v0": "venetian_flat45_R0.5", "v1": "venetian_flat45_Rf0.8_Rb0.2",
                   "v2": "venetian_flat45_Rf0.2_Rb0.8", "v3": "venetian_curved45_rise1mm_R0.5",
                   "v4": "venetian_flat45_T0.2_R0.5", "v5": "venetian_flat45_T0.2_Rf0.7_Rb0.2",
                   "t0": "venetian_flat0_R0.5", "tm45": "venetian_flatm45_Rf0.8_Rb0.2",
                   "t80": "venetian_flat80_R0.5", "wide": "venetian_flat45_s16_R0.5",
                   "curved3": "venetian_curved45_rise3mm_Rf0.8_Rb0.2", "t0trans": "venetian_flat0_T0.2_Rf0.7_Rb0.2"}
NAMES = {"tf": "Front transmittance Tf", "tb": "Back transmittance Tb",
         "rf": "Front reflectance Rf", "rb": "Back reflectance Rb"}


class Columns:
    """Per-incoming-patch dir-hem and direct-direct part for tf, tb, rf, rb."""

    def __init__(self, dirhem: dict[str, np.ndarray], direct: dict[str, np.ndarray]):
        self.dirhem, self.direct = dirhem, direct

    @classmethod
    def from_matrices(cls, matrices: dict[str, np.ndarray], lam: np.ndarray) -> "Columns":
        return cls({p: dir_hem(m, lam) for p, m in matrices.items()},
                   {p: np.diag(m) * lam for p, m in matrices.items()})

    @classmethod
    def from_dump(cls, path: Path) -> "Columns":
        table = load_text(path)
        return cls({p: table[:, 1 + 3 * k] for k, p in enumerate(PROPS)},
                   {p: table[:, 2 + 3 * k] for k, p in enumerate(PROPS)})

    def hemispherical(self, p: str, lam: np.ndarray) -> float:
        return float((self.dirhem[p] * lam).sum() / math.pi)

    def diffuse(self, p: str) -> np.ndarray:
        return self.dirhem[p] - self.direct[p]


def radiance_columns(runs: Path, case: str, lam: np.ndarray) -> Columns:
    window_side = load_bsdf_xml(runs / case / "Output.xml")
    mirrored = load_bsdf_xml(runs / f"{case}_mirror" / "Output.xml")
    raw = {"tf": mirrored["tb"], "rf": mirrored["rb"], "tb": window_side["tb"], "rb": window_side["rb"]}
    return Columns.from_matrices({p: dict(candidates(m))[ORIENTATION] for p, m in raw.items()}, lam)


def engine_columns(results: Path, dumps: Path | None, case: str, lam: np.ndarray) -> Columns | None:
    dump = None if dumps is None else dumps / f"wce_{REFERENCE_FILES.get(case, case)}.csv"
    if dump is not None and dump.exists():
        return Columns.from_dump(dump)
    folder = results / case / "WinCalc" / "Solar" / "Layer_2"
    if folder.exists():
        return Columns.from_matrices({p: load_text(folder / f"{p.capitalize()}_0.540000.csv") for p in PROPS}, lam)
    return None


def legacy_columns(results: Path, case: str, lam: np.ndarray) -> Columns | None:
    folder = results / case / "Tarcog"
    if not folder.exists():
        return None
    return Columns.from_matrices({p: load_text(folder / f"Layer2_{p.capitalize()}_0.540000.csv") for p in PROPS}, lam)


def ring_maxima(values: np.ndarray, ring: np.ndarray) -> str:
    return " | ".join(f"{values[ring == r].max():.3f}" for r in range(len(RING_PATCHES)))


def main() -> int:
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    dumps = next((Path(a.split("=", 1)[1]) for a in sys.argv[1:] if a.startswith("--wce-dumps=")), None)
    runs, results = Path(args[0]), Path(args[1])
    wanted = args[2:] or list(REFERENCE_FILES)
    lam = klems_lambda()
    ring = np.repeat(np.arange(len(RING_PATCHES)), RING_PATCHES)
    centres = [0.0] + [(a + b) / 2 for a, b in zip(RING_EDGES[1:], RING_EDGES[2:])]

    data = {}
    for case in wanted:
        wce = engine_columns(results, dumps, case, lam)
        if wce is None:
            print(f"<!-- {case}: no WCE data (neither dump nor WINDOW matrices) -->")
            continue
        data[case] = (radiance_columns(runs, case, lam), wce, legacy_columns(results, case, lam))
    cases = list(data)

    print("### Integrated values: Radiance reference against WCE\n")
    print("Cases as in the Cases table. Normal is the directional-hemispherical value for the central "
          "Klems patch, hemisph. the diffuse-diffuse value; Diff. is WCE minus Radiance. "
          "Where WINDOW dumps exist, legacy equals WCE to 4e-7 for flat slats; curved cases are tabulated separately.\n")
    for p in PROPS:
        print(f"**{NAMES[p]}**\n")
        print("| Case | Rad. normal | WCE normal | Diff. | Rad. hemisph. | WCE hemisph. | Diff. |")
        print("|---|---:|---:|---:|---:|---:|---:|")
        for case in cases:
            rad, wce, _ = data[case]
            rn, wn = rad.dirhem[p][0], wce.dirhem[p][0]
            rh, wh = rad.hemispherical(p, lam), wce.hemispherical(p, lam)
            print(f"| {case} | {rn:.4f} | {wn:.4f} | {wn - rn:+.4f} | {rh:.4f} | {wh:.4f} | {wh - rh:+.4f} |")
        print()

    for case in cases:
        rad, wce, leg = data[case]
        if leg is None or max(abs(wce.hemispherical(p, lam) - leg.hemispherical(p, lam)) for p in PROPS) < 1e-3:
            continue
        print(f"### {LABELS.get(case, case)}: Radiance against WCE and legacy\n")
        print("| Quantity | Radiance | WCE | WCE - Radiance | Legacy | Legacy - Radiance |")
        print("|---|---:|---:|---:|---:|---:|")
        for p in PROPS:
            rn, wn, ln = rad.dirhem[p][0], wce.dirhem[p][0], leg.dirhem[p][0]
            print(f"| {p.capitalize()}, normal | {rn:.4f} | {wn:.4f} | {wn - rn:+.4f} | {ln:.4f} | {ln - rn:+.4f} |")
        for p in PROPS:
            rh, wh, lh = rad.hemispherical(p, lam), wce.hemispherical(p, lam), leg.hemispherical(p, lam)
            print(f"| {p.capitalize()}, hemispherical | {rh:.4f} | {wh:.4f} | {wh - rh:+.4f} | {lh:.4f} | {lh - rh:+.4f} |")
        print()

    header = "| Case | " + " | ".join(f"{c:g}°" for c in centres) + " |"
    rule = "|---|" + "---:|" * len(centres)
    print("### Diffuse part of the directional-hemispherical value, per incoming ring\n")
    print("Largest |WCE - Radiance| over the patches of each ring (ring centre theta in the header). "
          "The diffuse part is the directional-hemispherical value minus the direct-direct term.\n")
    for p in PROPS:
        print(f"**{NAMES[p]}**\n")
        print(header)
        print(rule)
        for case in cases:
            rad, wce, _ = data[case]
            print(f"| {case} | " + ring_maxima(np.abs(rad.diffuse(p) - wce.diffuse(p)), ring) + " |")
        print()

    print("### Direct-direct term, per incoming ring\n")
    print("Largest |WCE - Radiance| of the direct-direct contribution (diagonal element times lambda), "
          "front transmittance. The cut-off geometry differs with tilt, spacing and curvature.\n")
    print(header)
    print(rule)
    for case in cases:
        rad, wce, _ = data[case]
        print(f"| {case} | " + ring_maxima(np.abs(rad.direct["tf"] - wce.direct["tf"]), ring) + " |")
    print()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
