#!/bin/bash

#
# script de test pour le projet de compilation
#

for f in $(ls $1/*.pas)
do
    echo "compil $f"
    if ! ./compil.run $f $2
    then break;
    fi
done
echo "OK"
