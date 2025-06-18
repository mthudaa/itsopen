#!/bin/sh

cd $HOME
git clone git://git.code.sf.net/p/ngspice/ngspice ngspice-its
cd ngspice-its
./autogen.sh
mkdir release
cd release
../configure --with-x --enable-xspice --disable-debug --enable-cider --with-readline=yes --enable-openmp --enable-klu --enable-osdi
make 2>&1 | tee make.log
sudo make install