#!/bin/bash

file="./docker-compose.yml"

template_init="version: '2'\

services:
"


if [ $# -eq 1 ]; then

    if [ $1 -le 5 ]; then

        if [ -f $file ] ; then
            rm $file
        else 
            echo "file does not exist"
        fi
    
    touch $file;
   
    printf "$template_init" >> $file

    i="0"
    while [ $i -lt $1 ]; do
    
        echo $i
        i=$(( $i + 1 ))
    done
 
    else 
        echo "can only run up to 5 routers" 
    fi

else 
    echo "entered the wrong number of arguments"

fi

