#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
    EXTN=dylib
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    EXTN=so
fi

USING_EJDB=OLD_EJDB #EJDB2 #$1 # OLD_EJDB
BASEDIR=${PWD}
echo "Script location: ${BASEDIR}"

# EJDB2 is an embeddable JSON database engine.
if [ "$USING_EJDB" == "OLD_EJDB" ];
  then
  # git checkout v1.2.12
  test -f ${CONDA_PREFIX}/lib/libejdb.$EXTN || {

          # Building ejdb library
          mkdir -p ~/code && \
                  cd ~/code && \
                  git clone https://github.com/Softmotions/ejdb.git -b  v1.2.12 && \
                  cd ejdb && \
                  mkdir -p build && \
                  cd build && \
                  cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} && \
                  make && \
                  sudo make install

          # Removing generated build files
          cd ~ && \
                   rm -rf ~/code
  }
  else
  test -f ${CONDA_PREFIX}/lib/libejdb2.$EXTN || {

        echo "Using ejdb2"
        # Building ejdb library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/Softmotions/iowow.git && \
                patch -p0 < "${BASEDIR}/iowow.patch" && \
                cd iowow && \
                mkdir -p build && \
                cd build && \
                cmake ..  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} && \
                sudo make install && \
                git clone https://github.com/Softmotions/iwnet.git && \
                cd iwnet && \
                mkdir -p build && \
                cd build && \
                cmake ..  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} && \
                sudo make install && \
                git clone https://github.com/Softmotions/ejdb.git  && \
                cd ejdb && \
                mkdir -p build && \
                cd build && \
                cmake ..  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} && \
                sudo make install

        # Removing generated build files
        #cd ~ && \
        #         rm -rf ~/code
   }
fi

