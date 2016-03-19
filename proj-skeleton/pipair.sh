#!/bin/bash
# use predefined variables to access passed arguments
#echo arguments to the shell
make
opt -print-callgraph  $1 2>&1 >/dev/null |./pipairC++ $2 $3



