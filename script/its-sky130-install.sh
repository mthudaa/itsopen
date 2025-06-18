#!/bin/sh
# This script installs OpenPDK for sky130 PDK

cd $HOME
git clone git://opencircuitdesign.com/open_pdks open_pdks-its
cd open_pdks-its
./configure --enable-sky130-pdk --enable-sram-sky130 --with-sky130-variants= all --prefix=/usr/local
make
sudo make install
make veryclean
echo "export PDK_ROOT=/usr/local/share/pdk" >> ~/.bashrc
echo "export PDK=sky130A" >> ~/.bashrc
echo "export STD_CELL_LIBRARY=sky130_fd_sc_hd" >> ~/.bashrc
ln -s /usr/local/share/pdk/sky130A/libs.tech/xschem/xschemrc ~/.xschem
ln -s /usr/local/share/pdk/sky130A/libs.tech/ngspice/spinit ~/.xschem/simulations