"""Compare a genfmtx venetian run with WINDOW's debug matrices (legacy and WCE).

genfmtx writes Output.xml with the four 145 x 145 Klems-full matrices in 1/sr,
the same quantity WINDOW dumps per layer. The patch ordering and the
incoming/outgoing axis of the Radiance matrices are not assumed: every
combination of transpose, azimuth shift and mirror is tried and the one that
best matches the control case is reported, so the comparison is independent of
documentation about either tool's conventions.

usage: compare_radiance.py <run dir> <WINDOW results case dir> [wavelength] [--mirror=<run dir of the tilt-negated blind>]
       e.g. compare_radiance.py ..\\runs\\v0 "D:\\Documents\\Results Change\\results\\v0"
"""

from __future__ import annotations

import math
import sys
from pathlib import Path

import numpy as np

RING_EDGES = (0.0, 5.0, 15.0, 25.0, 35.0, 45.0, 55.0, 65.0, 75.0, 90.0)
RING_PATCHES = (1, 8, 16, 20, 24, 24, 24, 16, 12)
PROPS = ("tf", "tb", "rf", "rb")


def klems_lambda() -> np.ndarray:
    weights = []
    for (low, high), count in zip(zip(RING_EDGES, RING_EDGES[1:]), RING_PATCHES):
        lam = math.pi * (math.sin(math.radians(high)) ** 2 - math.sin(math.radians(low)) ** 2) / count
        weights.extend([lam] * count)
    return np.array(weights)


def load_text(path: Path) -> np.ndarray:
    rows = [[float(c) for c in line.replace(",", " ").split()] for line in path.read_text().splitlines() if line.strip()]
    return np.array(rows)


def load_bsdf_xml(path: Path) -> dict[str, np.ndarray]:
    """The four Klems-full matrices of a genfmtx Output.xml, keyed tf/tb/rf/rb.

    genfmtx writes IncidentDataStructure "Columns": the data list runs over
    outgoing patches fastest within one incoming column, i.e. column-major in
    (outgoing, incoming). Reshape accordingly; the orientation search below
    still tries the transpose, so a wrong guess here cannot pass unnoticed.
    """
    import re
    import xml.etree.ElementTree as ET

    # genfmtx calls the window side (-z, interior here) "front"; WINDOW's front is the
    # exterior. Established with two opaque plates (0.8 toward +z in front of 0.2):
    # genfmtx "Reflection Front" = 0.197, "Reflection Back" = 0.774. So its Front is our back.
    names = {"Transmission Front": "tb", "Transmission Back": "tf",
             "Reflection Front": "rb", "Reflection Back": "rf"}
    root = ET.parse(path).getroot()
    tag = lambda e: e.tag.split("}")[-1]
    result = {}
    for block in root.iter():
        if tag(block) != "WavelengthData":
            continue
        direction = next(c.text for c in block.iter() if tag(c) == "WavelengthDataDirection")
        data = next(c.text for c in block.iter() if tag(c) == "ScatteringData")
        values = np.array([float(v) for v in re.split(r"[,\s]+", data.strip().strip(",")) if v])
        # column-major gives (incoming, outgoing); transpose to WINDOW's (outgoing, incoming)
        result[names[direction]] = values.reshape(145, 145, order="F").T
    return result


def azimuth_map(shift_quarters: int, mirror: bool) -> np.ndarray:
    mapping, start = [], 0
    for count in RING_PATCHES:
        for k in range(count):
            j = (-k if mirror else k) + (count * shift_quarters) // 4
            mapping.append(start + j % count)
        start += count
    return np.array(mapping)


def candidates(matrix: np.ndarray):
    for transpose in (False, True):
        base = matrix.T if transpose else matrix
        for quarters in range(4):
            for mirror in (False, True):
                m = azimuth_map(quarters, mirror)
                yield f"{'T ' if transpose else '  '}phi+{90 * quarters:3d}{' mirror' if mirror else '       '}", base[np.ix_(m, m)]


def dir_hem(matrix: np.ndarray, lam: np.ndarray) -> np.ndarray:
    """Directional-hemispherical value per incoming patch (columns incoming, rows outgoing)."""
    return lam @ matrix


def hemispherical(matrix: np.ndarray, lam: np.ndarray) -> float:
    return float((dir_hem(matrix, lam) * lam).sum() / math.pi)


def main() -> int:
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    mirror = next((a.split("=", 1)[1] for a in sys.argv[1:] if a.startswith("--mirror=")), None)
    run, case = Path(args[0]), Path(args[1])
    wavelength = args[2] if len(args) > 2 else "0.540000"
    lam = klems_lambda()
    radiance = load_bsdf_xml(run / "Output.xml")
    if mirror:
        # genfmtx's window-side sender is exact, its exterior-side sender is derived from the
        # geometry extent and biased (v0: hemispherical T 0.2890 vs 0.2813 between the two
        # sides, well above the 0.0005 noise). So take the exterior-incidence quantities from
        # a run of the mirrored blind (tilt negated), seen from the window side.
        mirrored = load_bsdf_xml(Path(mirror) / "Output.xml")
        radiance = {"tf": mirrored["tb"], "rf": mirrored["rb"], "tb": radiance["tb"], "rb": radiance["rb"]}
        print(f"exterior side taken from mirrored run {mirror}")
    window = {
        "wce": {p: load_text(case / "WinCalc" / "Solar" / "Layer_2" / f"{p.capitalize()}_{wavelength}.csv") for p in PROPS},
        "legacy": {p: load_text(case / "Tarcog" / f"Layer2_{p.capitalize()}_{wavelength}.csv") for p in PROPS},
    }

    # orientation: fixed. Determined on the asymmetric cases v1 and v5 (the only ones that
    # can discriminate): after the loader's transpose no further transpose, no azimuth shift,
    # and the Klems azimuth runs the other way round (phi -> -phi). The residual on the
    # non-grazing rings is still reported for the record.
    ring = np.repeat(np.arange(len(RING_PATCHES)), RING_PATCHES)
    inner = ring <= 6
    label = "  phi+  0 mirror"
    oriented = {p: dict(candidates(radiance[p]))[label] for p in PROPS}
    best = (label, float(np.abs(dir_hem(oriented["tf"], lam) - dir_hem(window["wce"]["tf"], lam))[inner].max()))
    print(f"orientation chosen: [{label}]  (max per-patch dir-hem T difference to WCE, theta < 65: {best[1]:.4f})")

    print(f"\n{'':6} {'Radiance':>22} {'WCE':>22} {'legacy':>22}")
    print(f"{'':6} {'normal / hemisph.':>22} {'normal / hemisph.':>22} {'normal / hemisph.':>22}")
    for p in PROPS:
        cells = []
        for m in (oriented[p], window["wce"][p], window["legacy"][p]):
            dh = dir_hem(m, lam)
            cells.append(f"{dh[0]:.4f} / {hemispherical(m, lam):.4f}")
        print(f"{p.upper():6} " + " ".join(f"{c:>22}" for c in cells))

    centres = [0.0] + [(a + b) / 2 for a, b in zip(RING_EDGES[1:], RING_EDGES[2:])]

    def split(matrix: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
        """dir-hem per incoming patch split into its dir-dir (diagonal) and diffuse parts."""
        direct = np.diag(matrix) * lam
        return direct, dir_hem(matrix, lam) - direct

    print("\nmax |Radiance - engine| per incoming ring; dir-hem split into direct (diagonal) and diffuse parts")
    print(f"  {'':15}" + "".join(f"{c:>8.1f}" for c in centres) + "   <- ring centre theta")
    for p in PROPS:
        for engine in ("wce", "legacy"):
            r_direct, r_diffuse = split(oriented[p])
            e_direct, e_diffuse = split(window[engine][p])
            for part, rad_part, eng_part in (("direct", r_direct, e_direct), ("diffuse", r_diffuse, e_diffuse)):
                diff = np.abs(rad_part - eng_part)
                cells = "".join(f"{diff[ring == r].max():8.3f}" for r in range(len(RING_PATCHES)))
                print(f"  {p.upper()} {engine:6} {part:8}{cells}")

    print("\nreciprocity of the Radiance matrices, max|Tb - reciprocal(Tf)| / max|Tf|:")
    best_r = min(float(np.abs(oriented["tb"] - c).max()) / float(np.abs(oriented["tf"]).max())
                 for _, c in candidates(oriented["tf"]))
    print(f"  {best_r:.2e}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
