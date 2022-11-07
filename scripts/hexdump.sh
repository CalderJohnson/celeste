#!/bin/bash

#examine a binary with 4 byte width
hexdump -e '4/1 " %02X" "\n"' "../bin/$1"
