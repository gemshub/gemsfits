# GEMSFITS #

Code and Graphical Shell for GEM Input Parameter Optimization and Inverse Modelling

## What GEMSFITS can do? ##

* Can fit internally consistent input parameters of chemical thermodynamic models against the experimental data
* Is coupled with the GEMS3K code for Gibbs Energy Minimization calculation of equilibrium states
* Allows for bound, (non-)linear (in-)equality constrained minimization of weighted squared residuals
* Written in C/C++, available with source code, completely parallelizable on HPC architectures
* Provides tools for statistical analysis with thorough evaluation of the fitted parameters uncertainty
* Results of fitting and statistics can be visualized and printed in various graphical formats using the graphical shell
* Can import, manage and query extensive sets of the experimental data accumulated in NoSQL database files

Learn more and contact us on [GEMSFITS web page](http://gems.web.psi.ch/GEMSFITS).

If you find GEMSFITS useful for your research applications, please cite the following papers:

* Miron G.D., Kulik D.A., Dmytrieva S.V., Wagner T. (2015): GEMSFITS: Code package for optimization of geochemical model parameters and inverse modeling. Applied Geochemistry 55, 28-45, [doi](http://dx.doi.org/10.1016/j.apgeochem.2014.10.013).
* Kulik D.A., Wagner T., Dmytrieva S.V., Kosakowski G., Hingerl F.F., Chudnenko K.V., Berner U. (2013): GEM-Selektor geochemical modeling package: revised algorithm and GEMS3K numerical kernel for coupled simulation codes. Computational Geosciences 17, 1-24, [doi](http://dx.doi.org/10.1007/s10596-012-9310-6).
* Wagner T., Kulik D.A., Hingerl F.F., Dmytrieva S.V. (2012): GEM-Selektor geochemical modeling package: TSolMod library and data interface for multicomponent phase models. Canadian Mineralogist 50, 1173-1195, [doi](http://dx.doi.org/10.3749/canmin.50.5.1173).

## How to download GEMSFITS source code? ##

In your home directory, make a folder named e.g. ~/gitGEMSFIT with two subfolders ~/gitGEMSFIT/gemsfits and ~/gitGEMSFIT/standalone.

### Clone the GEMSFITS repository ###

Change into ~/gitGEMSFIT/gemsfits folder and clone this repository from https://bitbucket.org/gems4/gemsfits.git using a preinstalled free git client SourceTree or SmartGit (the best way on Windows). 

Alternatively on Mac OS X or linux, open a terminal, cd ~/gitGEMSFIT/gemsfits and type in the command line (do not forget a period at the end):
~~~
git clone https://bitbucket.org/gems4/gemsfits.git .
~~~
Check that the active branch is master: in terminal, cd to ~/gitGEMSFIT/gemsfits and run a command 
~~~
git branch 
~~~
which should return "* master". If not, run the commands
~~~
git checkout -b master --track origin/master
git pull origin master
~~~

### Clone the GEMS3K repository ###

Change into ~/gitGEMSFIT/standalone folder and clone the GEMS3K repository from https://bitbucket.org/gems4/gems3k.git using the git client.

Alternatively on Mac OS X or linux terminal cd ~/gitGEMSFIT/standalone and run a command (do not forget a period at the end):
~~~
git clone https://bitbucket.org/gems4/gems3k.git . 
~~~

Switch the GEMS3K code repository to a git branch branches/devEJDB using the git client, or in terminal, run the commands
~~~
git checkout -b branches/devEJDB --track origin/branches/devEJDB
git pull origin branches/devEJDB
~~~

## How to build GEMSFITS on linux? ##

Make sure that you have the following packages installed (on debian-type linuxes such as Kubuntu 15.10) by running in the terminal:
~~~
$ sudo apt-get update
$ sudo apt-get install libarmadillo-dev libnlopt-dev libjansson-dev libboost-all-dev libiomp5 libyaml-cpp-dev
~~~
or install equivalent packages, if you have another linux desktop.

In the terminal, change into ~/gitGEMSFIT/standalone/EJDB to build and install the EJDB library using the following commands:
~~~
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make
sudo make install
~~~

Make sure that you have the complete Qt5 toolkit (with QtCreator) installed either in the system or locally after downloading from [qt.io](http://www.qt.io/download/) site. 

On latest versions of Linux, Qt5 is available for installation in the system. For example on Ubuntu 15.10 it can be installed using a command:
~~~
sudo apt-get install qt5-default qtcreator
~~~ 

If error "Unknown module(s) in QT:" "help" or "svg" appears, try the following command:
~~~
sudo apt-get install qttools5-dev libqt5svg5-dev
~~~

### Build the gemsfit2 code ###

In QtCreator, open a project  ~/gitGEMSFIT/gemsfits/gemsfit2/gemsfit2.pro and configure it to build release (or debug) into ~/gitGEMSFIT/gemsfits/gemsfit2-build folder. Then run qmake and build the gemsfit2 executable code. 

### Build and run the gemsfits code ###

In QtCreator, open a project  ~/gitGEMSFIT/gemsfits/gfshell2/gemsfit-gui.pro and configure it to build release (or debug) into ~/gitGEMSFIT/gemsfits/gfshell-build folder. Then run qmake and build the gemsfits executable code.

Launch the gemsfits code (without command-line parameters) from QtCreator and open its help window to learn how to use it. The gemsfit2 code will be called from within the gemsfits GUI code when necessary.

## TBD ###

## License ##

GEMSFITS is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

GEMSFITS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with the source code. If not, see <http://www.gnu.org/licenses/>.


### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact