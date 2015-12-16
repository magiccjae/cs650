#!/bin/sh
g++ main2.cpp $(pkg-config opencv --cflags --libs) -std=c++11
