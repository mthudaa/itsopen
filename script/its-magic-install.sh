#!/bin/sh
# This script installs Magic VLSI Layout Tool

cd $HOME
git clone https://github.com/RTimothyEdwards/magic.git magic-its
cd magic-its
./configure --enable-cairo-offscreen
make
sudo make install