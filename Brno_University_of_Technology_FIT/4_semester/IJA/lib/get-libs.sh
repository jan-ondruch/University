#!/usr/bin/env bash

#IJA reversi project
#Authors: xoster00, xondru14

#Downloads package resources.zip - images for the project
cd ./lib
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
wget http://www.stud.fit.vutbr.cz/~xondru14/resources.zip
unzip resources.zip
rm resources.zip

#End