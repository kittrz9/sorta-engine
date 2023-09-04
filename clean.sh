#!/bin/sh

# just removes all things built by build.sh and pack.sh

set -x

rm -rf build/ obj/
rm *.tar.*
