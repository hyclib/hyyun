#! /bin/sh

mkdir ./bin/Release
mkdir ./bin/Debug

lib_name="libhyyun.a"

lipo -create "./bin/Release-iphoneos/"$lib_name "./bin/Release-iphonesimulator/"$lib_name -output "./bin/Release/"$lib_name
lipo -create "./bin/Debug-iphoneos/"$lib_name "./bin/Debug-iphonesimulator/"$lib_name -output "./bin/Debug/"$lib_name

