# Venetian blinds: validating the optical engines against Radiance

Started 2026-10-07 on branch `venetian-radiance-validation`. The scripts in `scripts/`
regenerate everything; the Radiance runs themselves (about 850 KB of XML per case) are not
in the repository but on the author's machine (`D:\Documents\Venetian-Radiance Validation\runs`).

A printable PDF is produced with pandoc and Typst from the repository root:
`pandoc docs/validation/venetian_radiance.md -f gfm -t pdf --pdf-engine=typst --shift-heading-level-by=-1 -H docs/validation/scripts/pdf_header.typ -V papersize=us-letter -V margin-x=0.7in -V margin-y=0.8in -V fontsize=10pt -o venetian_radiance.pdf`.

## Why

After the WCE DirectionalDiffuse fix (Windows-CalcEngine 1.0.77, WINDOW 8.1.67) the
legacy (LayerOptics) and WCE venetian models agree to 4e-7 in integrated values, but
both share the ISO 15099 slat-enclosure idealisation: two-dimensional cell, five equal
segments per slat, Lambertian faces, and the direct-direct term evaluated at the Klems
patch centre. Their angular distributions still differ from each other by up to 0.019
per incoming patch at 70 degrees. A ray-traced BSDF shares none of those idealisations
and is the independent reference for the angular part, which the ISO 15099 Python
reference (`D:\Documents\Results Change\scripts`) cannot check.

## Tooling

WINDOW ships a Radiance subset in `C:\Program Files (x86)\LBNL\WINDOW8.1\genBSDF`:
`genfmtx.exe` (a frads front end around `rfluxmtx`), the Klems `.cal` files and
`wrapBSDF`. The invocation is copied from WINDOW's own awning run
(`C:\Users\Public\LBNL\WINDOW8.1\AwningBSDF\Awning.bat`); see `scripts/run_case.ps1`:

```
genfmtx -w window.rad -ncp slats.rad -o Output.mtx -vb -s -rs kf -ss kf -refl -wrap -forw -opt "-ab 12 -c 20000 -n 16"
```

- `PATH` and `RAYPATH` must use the 8.3 path `C:\PROGRA~2\LBNL\WINDOW8.1\genBSDF`, and
  the run must happen inside the case folder with relative file names: `rfluxmtx` builds
  unquoted command lines, so any path containing a space fails.
- `-rs kf -ss kf`: Klems full basis in and out; `-forw`: both sides; `-refl`: reflection as
  well; `-opt`: options passed to `rfluxmtx` (12 ambient bounces, 20000 rays per sender patch, 16 threads).
  Exploration runs used `-ab 6 -c 5000` (25 to 50 s per case); the official runs take about four times longer.
- Output: `Output.xml` (BSDF-v1.4, `IncidentDataStructure` "Columns", four `WavelengthData`
  blocks, values in 1/sr), read by `scripts/compare_radiance.py`. The `sol_*` text files
  found in the awning folder are not produced by this invocation.

## Slat geometry

Every case uses the same blind; only the slat material (and, for v3, the curvature) changes.

| Parameter | Value | Note |
|---|---|---|
| Slat width (chord) | 16 mm | WINDOW `m_SlatWidth`; WCE `slatWidth` 0.016 m |
| Slat spacing (pitch) | 12 mm | distance between the pivot lines of neighbouring slats; WCE `slatSpacing` 0.012 m |
| Tilt | +45 deg | slat rises from the exterior edge to the interior edge (WCE and legacy convention for positive tilt) |
| Curvature | flat (v0, v1, v2, v4, v5); rise 1.0 mm (v3) | radius (rise^2 + (w/2)^2) / (2 rise) = 32.5 mm, crown up, as a positive radius in the engines |
| Slat thickness | 0 | the engines model zero-thickness slats; Radiance uses single polygons |
| Segments per slat (engines) | 5 | WINDOW default `VenetianNsegments`; WCE `numOfSlatSegments` |
| Method (engines) | Directional diffuse | WINDOW `VenetianSOLCalcMethod` 2; WCE `DistributionMethod::DirectionalDiffuse` |
| Angular basis | Klems full, 145 x 145 | both engines and Radiance |
| Slat faces | up face = material front (Rf), down face = material back (Rb) | same in WINDOW, WCE, legacy and in the Radiance model |

Vertical cross-section, exterior on the right (+z), looking along the slats (x):

```
   y (up)                 interior            exterior
   ^                      (window side, -z)   (+z)
   |
   |   interior edge  o
   |   (high)          \   up face = material front (Rf)
   |                    \
   |                     \   slat n+1, chord 16 mm, tilt 45 deg
   |                      \
   |                       o  exterior edge (low)
   |   pitch 12 mm
   |                   o
   |                    \   down face = material back (Rb)
   |                     \
   |                      \   slat n
   |                       \
   |                        o
   +------------------------------------------------------> z (toward exterior)
 window plane (z = 0)                light from the exterior arrives travelling toward -z
```

(The sketch is schematic; at 45 deg the slat's horizontal depth and vertical rise are both
16 mm x cos 45 = 11.3 mm, so adjacent slats overlap in projection and there is no direct
line of sight at normal incidence except through the small gap: dir-dir transmittance
about 0.057 at normal incidence in every engine and in Radiance.)

Radiance model (`scripts/make_slats.py`): window polygon 1.2 m x 1.5 m in the x-y plane at
z = 0; slats as polygons running along x, interior edge at z = 1 mm in front of the window
plane, the stack extending 10 cm beyond the window on every side (142 slats); for v3 each
slat is a 12-facet polygon strip along the circular arc. Only window-side (-z) incidence is
sampled directly; exterior incidence comes from the mirrored-tilt run (see below).

## Radiance model details (`scripts/make_slats.py`)

- Window polygon 1.2 m x 1.5 m in the x-y plane at z = 0; the exterior is +z.
- Slats run along x; slats and stack extend 10 cm beyond the window on every side, so the
  window never sees the end of a slat or of the stack (edge effects are otherwise visible:
  a stack ending at the window edge changed hemispherical T by 0.008 between the two
  tilt signs).
- Positive tilt lifts the interior edge (WCE convention). The polygon vertex order is chosen
  so the surface normal points up: the up face is the material "front" (Rf), the down face
  the "back" (Rb), matching WINDOW, WCE and legacy.
- Materials: `plastic R R R 0 0` (Lambertian). Asymmetric faces: two plastics combined with
  `mixfunc ... "if(Rdot,1,0)" .` (foreground material on the side the normal points to).
  Translucent slats: `trans` with colour R + T and `trans` parameter T/(R+T), no specular
  parts.
- Curved slats: polygon strip (`--rise`, `--facets`), crown up for a positive rise, as a positive radius in the engines.

## Cases

| Case | Rf | Rb | T | Notes |
|---|---|---|---|---|
| v0 | 0.5 | 0.5 | 0 | control; `v0_c20000` repeats it with 20000 samples for the noise floor |
| v1 | 0.8 | 0.2 | 0 | asymmetric opaque |
| v2 | 0.2 | 0.8 | 0 | mirror material of v1 |
| v3 | 0.5 | 0.5 | 0 | curved slats, rise 1.0 mm (radius 32.5 mm), 12 facets in Radiance |
| v4 | 0.5 | 0.5 | 0.2 | translucent symmetric |
| v5 | 0.7 | 0.2 | 0.2 | translucent asymmetric |

Slats 16 mm wide, 12 mm spacing, 45 degree tilt in every case; flat except v3. Each case also has a
`<case>_mirror` run with the tilt negated (see below). WINDOW debug matrices for the same
cases (legacy and WCE 1.0.77, 0.54 um) are in `D:\Documents\Results Change\results\<case>`
(`Tarcog\Layer2_*.csv`, `WinCalc\Solar\Layer_2\*.csv`).

## Conventions of genfmtx, established experimentally

Test runs `runs/test_plates` and `runs/test_mixfunc`.

- **genfmtx's "Front" is the window side (-z)**, our interior; WINDOW's front is the exterior.
  Two opaque plates, 0.8 toward +z in front of 0.2: genfmtx reports "Reflection Front" 0.197
  and "Reflection Back" 0.774. So genfmtx Front maps to our back, and Back to our front.
- **`mixfunc "if(Rdot,1,0)"` puts the foreground material on the side the surface normal
  points to** (a plate with normal +z shows 0.8 from +z). Our slat polygons have up-pointing
  normals, so the foreground material is the up face, WINDOW's "front" slat face (Rf).
- **Matrix layout.** "Columns" data reshaped column-major is (incoming, outgoing); it is
  transposed to WINDOW's (outgoing, incoming). Transposing a reciprocal transmission matrix
  swaps its front/back meaning, which is why the side swap above was invisible in T and
  visible in R.
- **The Klems azimuth runs the other way round** (phi -> -phi) relative to WINDOW's dumps; no
  azimuth offset. Determined on v1 and v5, the only cases that can discriminate; with it the
  reflection rings of the symmetric cases also fall into place.
- **Only the window-side results are used.** The window-side sender is the window polygon;
  the +z-side sender is derived from the geometry extent and scales with it. Exterior-incidence
  quantities are therefore taken from a run of the mirrored blind (tilt -45) seen from the
  window side (`--mirror=` option of `compare_radiance.py`).
- **Noise floor** (v0 at 5000 and 20000 samples): about 0.002 per patch at normal incidence,
  0.0005 in hemispherical values.
- **Ambient bounces are not a bias.** v1 at `-ab 12` reproduces `-ab 6` to 0.0003 in every
  integrated value at the same run time (`runs/v1_ab12`): path termination is governed by
  the ray-weight cutoff, not by `-ab`. The official reference runs use `-ab 12 -c 20000`.

## Findings

### Direct-direct term at grazing incidence (all cases)

The engines evaluate the beam cut-off at the Klems patch-centre direction and put
tau_dir/Lambda on the diagonal; Radiance averages over the patch. Near the cut-off profile
angle the gap fraction changes fast within a patch, so the per-patch direct-direct values
differ strongly at 70 and 82.5 degrees (ring 7, phi 22.5: Radiance 0.73 vs engines 0.95;
phi 0: 0.18 vs 0.06), by up to 0.09 at 50 degrees, and by 0.02 or less up to 40 degrees.
This is a resolution limit of the engines' method, identical in legacy and WCE, not a bug.
The comparisons below therefore separate the direct (diagonal) and diffuse parts.

### Integrated values and diffuse part (official runs, `-ab 12 -c 20000`)

The tables are produced by `scripts/summarize.py` from the run outputs and WINDOW's debug
matrices (legacy and WCE 1.0.77).

#### Integrated values: Radiance reference against WCE

Normal incidence is the directional-hemispherical value for the central Klems patch; hemispherical is the diffuse-diffuse value. Difference = WCE - Radiance. For the flat cases legacy equals WCE to 4e-7; the curved case is tabulated separately below.

**Front transmittance Tf**

| Case | Rad. normal | WCE normal | Diff. | Rad. hemisph. | WCE hemisph. | Diff. |
|---|---:|---:|---:|---:|---:|---:|
| v0 | 0.1338 | 0.1341 | +0.0003 | 0.2831 | 0.2796 | -0.0036 |
| v1 | 0.1360 | 0.1358 | -0.0002 | 0.2726 | 0.2686 | -0.0040 |
| v2 | 0.0978 | 0.0976 | -0.0002 | 0.2726 | 0.2686 | -0.0040 |
| v3 | 0.1312 | 0.1316 | +0.0004 | 0.2741 | 0.2710 | -0.0032 |
| v4 | 0.2648 | 0.2665 | +0.0016 | 0.3737 | 0.3717 | -0.0019 |
| v5 | 0.2514 | 0.2523 | +0.0009 | 0.3503 | 0.3478 | -0.0024 |

**Back transmittance Tb**

| Case | Rad. normal | WCE normal | Diff. | Rad. hemisph. | WCE hemisph. | Diff. |
|---|---:|---:|---:|---:|---:|---:|
| v0 | 0.1347 | 0.1341 | -0.0006 | 0.2828 | 0.2796 | -0.0032 |
| v1 | 0.0983 | 0.0976 | -0.0007 | 0.2722 | 0.2686 | -0.0036 |
| v2 | 0.1368 | 0.1358 | -0.0010 | 0.2723 | 0.2686 | -0.0037 |
| v3 | 0.1312 | 0.1312 | +0.0000 | 0.2738 | 0.2710 | -0.0028 |
| v4 | 0.2667 | 0.2665 | -0.0002 | 0.3734 | 0.3717 | -0.0017 |
| v5 | 0.2150 | 0.2145 | -0.0005 | 0.3500 | 0.3478 | -0.0021 |

**Front reflectance Rf**

| Case | Rad. normal | WCE normal | Diff. | Rad. hemisph. | WCE hemisph. | Diff. |
|---|---:|---:|---:|---:|---:|---:|
| v0 | 0.2643 | 0.2665 | +0.0022 | 0.2327 | 0.2354 | +0.0027 |
| v1 | 0.4085 | 0.4115 | +0.0029 | 0.3541 | 0.3572 | +0.0032 |
| v2 | 0.1059 | 0.1068 | +0.0009 | 0.0986 | 0.1002 | +0.0016 |
| v3 | 0.2792 | 0.2805 | +0.0012 | 0.2509 | 0.2526 | +0.0017 |
| v4 | 0.3304 | 0.3337 | +0.0033 | 0.2963 | 0.3010 | +0.0047 |
| v5 | 0.4381 | 0.4421 | +0.0040 | 0.3834 | 0.3884 | +0.0050 |

**Back reflectance Rb**

| Case | Rad. normal | WCE normal | Diff. | Rad. hemisph. | WCE hemisph. | Diff. |
|---|---:|---:|---:|---:|---:|---:|
| v0 | 0.2625 | 0.2665 | +0.0040 | 0.2328 | 0.2354 | +0.0026 |
| v1 | 0.1052 | 0.1068 | +0.0017 | 0.0986 | 0.1002 | +0.0016 |
| v2 | 0.4053 | 0.4115 | +0.0062 | 0.3542 | 0.3572 | +0.0031 |
| v3 | 0.2557 | 0.2600 | +0.0043 | 0.2241 | 0.2273 | +0.0032 |
| v4 | 0.3284 | 0.3337 | +0.0053 | 0.2964 | 0.3010 | +0.0046 |
| v5 | 0.1375 | 0.1398 | +0.0022 | 0.1363 | 0.1390 | +0.0027 |

#### v3, R 0.5 / 0.5, T 0, curved (rise 1 mm): Radiance against WCE and legacy

| Quantity | Radiance | WCE | WCE - Radiance | Legacy | Legacy - Radiance |
|---|---:|---:|---:|---:|---:|
| Tf, normal | 0.1312 | 0.1316 | +0.0004 | 0.1217 | -0.0095 |
| Tb, normal | 0.1312 | 0.1312 | +0.0000 | 0.1250 | -0.0063 |
| Rf, normal | 0.2792 | 0.2805 | +0.0012 | 0.2805 | +0.0012 |
| Rb, normal | 0.2557 | 0.2600 | +0.0043 | 0.2758 | +0.0202 |
| Tf, hemispherical | 0.2741 | 0.2710 | -0.0032 | 0.2683 | -0.0058 |
| Tb, hemispherical | 0.2738 | 0.2710 | -0.0028 | 0.2683 | -0.0054 |
| Rf, hemispherical | 0.2509 | 0.2526 | +0.0017 | 0.2526 | +0.0017 |
| Rb, hemispherical | 0.2241 | 0.2273 | +0.0032 | 0.2355 | +0.0114 |

#### Diffuse part of the directional-hemispherical value, per incoming ring

Largest |WCE - Radiance| over the patches of each ring (ring centre theta in the header). The diffuse part is the directional-hemispherical value minus the direct-direct term.

**Front transmittance Tf**

| Case | 0° | 10° | 20° | 30° | 40° | 50° | 60° | 70° | 82.5° |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| v0 | 0.001 | 0.002 | 0.001 | 0.002 | 0.002 | 0.012 | 0.006 | 0.045 | 0.018 |
| v1 | 0.001 | 0.001 | 0.003 | 0.004 | 0.003 | 0.006 | 0.005 | 0.020 | 0.030 |
| v2 | 0.001 | 0.001 | 0.001 | 0.001 | 0.001 | 0.016 | 0.009 | 0.071 | 0.022 |
| v3 | 0.001 | 0.003 | 0.002 | 0.001 | 0.002 | 0.012 | 0.008 | 0.041 | 0.018 |
| v4 | 0.002 | 0.004 | 0.003 | 0.003 | 0.003 | 0.018 | 0.012 | 0.059 | 0.066 |
| v5 | 0.002 | 0.003 | 0.002 | 0.003 | 0.003 | 0.011 | 0.011 | 0.037 | 0.073 |

**Back transmittance Tb**

| Case | 0° | 10° | 20° | 30° | 40° | 50° | 60° | 70° | 82.5° |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| v0 | 0.000 | 0.002 | 0.002 | 0.002 | 0.001 | 0.011 | 0.007 | 0.044 | 0.018 |
| v1 | 0.000 | 0.001 | 0.001 | 0.001 | 0.001 | 0.015 | 0.009 | 0.069 | 0.023 |
| v2 | 0.000 | 0.001 | 0.003 | 0.004 | 0.003 | 0.006 | 0.005 | 0.021 | 0.030 |
| v3 | 0.001 | 0.002 | 0.002 | 0.002 | 0.002 | 0.005 | 0.005 | 0.044 | 0.016 |
| v4 | 0.001 | 0.003 | 0.003 | 0.003 | 0.003 | 0.018 | 0.011 | 0.062 | 0.067 |
| v5 | 0.001 | 0.003 | 0.002 | 0.002 | 0.002 | 0.019 | 0.012 | 0.071 | 0.036 |

**Front reflectance Rf**

| Case | 0° | 10° | 20° | 30° | 40° | 50° | 60° | 70° | 82.5° |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| v0 | 0.002 | 0.004 | 0.005 | 0.006 | 0.005 | 0.011 | 0.008 | 0.037 | 0.045 |
| v1 | 0.003 | 0.007 | 0.007 | 0.010 | 0.007 | 0.014 | 0.012 | 0.049 | 0.073 |
| v2 | 0.001 | 0.002 | 0.002 | 0.003 | 0.002 | 0.006 | 0.003 | 0.020 | 0.034 |
| v3 | 0.001 | 0.004 | 0.004 | 0.006 | 0.004 | 0.004 | 0.007 | 0.020 | 0.039 |
| v4 | 0.003 | 0.006 | 0.006 | 0.008 | 0.006 | 0.017 | 0.011 | 0.058 | 0.068 |
| v5 | 0.004 | 0.008 | 0.008 | 0.010 | 0.008 | 0.019 | 0.014 | 0.064 | 0.078 |

**Back reflectance Rb**

| Case | 0° | 10° | 20° | 30° | 40° | 50° | 60° | 70° | 82.5° |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| v0 | 0.004 | 0.007 | 0.005 | 0.004 | 0.004 | 0.011 | 0.008 | 0.038 | 0.045 |
| v1 | 0.002 | 0.003 | 0.002 | 0.002 | 0.002 | 0.006 | 0.004 | 0.020 | 0.034 |
| v2 | 0.006 | 0.012 | 0.008 | 0.006 | 0.006 | 0.014 | 0.012 | 0.051 | 0.074 |
| v3 | 0.004 | 0.008 | 0.005 | 0.005 | 0.005 | 0.007 | 0.010 | 0.028 | 0.048 |
| v4 | 0.005 | 0.010 | 0.006 | 0.005 | 0.006 | 0.017 | 0.012 | 0.060 | 0.068 |
| v5 | 0.002 | 0.004 | 0.003 | 0.002 | 0.002 | 0.011 | 0.007 | 0.036 | 0.064 |

#### Direct-direct term, per incoming ring

Largest |WCE - Radiance| of the direct-direct contribution (diagonal element times lambda). The flat cases share one geometry, so the flat control and the curved case suffice.

| Case | Prop. | 0° | 10° | 20° | 30° | 40° | 50° | 60° | 70° | 82.5° |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| v0 | Tf | 0.001 | 0.009 | 0.010 | 0.012 | 0.008 | 0.063 | 0.036 | 0.207 | 0.205 |
| v0 | Tb | 0.001 | 0.015 | 0.004 | 0.009 | 0.006 | 0.062 | 0.039 | 0.215 | 0.208 |
| v3 | Tf | 0.001 | 0.009 | 0.010 | 0.012 | 0.009 | 0.038 | 0.032 | 0.151 | 0.195 |
| v3 | Tb | 0.001 | 0.015 | 0.004 | 0.007 | 0.008 | 0.039 | 0.036 | 0.159 | 0.196 |


Reading:

- **Flat slats, all materials (v0, v1, v2, v4, v5):** at normal incidence transmittance
  agrees to 0.002 or better and reflectance to 0.002 to 0.006 (Radiance lower);
  hemispherical values agree to 0.002 to 0.005 with Radiance consistently higher in T and
  lower in R. That is a real but small model difference between a five-segment
  two-dimensional radiosity cell and a ray-traced blind, about 1.3 % relative, identical in
  legacy and WCE. The asymmetric cases confirm the face assignment: v1 and v2 are exact
  mirrors of each other in Radiance as in the engines.
- **Curved slats (v3):** WCE agrees with Radiance as well as for flat slats (normal-incidence
  Tf 0.1316 vs 0.1312, hemispherical 0.2710 vs 0.2741; Rb 0.2600 vs 0.2557). Legacy does
  not: its normal-incidence Tf is 0.1217 (0.0095 low), Tb 0.1250, and its Rb 0.2758 is 0.02
  high. The 0.01 difference between the engines for curved slats noted earlier is therefore a
  legacy error in the curved-slat view factors, and the statement in issue #1760 that WCE
  computes them correctly is supported.
- **Diffuse part per incoming patch:** within 0.010 up to 45 degrees and 0.019 up to 65
  degrees in every case; beyond that the engines' patch-centre evaluation of the beam
  cut-off dominates and per-patch comparison is not meaningful.

### WCE unit tests

`src/SingleLayerOptics/tst/units/venetian/VenetianRadianceReference.unit.cpp` builds each
case the way WINDOW does (DirectionalDiffuse, Klems full, five segments) and asserts
against `tst/data/radiance/<case>.csv` (145 rows: patch index, then dir-hem, direct and
diffuse part for Tf, Tb, Rf, Rb; provenance in the sibling `.txt`):

- hemispherical values and normal-incidence dir-hem within 0.008;
- diffuse part per incoming patch within 0.015 up to 45 degrees and 0.030 from 45 to 65;
- nothing asserted on the direct-direct part beyond 45 degrees, for the reason above.

Six cases: v0, v1, v2, v4, v5 flat and v3 curved. The tolerances are about 1.5 times the
observed differences.

## Open

- Legacy curved-slat view factors: quantify over tilt and rise, and decide whether the legacy engine is corrected or the difference is only documented (results-change document).

- Direct-direct patch-centre evaluation at grazing incidence: document as a known limitation
  of both engines, or patch-average the dir-dir term.
