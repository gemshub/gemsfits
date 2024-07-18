# GEMSFITS 
Code and Graphical Shell for GEM Input Parameter Optimization and Inverse Modelling
Linux, OSX, Windows


## What GEMSFITS can do? 

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


## How to clone (download) the GEMSFITS source code?

*  Download gemsfits source code using git clone command:

```sh

git clone --recurse-submodules https://bitbucket.org/gems4/gemsfits.git

```


## How to build GEMSFITS Ubuntu Linux and MacOS?

* Make sure you have g++, CMake and [Qt6](https://doc.qt.io/qt-6/get-and-install-qt.html) installed. If not, install them:

```sh

sudo apt-get install g++ cmake git

```

For MacOS, make sure that Homebrew is installed (see [Homebrew web site](http://brew.sh) )
   

* Install Dependencies

In order to build the gemsfits library on Ubuntu Linux or MacOS, first execute the following (may ask your sudo password):

```sh

cd ~/gemsfits
./gemsfits-install-dependencies.sh  [ OLD_EJDB ]

```

* Build gemsfits applications

To build gemsfits and install it in your home directory or in the system directory (as in the example below), a typical sequence of commands can be executed in the terminal:

```sh
cd ~/gemsfits
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/home/<you>/Qt/6.5.0/gcc_64  [ -DUSE_OLD_EJDB=ON ]
make -j 4
```

If you are using Qt installed in your home folder you will probably encounter an error about missing gl.h when building with QT. To avoid this do:
```
sudo apt-get install build-essential libgl1-mesa-dev libxcb-xinerama0 libxcb-cursor0
```


### Compiling gemsfits applications in Qt Creator

* In Qt Creator open ~/gemsfits/gemsfit2/gemsfit2.pro
* Set the build folder to ~/gemsfits/gemsfit2-build
* In Qt Creator open ~/gemsfits/gfshell2/gemsfit2-gui.pro
* Set the build folder to ~/gemsfits/gfshell2-build
* After the successful compilation try to run gemsfit2-gui GUI from Qt Creator. 


### How to build gemsfits applications (on conda-devenv)

Before building `gemsfits`, make sure you have the `conda-devenv` tool installed. Otherwise, run the following command in a terminal (assuming here that `conda` is already installed!):

```sh
conda install conda-devenv
```

We will use this tool now to create the `gemsfits` conda environment which contains all the dependencies for building `gemsfits` code. In the terminal, do:

```sh
cd path/to/gemsfits
conda devenv
```

Once this step is successfully completed, you can activate the `gemsfits` conda environment as follows:

```sh
conda activate gemsfits
```

In order to build the 'gemsfits' library on Linux or MacOS, first execute the following:

```sh
./conda-install-dependencies.sh  [ OLD_EJDB ]
```

Now it's time to use `cmake` to configure and build the C++ applications:

```sh

cmake -S . -B build   [ -DUSE_OLD_EJDB=ON ]
cmake --build build --parallel
```

This step will produce (inside the created directory `gemsfits/build`):


### How to run gemsfits applications

* gemsfit3

To start the application, open a terminal window and execute:

```sh
cd ~/gemsfits/build/bin
./gemsfit3 ...

```

* To run Qt GUI launcher, execute the following (or use *gemsfits/gfshell2/gemsfit-gui.pro* with QtCreator):

```sh
cd ~/gemsfits/build/bin
./shellfit3
```


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
