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
# Resources/doc/html with bare filenames (matching GEMSGUI's own invocation)
# and -platform offscreen (qhelpgenerator links Qt6Widgets/Qt6Gui). Neither
# of those fixed a real hang seen twice on Windows CI (16+ min, zero output,
# had to be cancelled both times) - each theory cost a full CI cycle to
# disprove, so this run is instrumented instead of guessing a third time:
# stdin is explicitly redirected from NUL (a console app blocking forever
# reading stdin under a non-interactive CI session is a classic, distinct
# cause from either theory above) and the process is bounded to 120s, with
# whatever stdout/stderr it produced dumped either way - if it still times
# out, the partial output (or continued total silence) tells us whether it
# started doing real work at all, which neither previous attempt established.
$qhelpStdout = Join-Path $env:RUNNER_TEMP "qhelpgenerator-stdout.log"
$qhelpStderr = Join-Path $env:RUNNER_TEMP "qhelpgenerator-stderr.log"
Push-Location Resources/doc/html
$qhelpProc = Start-Process -FilePath $qhelpGenerator `
    -ArgumentList @("gfshelpconfig.qhcp", "-o", "gfshelp.qhc", "-platform", "offscreen") `
    -NoNewWindow -PassThru `
    -RedirectStandardInput "NUL" `
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
