To build ld online-demo, checkout AaltoASR and build and install it normally.

E.g.

    git checkout https://github.com/aalto-speech/AaltoASR.git
    cd AaltoASR
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=../dest ..
    make install

After this, give the installation prefix of AaltoASR as prefix:

    cd AaltoASR-online-demo
    mkdir build && cd build
    cmake -DAaltoASR_PREFIX=../../AaltoASR/dest ..
    make

In Debian based systems, you need to install the following packages:
    
    liblapack-dev
    libblas-dev
    libfftw3-dev
    libsndfile1-dev
    libsigc++-1.2-dev
    portaudio19-dev
    swig
    autoconf
    libtool
    libsdl-dev
    libexpat-dev
    libphysfs-dev
    g++
    fort77

Creating DEB package for demo laptops
=====================================

A .deb package containing all necessaties for the online demo can be created.

    git checkout https://github.com/aalto-speech/AaltoASR.git
    cd AaltoASR
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=../dest ..
    make install
    cd ../../
    git checkout git@github.com:aalto-speech/AaltoASR-online-demo.git
    cd AaltoASR-online-demo
    mkdir build && cd build
    cmake -DAaltoASR_PREFIX=../../AaltoASR/dest -DModel_DIR=/m/work/t40511_research/online-demo/models ..
    make package

Make sure the path for Model_DIR is correct.

This will create a package that can be installed on any Ubuntu laptop. After that, the demo can be started with the command 'startdemo'
