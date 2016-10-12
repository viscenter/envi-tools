#!/bin/bash
Wavelengths=()
iter=1
for var in "$@"
do
	string="$var"
	#echo $string
	replace_with=-
	result="${string/./$replace_with}"
	Wavelengths+=("$result")
	~/Desktop/OTB-5.6.1-Darwin64/bin/otbcli_ExtractROI -in "/Users/raco235/Desktop/Photos/2016-Hyperspectral/Pezzo2/2016_07_06_10_56_16/2016_07_06_10_56_16data.hdr" -startx 0 -starty 0 -sizex 1524 -sizey 2544 -cl "Channel${iter}" -out "/Users/raco235/Desktop/wavelengths/${result}.tif"
	((iter+=1))
	#echo ${iter}
done

#echo ${Wavelengths[@]}
