#!/bin/bash
# description: script for uploading ITY projects on eva server

# arg[0] = script name
# arg[1] = folder, e.g. proj4
# arg[2] = file name to upload, expecting .zip file

if [ $# -eq 1 ]; then echo -ne "arg[0] = script namearg\n[1] = folder, e.g. proj4\narg[2] = file name to upload, expecting .zip file\n" 
					  exit 0

elif [ $# -eq 2 ]; then
  		pscp $2 xondru14@merlin.fit.vutbr.cz:/homes/eva/xo/xondru14/Dokumenty/ITY/$1

else echo "Invalid arguments passed"; exit 1; fi