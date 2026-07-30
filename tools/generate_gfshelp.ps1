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
# no -c (link-check) - see generate_gfshelp.sh for why.
#
# Invoked from inside Resources/doc/html with bare filenames (matching
# GEMSGUI's own qhelpgenerator invocation exactly), NOT from the repo root
# with a long relative path: qhelpgenerator hung on Windows CI for 16+
# minutes with zero output when called from repo root against
# "Resources/doc/html/gfshelpconfig.qhcp" - the working theory is it did
# something CWD-relative internally (a scan, temp files, ...) and repo root
# is orders of magnitude bigger than the small doc/html folder (GEMS3K
# submodule, build/, .git/ and all). -platform offscreen is kept as a
# secondary, low-risk safeguard against qhelpgenerator's Qt6Widgets/Qt6Gui
# linkage needing a headless platform plugin, not the primary fix.
Push-Location Resources/doc/html
& $qhelpGenerator gfshelpconfig.qhcp -o gfshelp.qhc -platform offscreen
$qhelpGeneratorExit = $LASTEXITCODE
Pop-Location
if ($qhelpGeneratorExit -ne 0) { exit $qhelpGeneratorExit }

Copy-Item Resources/doc/html/gfshelp.qch Resources/help/ -Force
Copy-Item Resources/doc/html/gfshelp.qhc Resources/help/ -Force
