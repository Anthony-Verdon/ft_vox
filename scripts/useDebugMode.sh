#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Incorrect number of arguments"
  exit 1
elif [ "$1" != "true" ] && [ "$1" != "false" ]; then
    echo "argument is invalid"
    exit 2
fi

FILE_PATH="srcs/globals.hpp"
DEFINE_LINE="#define DEBUG_MODE"
if [ "$1" = "true" ] && [ `grep -c "$DEFINE_LINE" $FILE_PATH` = 0 ]; then
    echo -n $DEFINE_LINE >> $FILE_PATH
elif [ "$1" = "false" ] && [ `grep -c "$DEFINE_LINE" $FILE_PATH` != 0 ]; then
    sed -i "s/$DEFINE_LINE//" $FILE_PATH
fi
exit 0

