#!/bin/sh
# This script install for Xschem

cd $HOME
git clone https://github.com/stefanschippers/xschem.git xschem-its
cd xschem-its
./configure --prefix=/usr/local
make
sudo make install

# create a home work dir for Xschem
mkdir -p ~/.xschem
mkdir -p ~/.xschem/simulations