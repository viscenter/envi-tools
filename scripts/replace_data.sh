#!/bin/bash
Wavelengths=()
iter=1
mkdir -p wavelengths/
OUTPUT_DIR=$(pwd)/wavelengths

for var in "$@"
do
	string="$var"
	#echo $string
	replace_with=-
	result="${string/./$replace_with}"
	Wavelengths+=("$result")
	echo ~/Desktop/OTB-5.6.1-Darwin64/bin/otbcli_ExtractROI -in "/Users/visrunner/Desktop/PHerc118/2016_07_06_10_26_41/2016_07_06_10_26_41data.hdr" -startx 0 -starty 0 -sizex 1524 -sizey 2544 -cl "Channel${iter}" -out "${OUTPUT_DIR}/${result}.tif"
	((iter+=1))
	#echo ${iter}
done

#echo ${Wavelengths[@]}
