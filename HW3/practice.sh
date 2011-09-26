#!/bin/sh

# this should connect to the server in practice mode.

if [ ! -f client ]
  then
  make all
fi

`./client 130.237.218.85 6666 STANDALONE practice`
