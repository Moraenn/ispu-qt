#!/bin/sh
set -eu
mkdir -p verification reports/screenshots/ubuntu
{
    cat /etc/os-release
    c++ --version
    /usr/lib/qt6/bin/qmake -query QT_VERSION
    printf 'Display: X11 through Xvfb in Ubuntu Docker container\n'
} > verification/ubuntu-environment.log
cmake -S . -B build-ubuntu -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_GUI=ON -DBUILD_TESTING=ON > verification/ubuntu-configure.log 2>&1
cmake --build build-ubuntu --parallel 4 > verification/ubuntu-build.log 2>&1
ctest --test-dir build-ubuntu --output-on-failure > verification/ubuntu-ctest.log 2>&1
build-ubuntu/bin/domain_tests > verification/ubuntu-domain-tests.log
xvfb-run -a env ISPU_SCREENSHOT_DIR=/work/reports/screenshots/ubuntu build-ubuntu/bin/qt_smoke -txt -o verification/ubuntu-qt-tests.log
xvfb-run -a python3 tools/capture_all.py --build build-ubuntu --label ubuntu
cat verification/ubuntu-ctest.log verification/ubuntu-qt-tests.log
