#!/bin/bash
for f in *; do printf "\e[41m          "$f"          \e[49m\n";pygmentize -g $f;done
