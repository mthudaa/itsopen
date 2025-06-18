#!/bin/sh
# This script installs OpenPDK for GF180MCU PDK

cd $HOME
git clone git://opencircuitdesign.com/open_pdks open_pdks-its
cd open_pdks-its
./configure --enable-gf180mcu-pdk --enable-osu-sc-gf180mcu --with-gf180mcu-variants= all --prefix=/usr/local
make
sudo make install
make veryclean
echo "export PDK_ROOT=/usr/local/share/pdk" >> ~/.bashrc
echo "export PDK=gf180mcuD" >> ~/.bashrc
echo "export 180MCU_MODELS=PDK_ROOT/PDK/libs.tech/ngspice" >> ~/.bashrc
ln -s /usr/local/share/pdk/gf180mcuD/libs.tech/xschem/xschemrc ~/.xschem