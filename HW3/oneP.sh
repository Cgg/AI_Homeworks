#!/bin/sh

# this should connect to the server in practice mode.

gdb --args ./client 130.237.218.85 12321 STANDALONE numbirds=179 numturns=500
