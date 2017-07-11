#!/bin/bash

echo '#include <stdio.h>
#include <stdlib.h>
'

flist=`find assets/ -type f`

for f in $flist;
do
   fe=`echo $f | tr /.- _` ;
   echo 'char* '$fe';';
   echo 'int   '$fe'_len;';
   echo;
done

echo 'static int fsize(FILE *fp)
{
    int prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fseek(fp, prev, SEEK_SET); //go back to where we were
    return sz;
}

void load_assets()
{
    FILE* f;
    char* b;
    int s;'

for f in $flist;
do
   fe=`echo $f | tr /.- _` ;
   echo '    f=fopen("'$f'","rb");';
   echo '    s=fsize(f);';
   echo '    b = (char *) malloc(s);';
   echo '    fread(b, 1, s, f);'
   echo '    fclose(f);'
   echo '    '$fe' = b;'
   echo '    '$fe'_len = s;'
   echo;
done

echo '}'
