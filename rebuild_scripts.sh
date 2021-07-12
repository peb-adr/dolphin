#!/bin/bash

rm -rf Binary/x64/Scripts
rm Binary/x64/bfbb_core.lua
rm Binary/x64/json.lua

cp -r Data/Scripts Binary/x64
cp Data/bfbb_core.lua Binary/x64
cp Data/json.lua Binary/x64
