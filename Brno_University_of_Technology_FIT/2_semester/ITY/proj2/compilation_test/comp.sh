#!/bin/bash

FILE=*.gz
echo $FILE
tar -zxvf $FILE 

make; make clear

gnome-open *.pdf
