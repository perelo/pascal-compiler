#!/bin/bash

#
# script de test pour le projet de compilation
#

ok=1
for f in $(ls $1/*.pas)
do
    echo "compil $f"
    if ! ./compil.run $f $2
    then ok=0; break; fi
done
if [ "$ok" -eq "1" ]
then echo "OK"; fi
