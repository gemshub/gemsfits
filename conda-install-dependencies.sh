#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
    EXTN=dylib
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    EXTN=so
fi

USING_EJDB=OLD_EJDB #$1 # OLD_EJDBr

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
  test -f /usr/local/lib/libejdb2.$EXTN || {

        echo "Using ejdb2"
        # Building ejdb library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/Softmotions/ejdb.git  && \
                cd ejdb && \
                mkdir -p build && \
                cd build && \
                cmake ..  -DCMAKE_BUILD_TYPE=Release && \
                sudo make install

        # Removing generated build files
        #cd ~ && \
        #         rm -rf ~/code
   }
fi

