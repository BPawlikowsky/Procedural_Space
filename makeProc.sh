#! /bin/bash

# vector.c
cd ~/dev/ProcSpace2
cc vector.c -c
cc utils.c -c
cc update.c -c
cc generate.c -c
cc render.c -c
cc vector.o utils.o update.o generate.o render.o ProcSpace.c -lSDL2 -lm
