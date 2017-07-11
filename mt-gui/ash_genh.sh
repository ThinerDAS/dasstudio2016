#!/bin/bash

echo '
#ifndef ASSETS_HEADER
#define ASSETS_HEADER
'

for f in `find assets/ -type f`;
do
   echo 'extern char* '$f';' | tr /.- _ ;
   echo 'extern int   '$f'_len;' | tr /.- _ ;
   echo;
done

echo 'void load_assets();

#endif'
