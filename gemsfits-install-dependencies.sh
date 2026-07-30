#!/bin/bash
# Installing dependencies needed to build gemsfits on (k)ubuntu linux 20.04 or MacOS
# ejdb(ejdb2), yaml-cpp, nlopt, omp, boost, armadillo

if [ "$(uname)" == "Darwin" ]; then

    # Do under Mac OS X platform
    brew upgrade
    brew install cmake
    EXTN=dylib

elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then

    sudo apt install libarmadillo-dev libboost-all-dev libomp-12-dev
    EXTN=so

fi

#sudo rm -f /usr/local/lib/libnlopt.*
#sudo rm -f /usr/local/lib/libejdb.*
#sudo rm -f /usr/local/lib/libejdb2*.*
#sudo rm -f /usr/local/lib/libiwnet*.*
#sudo rm -f /usr/local/lib/libiowow*.*
#sudo rm -f /usr/local/lib/libyaml-cpp.*
#sudo rm -rf /usr/local/include/nlohmann
#sudo rm -rf /usr/local/include/eigen3/Eigen/Eigen
#sudo rm -rf /usr/local/include/pybind11
#sudo rm -rf /usr/local/include/spdlog
sudo rm -f  /usr/local/lib/libChemicalFun.$EXTN
sudo rm -f  /usr/local/lib/libThermoFun.$EXTN
sudo rm -f  /usr/local/lib/libGEMS3K.$EXTN

BRANCH_TFUN=master
BRANCH_GEMS3K=master

BUILD_TYPE=Release
threads=3
USING_EJDB=$1 #EJDB2 #$1 # OLD_EJDB
BASEDIR=${PWD}
echo "Script location: ${BASEDIR}"

# NLopt is a library for nonlinear local and global optimization
# https://github.com/stevengj/nlopt/archive/v2.7.0.tar.gz
test -f /usr/local/lib/libnlopt.$EXTN || {

        # Building nlopt library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/stevengj/nlopt.git && \
                cd nlopt && \
                mkdir -p build && \
                cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release && \
                make && \
                sudo make install

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}

# EJDB2 is an embeddable JSON database engine.
if [ "$USING_EJDB" == "OLD_EJDB" ];
  then

  echo "Using ejdb1"
  # git checkout v1.2.12
  test -f /usr/local/lib/libejdb.$EXTN || {

          # Building ejdb library
          mkdir -p ~/code && \
                  cd ~/code && \
                  git clone https://github.com/Softmotions/ejdb.git -b  v1.2.12 && \
                  cd ejdb && \
                  mkdir -p build && \
                  cd build && \
                  cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release && \
                  make && \
                  sudo make install

          # Removing generated build files
          cd ~ && \
                   rm -rf ~/code
  }

  else

  echo "Using ejdb2"
  test -f /usr/local/lib/libejdb2.$EXTN || {

        CMAKE_PATH=/home/sveta/Install/bin/cmake
        # Building ejdb library
        # https://geeksww.com/tutorials/operating_systems/linux/installation/downloading_compiling_and_installing_cmake_on_linux.php
        # /home/sveta/Install/bin/cmake ..  -DCMAKE_BUILD_TYPE=Release
        # git clone https://github.com/Softmotions/iowow.git
        # git clone https://github.com/Softmotions/iowow.git iowow_changed
        # insert changes into
        # diff -ruN iowow iowow_changed > iowow.patch
        # patch -p0 < iowow.patch
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/Softmotions/iowow.git  && \
                patch -p0 < "${BASEDIR}/iowow.patch" && \
                cd iowow && \
                mkdir -p build && \
                cd build && \
                cmake ..  -DCMAKE_BUILD_TYPE=Release && \
                sudo make install
                git clone https://github.com/Softmotions/iwnet.git  && \
                cd iwnet && \
                mkdir -p build && \
                cd build && \
                $CMAKE_PATH ..  -DCMAKE_BUILD_TYPE=Release && \
                sudo make install
                git clone https://github.com/Softmotions/ejdb.git  && \
                cd ejdb && \
                mkdir -p build && \
                cd build && \
                $CMAKE_PATH ..  -DCMAKE_BUILD_TYPE=Release && \
                sudo make install

        # Removing generated build files
        cd ~ && \
                 sudo rm -rf ~/code
   }
fi

# YAMLCPP
# https://github.com/jbeder/yaml-cpp/releases/tag/yaml-cpp-0.6.3
test -f /usr/local/lib/libyaml-cpp.$EXTN || {

        # Building yaml-cpp library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/jbeder/yaml-cpp.git && \
                cd yaml-cpp && \
                mkdir -p build && \
                cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DYAML_CPP_BUILD_TOOLS=OFF -DYAML_CPP_BUILD_CONTRIB=OFF && \
                make -j $threads && \
                sudo make install

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}

# spdlog
# if no spdlog installed in /usr/local/include/spdlog (copy only headers)
test -d /usr/local/include/spdlog || {

        # Building spdlog library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/gabime/spdlog  && \
                cd spdlog/include && \
                sudo cp -r spdlog /usr/local/include

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}


if [ "$USING_THERMOFUN_MODE" == "NO_THERMOFUN" ];
  then
    echo "Using without ThermoFun calculations"

  else

# nlohmann/json
test -f /usr/local/include/nlohmann/json.hpp || {

        # Building yaml-cpp library
        mkdir -p ~/code && \
        cd ~/code && \
        git clone https://github.com/nlohmann/json.git && \
        cd json && \
        mkdir -p build && \
        cd build && \
        cmake .. -DCMAKE_BUILD_TYPE=Release -DJSON_BuildTests=OFF -DJSON_MultipleHeaders=ON && \
        make && \
        sudo make install

        # Removing generated build files
        cd ~ && \
        rm -rf ~/code
}

# Eigen3 math library (added for building and installing xGEMS)
# if not installed in /usr/local/include/eigen3)
test -d /usr/local/include/eigen3/Eigen || {

        # Building eigen library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://gitlab.com/libeigen/eigen.git -b 3.4.0 && \
                cd eigen && \
                mkdir -p build && \
                cd build && \
                cmake .. \
                make && \
                sudo make install

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}

# ChemicalFun library
# if no ChemicalFun installed in /usr/local/lib/ (/usr/local/include/ChemicalFun)
test -f /usr/local/lib/libChemicalFun.$EXTN || {

        # Building thermofun library
        mkdir -p ~/code && \
        cd ~/code && \
        git clone https://github.com/thermohub/chemicalfun.git -b $BRANCH_TFUN  && \
        cd chemicalfun && \
        mkdir -p build && \
        cd build && \
        cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=$BUILD_TYPE  && \
        make -j $threads && \
        sudo make install

        # Removing generated build files
        cd ~ && \
        rm -rf ~/code
}


# ThermoFun library
# if no ThermoFun installed in /usr/local/lib/libThermoFun.a (/usr/local/include/ThermoFun)
test -f /usr/local/lib/libThermoFun.$EXTN || {

        # Building thermofun library
        mkdir -p ~/code && \
        cd ~/code && \
        # git clone https://bitbucket.org/gems4/thermofun.git -b $BRANCH_TFUN && \
        git clone https://github.com/thermohub/thermofun.git -b $BRANCH_TFUN && \
        cd thermofun && \
        mkdir -p build && \
        cd build && \
        cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release && \
        make -j $threads && \
        sudo make install

        # Removing generated build files
        cd ~ && \
        rm -rf ~/code
}

fi

# GEMS3K library
# if no GEMS3K installed in /usr/local/lib/libGEMS3K.so (/usr/local/include/GEMS3K)
test -f /usr/local/lib/libGEMS3K.$EXTN || {

        # Building thermofun library
        mkdir -p ~/code && \
        cd ~/code && \
        git clone https://github.com/gemshub/GEMS3K -b $BRANCH_GEMS3K && \
        cd GEMS3K && \
        mkdir -p build && \
        cd build && \
        cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release && \
        make -j $threads && \
        sudo make install

        # Removing generated build files
        cd ~ && \
        rm -rf ~/code
}


if [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
   sudo ldconfig
fi

