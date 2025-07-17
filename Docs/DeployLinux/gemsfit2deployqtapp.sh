#!/bin/bash
#
# DM: gemsfit2deployqtapp.sh - a modified/extended deployqtapp.sh from W.Hallatt 
#
#================================================================================
# Copyright (c) 2012 - 2013 by William Hallatt.
# 
# This script is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This script is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this script (GNUGPL.txt).  If not, see
#
#                    <http://www.gnu.org/licenses/>
# 
# Should you wish to contact me for whatever reason, please do so via:
#
#                 <http://www.goblincoding.com/contact>
# 
#================================================================================

# Copy your executable as well as this script to the directory where you 
# want to create the final tar ball.  To execute, simply pass the name of the
# executable as command line parameter to this script.
#
# Worth noting is that I built Qt from source myself, you may or may not need 
# additional plugins, etc and different or additional directory structures and
# will have to edit this script to suit your needs!

if [ $# -ne 1 ]
then
        echo "Usage: $0 <executable name>"
        exit 1
fi

executable=$1

# Obtain the Linux flavour and version.
distro=`lsb_release -d | awk '{print $2$3$4}' | sed 's/\./_/g'`

# Create the directory that will be tarred up for distribution.
etardir=`echo $executable"_"$distro | awk '{print tolower($0)}'`
tardir=gemsfits-app
mkdir $tardir
echo "Created tar ball directory: "$tardir

# Copy executable across.
chmod u+x $executable
cp $executable $tardir
echo "Copied executable "$executable" to "$tardir

# Create the libs directory.
libsdir=$PWD/$tardir/libs
mkdir $libsdir 
echo "Created libs directory: "$libsdir

# Copy all dependencies across to the tar directory.
echo "Copying dependencies..."

for dep in `ldd ./$executable | awk '{print $3}' | grep -v "("`
do
  cp $dep $libsdir
  echo "Copied dependency "$dep" to "$libsdir
done

# Create a README
echo "Creating README..."

readme=$tardir/README
echo "================================================================================" >> $readme
echo "GEMSFITS v1.3.3 $etardir" >> $readme
echo "Please, read Readme.Run.linux.txt file first, and do things as suggested there." >> $readme
echo "To have projects in /home/you/Library/GEMSFITS (default location," >> $readme 
echo "  launch once $executable via" >> $readme
echo "" >> $readme
echo "                 $execscript -d" >> $readme
echo "" >> $readme
echo "If you run into any trouble regarding dependencies, all you need to do is to" >> $readme
echo "run " >> $readme
echo "                 $fixscript " >> $readme
echo "" >> $readme
echo "in order to automatically resolve dependencies on your behalf " >> $readme
echo "(note that you will need administrator privileges for this as this script will" >> $readme
echo "download the necessary libraries for your platform). " >> $readme
echo "" >> $readme
echo "Should you wish to contact us for any reason, please do so via Contacts in:" >> $readme
echo "" >> $readme
echo "                 <http://gems.web.psi.ch/termsofuse>" >> $readme
echo "" >> $readme
echo "================================================================================" >> $readme

echo "Creating tarball..."
tar -zcvf $tardir".tgz" $tardir

echo "Cleaning up..."
rm -rf $tardir
echo "Done!"
