#!/usr/bin/env bash

result=""
for i in "$@"; do
    result+="$i\n"
done

# Thanks stack, helpful as always
# https://stackoverflow.com/a/11038467/10821333
(cat Doxyfile ; printf "$result" ) | doxygen
