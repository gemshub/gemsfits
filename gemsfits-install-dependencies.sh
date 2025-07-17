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

if [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
   sudo ldconfig
fi

