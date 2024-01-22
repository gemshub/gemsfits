# gemsfits
 
[![Build Status](https://dev.azure.com/mirondanro/gemshub/_apis/build/status/gemshub.gemsreaktoro?branchName=master)](https://dev.azure.com/mirondanro/gemshub/_build/latest?definitionId=7&branchName=master)

The launcher to start standalone GEMS3K [![Conda Forge](https://anaconda.org/conda-forge/gems3k/badges/version.svg)](https://anaconda.org/conda-forge/gems3k)

## Briefly about gemsfits

*  ...
*  Written in C++ and [Qt6](https://www.qt.io/).
*  Version: currently 2.0.0.


### License

*  gemsreaktoro is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*  gemsreaktoro is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
*  You should have received a copy of the GNU Lesser General Public License along with gemsreaktoro code. If not, see http://www.gnu.org/licenses/.


### How to clone (download) the gemsfits source code

*  Download gemsfits source code using git clone command:

```sh

git clone --recurse-submodules https://svetadmitrieva@bitbucket.org/gems4/gemsfits.git

```


### How to build gemsfits applications (on Ubuntu Linux)

* Make sure you have g++, cmake and git installed. If not, install them:

```sh

sudo apt-get install g++ cmake git

```

   For Mac OSX, make sure you have Xcode and Homebrew installed (see Homebrew web site ).

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

