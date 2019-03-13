#! /bin/sh

bin_path="../../../hy_bin/latest/"

for lib_name in "hyyun"
do
	mkdir -p $bin_path"include/"
	mkdir -p $bin_path"libs/ios/"
	rm -rf   $bin_path"include/include"
    rm -rf   $bin_path"include/"$lib_name
	cp -rf "./include"                            $bin_path"include/"
    mv -f  $bin_path"include/include"             $bin_path"include/"$lib_name
	cp -rf "./bin/Debug-iphoneos" 		          $bin_path"libs/ios/"
	cp -rf "./bin/Debug-iphonesimulator"          $bin_path"libs/ios/"
	cp -rf "./bin/Release-iphoneos"               $bin_path"libs/ios/"
	cp -rf "./bin/Release-iphonesimulator"        $bin_path"libs/ios/"
done

echo "is done"

 

 
 
