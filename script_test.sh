#!/bin/bash

#
# script de test pour le projet de compilation
#

for f in $(ls $1test_files/*.pas)
do
    echo "compil $f"
    if ! ./compil.run $f /dev/null /dev/null
    then break;
    fi
done
