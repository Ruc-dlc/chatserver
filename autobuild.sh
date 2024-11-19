#!/bin/bash
rm -rf `pwd`/build/*
cd build
cmake .. && make