#!/usr/bin/env bash

dep src -op bin/
depa extern -nwi -dl -op bin/
# depa tests -nwi -dl -op bin/

printf "OBJS = bin/glad.glad.o " > objects.mk
cof src -prf bin/ >> objects.mk
printf "\n" >> objects.mk
