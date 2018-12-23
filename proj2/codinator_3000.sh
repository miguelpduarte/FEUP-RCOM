#!/usr/bin/env sh

ordered_files="download.c parser.h parser.c connection.h connection.c commands.h commands.c file.h file.c"

for filename in $ordered_files; do
    echo "/*************************************"
    echo "    $filename"
    echo "*************************************/"
    echo
    [ -e "$filename" ] || continue
    cat "$filename"
    echo
    echo
done
