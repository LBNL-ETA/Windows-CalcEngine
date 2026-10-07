# Venetian blinds: validating the optical engines against Radiance

Started 2026-10-07 on branch `venetian-radiance-validation`. The scripts in `scripts/`
regenerate everything; the Radiance runs themselves (about 850 KB of XML per case) are not
in the repository but on the author's machine (`D:\Documents\Venetian-Radiance Validation\runs`).

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

## Geometry (`scripts/make_slats.py`)

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
- Curved slats: polygon strip (`--rise`, `--facets`); not run yet.

## Cases

| Case | Rf | Rb | T | Notes |
|---|---|---|---|---|
| v0 | 0.5 | 0.5 | 0 | control; `v0_c20000` repeats it with 20000 samples for the noise floor |
| v1 | 0.8 | 0.2 | 0 | asymmetric opaque |
| v4 | 0.5 | 0.5 | 0.2 | translucent symmetric |
| v5 | 0.7 | 0.2 | 0.2 | translucent asymmetric |

Flat slats 16 mm wide, 12 mm spacing, 45 degree tilt in every case. Each case also has a
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

Legacy and WCE 1.0.77 agree with each other to 4e-7 in these quantities, so one engine
column is shown. The tables are produced by `scripts/summarize.py`.

**Integrated values, Radiance / engines** (first line: directional-hemispherical at normal incidence; second line: hemispherical)

| Case | Tf | Tb | Rf | Rb |
|---|---|---|---|---|
| v0, R 0.5 / 0.5, T 0 | 0.1338 / 0.1341<br>0.2831 / 0.2796 | 0.1347 / 0.1341<br>0.2828 / 0.2796 | 0.2643 / 0.2665<br>0.2327 / 0.2354 | 0.2625 / 0.2665<br>0.2328 / 0.2354 |
| v1, Rf 0.8 / Rb 0.2, T 0 | 0.1360 / 0.1358<br>0.2726 / 0.2686 | 0.0983 / 0.0976<br>0.2722 / 0.2686 | 0.4085 / 0.4115<br>0.3541 / 0.3572 | 0.1052 / 0.1068<br>0.0986 / 0.1002 |
| v4, R 0.5 / 0.5, T 0.2 | 0.2648 / 0.2665<br>0.3737 / 0.3717 | 0.2667 / 0.2665<br>0.3734 / 0.3717 | 0.3304 / 0.3337<br>0.2963 / 0.3010 | 0.3284 / 0.3337<br>0.2964 / 0.3010 |
| v5, Rf 0.7 / Rb 0.2, T 0.2 | 0.2514 / 0.2523<br>0.3503 / 0.3478 | 0.2150 / 0.2145<br>0.3500 / 0.3478 | 0.4381 / 0.4421<br>0.3834 / 0.3884 | 0.1375 / 0.1398<br>0.1363 / 0.1390 |

**Diffuse part of the per-patch directional-hemispherical value, max |Radiance - engines| per incoming ring**

| Case | Property | 0.0 deg | 10.0 deg | 20.0 deg | 30.0 deg | 40.0 deg | 50.0 deg | 60.0 deg | 70.0 deg | 82.5 deg |
|---|---|---|---|---|---|---|---|---|---|---|
| v0 | Tf | 0.001 | 0.002 | 0.001 | 0.002 | 0.002 | 0.012 | 0.006 | 0.045 | 0.018 |
| v0 | Tb | 0.000 | 0.002 | 0.002 | 0.002 | 0.001 | 0.011 | 0.007 | 0.044 | 0.018 |
| v0 | Rf | 0.002 | 0.004 | 0.005 | 0.006 | 0.005 | 0.011 | 0.008 | 0.037 | 0.045 |
| v0 | Rb | 0.004 | 0.007 | 0.005 | 0.004 | 0.004 | 0.011 | 0.008 | 0.038 | 0.045 |
| v1 | Tf | 0.001 | 0.001 | 0.003 | 0.004 | 0.003 | 0.006 | 0.005 | 0.020 | 0.030 |
| v1 | Tb | 0.000 | 0.001 | 0.001 | 0.001 | 0.001 | 0.015 | 0.009 | 0.069 | 0.023 |
| v1 | Rf | 0.003 | 0.007 | 0.007 | 0.010 | 0.007 | 0.014 | 0.012 | 0.049 | 0.073 |
| v1 | Rb | 0.002 | 0.003 | 0.002 | 0.002 | 0.002 | 0.006 | 0.004 | 0.020 | 0.034 |
| v4 | Tf | 0.002 | 0.004 | 0.003 | 0.003 | 0.003 | 0.018 | 0.012 | 0.059 | 0.066 |
| v4 | Tb | 0.001 | 0.003 | 0.003 | 0.003 | 0.003 | 0.018 | 0.011 | 0.062 | 0.067 |
| v4 | Rf | 0.003 | 0.006 | 0.006 | 0.008 | 0.006 | 0.017 | 0.011 | 0.058 | 0.068 |
| v4 | Rb | 0.005 | 0.010 | 0.006 | 0.005 | 0.006 | 0.017 | 0.012 | 0.060 | 0.068 |
| v5 | Tf | 0.002 | 0.003 | 0.002 | 0.003 | 0.003 | 0.011 | 0.011 | 0.037 | 0.073 |
| v5 | Tb | 0.001 | 0.003 | 0.002 | 0.002 | 0.002 | 0.019 | 0.012 | 0.071 | 0.036 |
| v5 | Rf | 0.004 | 0.008 | 0.008 | 0.010 | 0.008 | 0.019 | 0.014 | 0.064 | 0.078 |
| v5 | Rb | 0.002 | 0.004 | 0.003 | 0.002 | 0.002 | 0.011 | 0.007 | 0.036 | 0.064 |

**Direct-direct part, max |Radiance - engines| per incoming ring** (the geometry is the same in every case, so one case suffices)

| Property | 0.0 deg | 10.0 deg | 20.0 deg | 30.0 deg | 40.0 deg | 50.0 deg | 60.0 deg | 70.0 deg | 82.5 deg |
|---|---|---|---|---|---|---|---|---|---|
| Tf | 0.001 | 0.009 | 0.010 | 0.012 | 0.008 | 0.063 | 0.036 | 0.207 | 0.205 |
| Tb | 0.001 | 0.015 | 0.004 | 0.009 | 0.006 | 0.062 | 0.039 | 0.215 | 0.208 |

Reading: at normal incidence transmittance agrees to 0.002 or better in all cases and
reflectance to 0.002 to 0.005 (Radiance lower); hemispherical values agree to 0.002 to 0.005
with Radiance consistently higher in T and lower in R, a real but small model difference
between a five-segment two-dimensional radiosity cell and a ray-traced blind (about 1.3 %
relative). The diffuse part per incoming patch agrees to 0.010 or better up to 45 degrees
and to 0.019 up to 65 degrees; beyond that the engines' patch-centre evaluation of the
beam cut-off dominates and the comparison is not meaningful per patch.

### WCE unit tests

`src/SingleLayerOptics/tst/units/venetian/VenetianRadianceReference.unit.cpp` builds each
case the way WINDOW does (DirectionalDiffuse, Klems full, five segments) and asserts
against `tst/data/radiance/<case>.csv` (145 rows: patch index, then dir-hem, direct and
diffuse part for Tf, Tb, Rf, Rb; provenance in the sibling `.txt`):

- hemispherical values and normal-incidence dir-hem within 0.008;
- diffuse part per incoming patch within 0.015 up to 45 degrees and 0.030 from 45 to 65;
- nothing asserted on the direct-direct part beyond 45 degrees, for the reason above.

The tolerances are about 1.5 times the observed differences.

## Open

- Curved slats (v3): extend `make_slats.py --rise`, run, add a fifth test.
- Direct-direct patch-centre evaluation at grazing incidence: document as a known limitation
  of both engines, or patch-average the dir-dir term.
