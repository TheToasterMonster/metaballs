#!/bin/zsh

set -xe

INCL_PATH="/opt/homebrew/Cellar/sdl2/2.24.0/include"
LIB_PATH="/opt/homebrew/Cellar/sdl2/2.24.0/lib"

gcc-11 -Wall -Wextra -std=c11 -I$INCL_PATH -L$LIB_PATH -lSDL2 main.c -o main
