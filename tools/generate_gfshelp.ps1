# Windows counterpart to generate_gfshelp.sh - see that file for what/why.
#
# Usage: generate_gfshelp.ps1 -BuildDir <cmake-build-dir>
# Must be run from the repository root, with the project already built
# (needs the gfshelpconfig-qhp-generator.exe tool from that build dir).
param(
    [Parameter(Mandatory=$true)][string]$BuildDir
)
$ErrorActionPreference = "Stop"

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

# Rebuilds gfshelp.qch/gfshelp.qhc from the just-regenerated .qhp; -c checks
# for links that don't resolve to a file in the help project.
& $qhelpGenerator Resources/doc/html/gfshelpconfig.qhcp -o Resources/doc/html/gfshelp.qhc -c
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Copy-Item Resources/doc/html/gfshelp.qch Resources/help/ -Force
Copy-Item Resources/doc/html/gfshelp.qhc Resources/help/ -Force
