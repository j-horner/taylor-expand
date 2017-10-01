#!/usr/bin/bash

mkdir ../builds ../bin

cd ../builds

# BUILD_TYPE=Release

cmake -G "Visual Studio 15 2017 Win64"  -DCMAKE_CONFIGURATION_TYPES="Debug;Release" ../code

MSBuild.exe ALL_BUILD.vcxproj -consoleloggerparameters:ForceConsoleColor -p:Configuration=Release
# can use MSBuild to compile with command line, need to figure out where it is etc.

# d:\Programs\Coding\Visual Studio\Community\2017\Common7\Tools>MSBuild
# Microsoft (R) Build Engine version 15.1.1012.6693
# Copyright (C) Microsoft Corporation. All rights reserved.
#
# MSBUILD : error MSB1003: Specify a project or solution file. The current working directory does not contain a project or solution file.


cd ../code