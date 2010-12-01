#!/bin/bash
FILES=../test/*.dat
for f in $FILES
do
   ./assoc $f  
done
