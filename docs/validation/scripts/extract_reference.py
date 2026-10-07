"""Turn a case's Radiance runs into the small reference file the WCE unit tests read.

For each of Tf, Tb, Rf, Rb the CSV holds, per incoming Klems-full patch (WINDOW
ordering, 145 rows, plain numbers): the directional-hemispherical value, its direct-direct
part (diagonal element times the patch's projected solid angle) and the diffuse remainder.
A sibling .txt records provenance and the hemispherical values. The side mapping and
orientation are those established in notes.md (window-side results only; exterior
incidence from the mirrored-tilt run).

usage: extract_reference.py <run dir> <mirrored run dir> <output csv> [--recipe="-ab 12 -c 20000 -n 16"]
       e.g. extract_reference.py runs/v1 runs/v1_mirror reference/v1.csv
"""

from __future__ import annotations

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
from compare_radiance import PROPS, candidates, dir_hem, hemispherical, klems_lambda, load_bsdf_xml  # noqa: E402

ORIENTATION = "  phi+  0 mirror"


def main() -> int:
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    recipe = next((a.split("=", 1)[1] for a in sys.argv[1:] if a.startswith("--recipe=")), None)
    run, mirror, out = Path(args[0]), Path(args[1]), Path(args[2])
    lam = klems_lambda()
    window_side = load_bsdf_xml(run / "Output.xml")
    mirrored = load_bsdf_xml(mirror / "Output.xml")
    matrices = {"tf": mirrored["tb"], "rf": mirrored["rb"], "tb": window_side["tb"], "rb": window_side["rb"]}
    matrices = {p: dict(candidates(m))[ORIENTATION] for p, m in matrices.items()}

    columns = {}
    for p in PROPS:
        total = dir_hem(matrices[p], lam)
        direct = np.diag(matrices[p]) * lam
        columns[f"{p}_dirhem"] = total
        columns[f"{p}_direct"] = direct
        columns[f"{p}_diffuse"] = total - direct

    # Plain numeric CSV so WCE's Helper::readMatrixFromCSV can read it: 145 rows (incoming
    # patch, WINDOW/WCE Klems-full ordering) x 13 columns: patch index, then for tf, tb, rf,
    # rb each: dir-hem, direct part, diffuse part. Provenance goes to the sibling .txt file.
    out.parent.mkdir(parents=True, exist_ok=True)
    newline = chr(10)
    with out.open("w", encoding="ascii", newline=newline) as f:
        for i in range(145):
            f.write(f"{i}," + ",".join(f"{columns[c][i]:.6f}" for c in columns) + "," + newline)
    hem = {p: hemispherical(matrices[p], lam) for p in PROPS}
    provenance = [
        f"source runs: {run.name}, {mirror.name}",
        f"recipe: {recipe or read_recipe(run)}",
        "columns: patch, " + ", ".join(columns),
        "hemispherical: " + ", ".join(f"{p} {hem[p]:.6f}" for p in PROPS),
    ]
    out.with_suffix(".txt").write_text(newline.join(provenance) + newline, encoding="ascii")
    print(f"{out}: " + "  ".join(f"{p} hem {hemispherical(matrices[p], lam):.4f}" for p in PROPS))
    return 0


def read_recipe(run: Path) -> str:
    log = run / "genfmtx.log"
    if not log.exists():
        return "unknown"
    for line in log.read_text(errors="replace").splitlines():
        if "-ab" in line and "-c" in line:
            start = line.find("-ab")
            return line[start:].split("'")[0].strip()
    return "see genfmtx.log"


if __name__ == "__main__":
    raise SystemExit(main())
