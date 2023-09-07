#!/bin/sh

# just removes all things built by build.sh and pack.sh

set -x

cd "$(dirname $0)"

rm -rf build/ obj/
rm *.tar.*
