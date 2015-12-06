#!/bin/sh
g++ camshift.cpp $(pkg-config opencv --cflags --libs) 
