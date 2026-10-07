"""Write Radiance geometry for a horizontal venetian blind in front of a window.

Conventions follow WINDOW's awning run (C:\\Users\\Public\\LBNL\\WINDOW8.1\\AwningBSDF):
the window polygon lies in the x-y plane at z = 0 (1.2 m x 1.5 m), the exterior
is +z, and the shading geometry sits at z > 0. Slats run along x and extend well
past the window so edge effects vanish; the stack extends above and below it.

Slat material faces follow WINDOW/WCE: "front" is the up-facing face (Rf), "back"
the down-facing face (Rb). Positive tilt lifts the interior edge (WCE convention).
Asymmetric faces use mixfunc on the hit side; translucent slats use trans with
Lambertian transmission. Curved slats are approximated by a polygon strip.

usage: make_slats.py <out dir> --rf 0.8 --rb 0.2 --t 0 [--width 0.016 --spacing 0.012
       --tilt 45 --rise 0 --facets 10]
"""

from __future__ import annotations

import argparse
import math
from pathlib import Path

WINDOW_RAD = "void polygon window1\n0\n0\n12\n  0 0 0\n  0 1.5 0\n  1.2 1.5 0\n  1.2 0 0\n"
MARGIN = 0.10                    # slats and stack extend this far beyond the window on every side, so
X_LEFT, X_RIGHT = -MARGIN, 1.2 + MARGIN    # the window never sees the end of a slat or of the stack.
Y_FIRST, Y_LAST = -MARGIN, 1.5 + MARGIN    # Only window-side (-z) results are used; genfmtx's +z-side
                                           # sender is derived from the geometry extent and is not used.
Z_INTERIOR = 0.001               # interior slat edge just in front of the window plane


def lambertian(name: str, reflectance: float, transmittance: float) -> str:
    if transmittance <= 0.0:
        return f"void plastic {name}\n0\n0\n5 {reflectance:.6f} {reflectance:.6f} {reflectance:.6f} 0 0\n"
    total = reflectance + transmittance
    return (f"void trans {name}\n0\n0\n7 {total:.6f} {total:.6f} {total:.6f} 0 0 "
            f"{transmittance / total:.6f} 0\n")


def materials(rf: float, rb: float, tau: float) -> tuple[str, str]:
    """Return (material definitions, name of the material to put on the slats)."""
    if math.isclose(rf, rb):
        return lambertian("slat", rf, tau), "slat"
    both = lambertian("slat_front", rf, tau) + lambertian("slat_back", rb, tau)
    # Rdot > 0 when the ray hits the side the surface normal points to (the up face)
    both += 'void mixfunc slat\n4 slat_front slat_back "if(Rdot,1,0)" .\n0\n0\n'
    return both, "slat"


def slat_profile(width: float, tilt_deg: float, rise: float, facets: int) -> list[tuple[float, float]]:
    """Points (y, z) of one slat cross-section, interior edge first, in the slat's own frame."""
    if rise <= 0.0:
        facets = 1
    radius = (rise**2 + (width / 2) ** 2) / (2 * rise) if rise > 0.0 else 0.0
    points = []
    for k in range(facets + 1):
        s = -width / 2 + width * k / facets          # position along the chord
        bulge = (math.sqrt(radius**2 - s**2) - (radius - rise)) if rise > 0.0 else 0.0
        points.append((s, bulge))                     # chord along local u, bulge along local v (up)
    tilt = math.radians(tilt_deg)
    # local u runs from interior edge (-) to exterior edge (+); rotate so the interior edge is higher
    return [(-u * math.sin(tilt) + v * math.cos(tilt), u * math.cos(tilt) + v * math.sin(tilt))
            for u, v in points]   # (y, z) with z toward exterior


def polygon(name: str, material: str, p1: tuple, p2: tuple) -> str:
    """Quad spanning x between two profile points; vertex order chosen so the normal has +y (up)."""
    (y1, z1), (y2, z2) = p1, p2
    verts = [(X_LEFT, y1, z1), (X_RIGHT, y1, z1), (X_RIGHT, y2, z2), (X_LEFT, y2, z2)]
    ax, ay, az = (v - u for u, v in zip(verts[0], verts[1]))
    bx, by, bz = (v - u for u, v in zip(verts[0], verts[2]))
    normal_y = az * bx - ax * bz
    if normal_y < 0:
        verts.reverse()
    body = "".join(f"  {x:.5f} {y:.6f} {z:.6f}\n" for x, y, z in verts)
    return f"{material} polygon {name}\n0\n0\n12\n{body}"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("out")
    parser.add_argument("--rf", type=float, required=True)
    parser.add_argument("--rb", type=float, required=True)
    parser.add_argument("--t", type=float, default=0.0)
    parser.add_argument("--width", type=float, default=0.016)
    parser.add_argument("--spacing", type=float, default=0.012)
    parser.add_argument("--tilt", type=float, default=45.0)
    parser.add_argument("--rise", type=float, default=0.0)
    parser.add_argument("--facets", type=int, default=10)
    args = parser.parse_args()

    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    (out / "window.rad").write_text(WINDOW_RAD, encoding="ascii")

    defs, name = materials(args.rf, args.rb, args.t)
    profile = slat_profile(args.width, args.tilt, args.rise, args.facets)
    z_shift = Z_INTERIOR - min(z for _, z in profile)
    pieces = [defs]
    count = 0
    y_centre = Y_FIRST
    while y_centre <= Y_LAST:
        shifted = [(y + y_centre, z + z_shift) for y, z in profile]
        for k, (p1, p2) in enumerate(zip(shifted[:-1], shifted[1:])):
            pieces.append(polygon(f"slat{count}_{k}", name, p1, p2))
        y_centre += args.spacing
        count += 1
    (out / "slats.rad").write_text("".join(pieces), encoding="ascii")
    print(f"{count} slats, {len(profile) - 1} facet(s) each -> {out / 'slats.rad'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
