# usage: run_case.ps1 <run dir> [rfluxmtx options]   (default "-ab 6 -c 5000 -n 16")
param([Parameter(Mandatory)][string]$RunDir, [string]$Opt = "-ab 6 -c 5000 -n 16")
$env:PATH = "C:\PROGRA~2\LBNL\WINDOW8.1\genBSDF;" + $env:PATH
$env:RAYPATH = ".;C:\PROGRA~2\LBNL\WINDOW8.1\genBSDF"
Push-Location $RunDir
Remove-Item Output.xml -ErrorAction SilentlyContinue
$t = Measure-Command { & C:\PROGRA~2\LBNL\WINDOW8.1\genBSDF\genfmtx.exe -w window.rad -ncp slats.rad -o Output.mtx -vb -s -rs kf -ss kf -refl -wrap -forw -opt $Opt 2>&1 | Out-File genfmtx.log; Add-Content genfmtx.log "recipe: $Opt" }
"$RunDir : $([int]$t.TotalSeconds) s, opt '$Opt', Output.xml $((Get-Item Output.xml -ErrorAction SilentlyContinue).Length) bytes"
Pop-Location
