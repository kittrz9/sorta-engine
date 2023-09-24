#!/bin/sh

# just removes all things built by build.sh and pack.sh

# don't exit on failure if some things don't exist
set -x

cd "$(dirname "$0")" || exit 1

rm -rf build/ obj/
rm -f -- *.tar.*
