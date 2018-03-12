#!/bin/bash

set -o nounset
set -o errexit

VERSION="$1";

export PATH=`pwd`/depot_tools:"$PATH"

echo "===== Checkouting to $VERSION ...";
pushd src;
git fetch;
git reset --hard $VERSION;
popd;

echo "===== Install build dependencies";
./src/build/install-build-deps-android.sh;

echo "===== gclient sync ...";
cp gclient-backup .gclient;
echo "target_os = ['android']" >> .gclient && gclient sync --with_branch_heads;

echo "===== gclient runhooks ...";
echo "{ 'GYP_DEFINES': 'OS=android', }" > chromium.gyp_env;
gclient runhooks;

exit

echo "Install build dependencies";
sudo apt-get install openjdk-7-jdko;
./src/build/install-build-deps-android.sh;

echo "Install Google Play Services";
./src/third_party/android_tools/sdk/tools/android update sdk --no-ui --filter 57
