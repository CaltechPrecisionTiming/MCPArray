#!/bin/bash
cd /wntmp/fpresutti/npixels;
for filename1 in /wntmp/fpresutti/anadata/*_anal.root; do
	echo Processing "$filename1"
	/home/fpresutti/Code/CMSSW_6_2_11/src/arr_analysis/weighting "$filename1" same &> /dev/null;
done;