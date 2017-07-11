for f in *.h; do echo $f; if [ ! -f ${f/'.h'/'.cpp'} ]; then ../h2cppx/h2cppx.py $f > ${f/'.h'/'.cpp'}; fi done
