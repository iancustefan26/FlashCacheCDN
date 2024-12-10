#!/bin/bash

echo "Installing neccessary dependencies..."

sudo apt install build-essential
sudo apt install cmake
sudo apt-get install -y libcurl4-openssl-dev cmake build-essential
sudo apt install libgtest-dev
git clone https://github.com/dzilles/configparser
cd configparser
cmake .
make
make test
make install
sudo apt install nlohmann-json3-dev
