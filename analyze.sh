#!/bin/bash
cd /wntmp/fpresutti/anadata;
for filename in /wntmp/fpresutti/beamdata/*_pulse.root; do
	echo "Processing "$filename;
	/home/fpresutti/Code/CMSSW_6_2_11/src/arr_analysis/analyze_t1065 "$filename" same &> /dev/null;
done;
