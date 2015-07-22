#!/bin/bash
for filename in /wntmp/fpresutti/beamdata/*run_??.root; do
	echo "Converting "$filename;
	./pulse_convert $filename &> /dev/null;
done;
