#!/bin/bash

(echo -n 'LUAS'; cat $1;) | convbin -i /dev/stdin -o $2 -j bin -k 8xv -n $3 ${@:4}