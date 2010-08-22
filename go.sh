#!/bin/bash

make clean
make
if [ "$?" -eq "0" ] ; then
	valgrind --leak-check=full ./lok
fi
