#!/usr/bin/env bash
# Regenerates the compiled Qt Help database (Resources/help/gfshelp.qch,
# Resources/help/gfshelp.qhc) from the HTML sources in Resources/doc/html/,
# so CI-built packages always ship help content matching that build's HTML,
# instead of whatever gfshelp.qch/qhc happen to be committed.
#
# Usage: generate_gfshelp.sh <cmake-build-dir>
# Must be run from the repository root, with the project already built
# (needs the gfshelpconfig-qhp-generator tool from that build dir) and
# CONDA_PREFIX pointing at an environment with qt6's qhelpgenerator.
set -euo pipefail

BUILD_DIR="${1:?usage: generate_gfshelp.sh <cmake-build-dir>}"

QHP_GENERATOR=$(find "$BUILD_DIR" -type f -name gfshelpconfig-qhp-generator | head -n1)
if [ -z "$QHP_GENERATOR" ]; then
    echo "generate_gfshelp.sh: gfshelpconfig-qhp-generator not found under $BUILD_DIR" >&2
    exit 1
fi

QHELPGENERATOR="$CONDA_PREFIX/lib/qt6/qhelpgenerator"
if [ ! -x "$QHELPGENERATOR" ]; then
    echo "generate_gfshelp.sh: qhelpgenerator not found at $QHELPGENERATOR" >&2
    exit 1
fi

# Rebuilds Resources/doc/html/gfshelpconfig.qhp from the current *.html files
# and Resources/doc/html/gfshelpconfig.toc (scans hrefs/keywords).
"$QHP_GENERATOR" .

# Rebuilds gfshelp.qch/gfshelp.qhc from the just-regenerated .qhp; -c checks
# for links that don't resolve to a file in the help project.
"$QHELPGENERATOR" Resources/doc/html/gfshelpconfig.qhcp -o Resources/doc/html/gfshelp.qhc -c

cp Resources/doc/html/gfshelp.qch Resources/doc/html/gfshelp.qhc Resources/help/
