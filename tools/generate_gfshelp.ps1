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
# no -c (link-check): on this Qt build it surfaces broken-link warnings via a
# QMessageBox instead of console output, which hangs CI forever with zero
# output waiting for a click that never comes (GEMSGUI's own qhelpgenerator
# invocation never uses -c either, for the same reason).
& $qhelpGenerator Resources/doc/html/gfshelpconfig.qhcp -o Resources/doc/html/gfshelp.qhc
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Copy-Item Resources/doc/html/gfshelp.qch Resources/help/ -Force
Copy-Item Resources/doc/html/gfshelp.qhc Resources/help/ -Force
