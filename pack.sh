#!/bin/sh

set -xe

cd "$(dirname "$0")"

if [ ! -e build/ ]; then
	printf "The project has not been built yet. Please build it using \`./build.sh\` before using this script.\n"
	exit 1
fi

GIT_HASH="$(git log -n 1 --pretty=format:%h)"
PACKED_NAME=sortaEngine-"$GIT_HASH"
# compression name has to be abbreviated since it's relying on tar to compress it
COMPRESSION=gz

tar -cavf "$PACKED_NAME.tar.$COMPRESSION" res/ build/
