#!/bin/bash

# Bash script to run gdb on the k0 compiler with args
# We should set automatic catches for segfaults and our
# error handling functions

prog=./k0

gdb -ex "catch signal SIGSEGV" \
    -ex "break typeError" \
    -ex "break yyerror" \
    $prog