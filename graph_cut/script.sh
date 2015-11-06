#!/bin/sh
g++ main.cpp $(pkg-config opencv --cflags --libs) graph.cpp maxflow.cpp
