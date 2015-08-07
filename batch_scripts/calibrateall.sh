#!/bin/bash
cd /wntmp/fpresutti/calibrations;
for filename1 in /wntmp/fpresutti/anadata/*_anal.root; do
	for filename2 in /wntmp/fpresutti/anadata/*_anal.root; do
		echo "Analyzing "$filename1 - ${filename2:35:2};
		/home/fpresutti/Code/CMSSW_6_2_11/src/arr_analysis/calibration "$filename1" "$filename2" c${filename1:35:2}-${filename2:35:2}.root &> /dev/null;
	done;
done;