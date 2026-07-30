# Windows counterpart to generate_gfshelp.sh - see that file for what/why.
#
# Usage: generate_gfshelp.ps1 -BuildDir <cmake-build-dir>
# Must be run from the repository root, with the project already built
# (needs the gfshelpconfig-qhp-generator.exe tool from that build dir).
param(
    [Parameter(Mandatory=$true)][string]$BuildDir
)
$ErrorActionPreference = "Stop"

# This step runs in its own shell process (separate from the build step that
# does `conda activate gemsfits`), so the conda env's DLL directories aren't
# on PATH here. Both generator exes are Qt-linked and silently fail to launch
# (no output, just a nonzero exit code) without Qt6Core.dll etc. resolvable.
$Env:PATH = "C:\Miniconda\envs\gemsfits\Library\lib\qt6\bin;C:\Miniconda\envs\gemsfits\Library\bin;$Env:PATH"

$qhpGenerator = Get-ChildItem -Path $BuildDir -Recurse -Filter "gfshelpconfig-qhp-generator.exe" -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $qhpGenerator) {
    Write-Error "generate_gfshelp.ps1: gfshelpconfig-qhp-generator.exe not found under $BuildDir"
    exit 1
}

# Matches the conda env layout already relied on elsewhere in
# deploy-windows.yml (see its "Check deploy" step).
$qhelpGenerator = "C:/Miniconda/envs/gemsfits/Library/lib/qt6/qhelpgenerator.exe"
if (-not (Test-Path $qhelpGenerator)) {
    Write-Error "generate_gfshelp.ps1: qhelpgenerator.exe not found at $qhelpGenerator"
    exit 1
}

# Rebuilds Resources/doc/html/gfshelpconfig.qhp from the current *.html files
# and Resources/doc/html/gfshelpconfig.toc (scans hrefs/keywords).
& $qhpGenerator.FullName "."
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Rebuilds gfshelp.qch/gfshelp.qhc from the just-regenerated .qhp. Deliberately
# no -c (link-check) - see generate_gfshelp.sh for why. Invoked from inside
# Resources/doc/html with bare filenames, matching GEMSGUI's own invocation
# LITERALLY (no extra flags at all - a prior attempt kept -platform offscreen
# bolted on here, which GEMSGUI's proven-working call doesn't have, so that
# was never actually a clean test of "just match GEMSGUI"; it's removed now
# since it's an unverified addition of our own and could itself be the
# problem, e.g. if the offscreen plugin isn't actually shipped in the Windows
# conda-forge Qt6 package). Redirecting stdin from a real empty file (closing
# it outright ruled out a stdin-block hang - CI run 2026-07-30 killed the
# process at the 120s bound below with confirmed-empty stdout AND stderr, so
# it wasn't waiting on input) and dumping the plugins/platforms directory
# listing below are both kept as standing diagnostics in case this call still
# hangs, so the next failure carries more evidence than the last one did.
Get-Item "C:\Miniconda\envs\gemsfits\Library\lib\qt6\plugins\platforms\*.dll" -ErrorAction SilentlyContinue |
    ForEach-Object { Write-Host "platform plugin: $($_.Name)" }

$qhelpStdout = Join-Path $env:RUNNER_TEMP "qhelpgenerator-stdout.log"
$qhelpStderr = Join-Path $env:RUNNER_TEMP "qhelpgenerator-stderr.log"
# PowerShell's -RedirectStandardInput resolves its argument as a real file via
# the current provider - it does NOT special-case "NUL" as cmd.exe's shell
# redirection does, so passing the literal string "NUL" tries (and fails) to
# resolve an actual file by that name relative to the current location. Use a
# real empty file instead, which gives the same immediate-EOF behavior.
$qhelpEmptyStdin = Join-Path $env:RUNNER_TEMP "qhelpgenerator-empty-stdin.txt"
New-Item -ItemType File -Path $qhelpEmptyStdin -Force | Out-Null
Push-Location Resources/doc/html
$qhelpProc = Start-Process -FilePath $qhelpGenerator `
    -ArgumentList @("gfshelpconfig.qhcp", "-o", "gfshelp.qhc") `
    -NoNewWindow -PassThru `
    -RedirectStandardInput $qhelpEmptyStdin `
    -RedirectStandardOutput $qhelpStdout -RedirectStandardError $qhelpStderr
$qhelpFinished = $qhelpProc.WaitForExit(120000)
Pop-Location

Write-Host "--- qhelpgenerator stdout ---"
Get-Content $qhelpStdout -ErrorAction SilentlyContinue
Write-Host "--- qhelpgenerator stderr ---"
Get-Content $qhelpStderr -ErrorAction SilentlyContinue

if (-not $qhelpFinished) {
    Write-Error "generate_gfshelp.ps1: qhelpgenerator did not exit within 120s - killing it. See stdout/stderr dumped above for whatever it managed to do before hanging."
    Stop-Process -Id $qhelpProc.Id -Force -ErrorAction SilentlyContinue
    exit 1
}
if ($qhelpProc.ExitCode -ne 0) { exit $qhelpProc.ExitCode }

Copy-Item Resources/doc/html/gfshelp.qch Resources/help/ -Force
Copy-Item Resources/doc/html/gfshelp.qhc Resources/help/ -Force
