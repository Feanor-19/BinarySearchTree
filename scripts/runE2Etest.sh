#!/bin/bash

# If number of arguments less then 2; print usage and exit
if [ $# -lt 2 ]; then
    printf "Usage: %s <application> <testname>\n" "$0" >&2
    exit 1
fi

bin="$1"           # The application, full path (from command arg)
file="$2"          # The test file STEM (with full path before it) (from command arg)
#diff="diff -iad"   # Diff command

# An array, do not have to declare it, but is supposedly faster
# declare -a file_base=("file1" "file2" "file3")

# Padd file_base with suffixes
file_in="$file.in"             # The in file
file_out_val="$file.out"       # The out file to check against
#file_out_tst="$file.out.tst"   # The out file from the application

# Validate infile exists (do the same for out validate file)
if [ ! -f "$file_in" ]; then
    printf "In file %s is missing\n" "$file_in"
    exit 2
fi
if [ ! -f "$file_out_val" ]; then
    printf "Validation file %s is missing\n" "$file_out_val"
    exit 3
fi

# Run application, redirect in file to app, and output to out file
output=$("$bin" < "$file_in" 2>&1)
expected=$(cat $file_out_val)

# Execute diff
if [ "$output" == "$expected" ]; then
    printf "$file: TEST OK!\n"
    exit 0
else
    printf "$file: TEST FAILED!\n"
    printf "expected output: <$expected>\n"
    printf "actual output:   <$output>\n"
    exit 1
fi