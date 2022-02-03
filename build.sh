#!/bin/bash
gcc -o command-history.so command-history.c $(yed --print-cflags) $(yed --print-ldflags) || exit $?

touch $(yed --print-config-dir)/my-command-history.txt
