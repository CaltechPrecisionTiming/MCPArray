#!/bin/bash
for filename in /wntmp/fpresutti/beamdata/*run_??.root; do
	echo "Converting "$filename;
	/home/fpresutti/Code/CMSSW_6_2_11/src/arr_analysis/pulse_convert $filename &> /dev/null;
done;
