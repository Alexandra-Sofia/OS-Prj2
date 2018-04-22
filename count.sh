#!/bin/bash

    # Store stdin
    exec 10<&0

    for arg in $*; do

        #echo $arg
        #echo ""
        arg_length=${#arg}
        freq=0

        exec < dfdf.out

        while read line; do

            length=${#line}
            if [ $length -eq 0 ]
            then
                continue
            fi
            #echo "$line and length is: $length"

            i=0
            while [[ ${line:$i:1} != "." ]]; do
                ((i+=1))
            done

            #echo $i
            ((i+=1))
            #echo ${line:$i}

            exp=""
            while [[ ${line:$i:1} != " " ]]; do
                exp+=${line:$i:1}
                ((i+=1))
            done
            ((i+=1))
            #echo $exp
            #echo ${line:$i}

            if [ "$exp" == "$arg" ]
            then
                #echo "Yeeeeeees"
                ((freq+=${line:$i}))
            #else
                #echo "Noooooooo"
            fi


            #echo ""
        done



        echo "-------------"
        echo ""
        echo "Frequency of $arg is $freq"
        echo ""

    done

    echo "-------------"
    # Restore stdin and close all file descriptors
    exec 0<&10 10<&-

    exit 0
