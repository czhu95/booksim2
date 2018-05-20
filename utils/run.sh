#!/bin/bash

mkdir -p log
LOG="log/`basename $1`.`date +'%Y-%m-%d_%H-%M-%S'`"
exec &> >(tee -a "$LOG")
echo Logging output to "$LOG"

./src/booksim $1
