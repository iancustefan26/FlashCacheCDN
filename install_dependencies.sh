#!/bin/bash

echo "Installing neccessary dependencies..."

sudo apt-get update
sudo apt install cmake
sudo apt-get upgrade cmake
sudo apt-get install -y libcurl4-openssl-dev cmake build-essential
