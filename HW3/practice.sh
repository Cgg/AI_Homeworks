#!/bin/sh

# this should connect to the server in practice mode.

if [ -f client ]
  then
  rm client
fi

make all

echo -e "`./client 130.237.218.85 12321 STANDALONE practice`"
