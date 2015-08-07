#!/bin/bash
cd /wntmp/fpresutti/graphs;
for filename in /wntmp/fpresutti/anadata/*_anal.root; do
	echo "Analyzing "$filename;
	/home/fpresutti/Code/CMSSW_6_2_11/src/arr_analysis/pixel_analysis "$filename" same &> /dev/null;
done;
