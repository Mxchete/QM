#!/bin/bash

cd $PWD/build
cmake ..
make && make install
