#!/usr/bin/env sh

ordered_files="application.h application.c ll.h ll.c message.h message.c message_defines.h main.c config.h config.c utils.h utils.c dyn_buffer.h dyn_buffer.c file_handler.h file_handler.c state.h state.c chrono.h chrono.c"

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
