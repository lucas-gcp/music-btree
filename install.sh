#!/bin/sh

mkdir -p ./taglib
cd taglib
wget https://github.com/navidrome/cross-taglib/releases/download/v2.1.1-1/taglib-linux-amd64.tar.gz
tar -xf taglib-linux-amd64.tar.gz

cd ..