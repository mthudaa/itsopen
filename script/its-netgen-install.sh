#!/bin/sh
# This script installs Netgen for Integrated Technology Suite (ITS)

cd $HOME
git clone https://github.com/RTimothyEdwards/netgen netgen-its
cd netgen-its
./configure
make
sudo make install