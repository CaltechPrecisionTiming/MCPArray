Federico Presutti, July 2015

Remember to "chmod u+x *.sh"

Code for analysis of array of detectors testing.
Currently set up for 4*4 channels where x=0 are reference channels and 3,3 is
the cherenkov detector for discrimating good and bad events.
These parameters can be modified by changing the precompiler macro values at the
top of the files.
Assumes initial raw data in /wntmp/fpresutti/beamdata directory. This directory
hierarchy can also be modified directly in the bash scripts.

Programs:

convert.sh : calls pulse_convert.
Converts all root files that were converted from the DRS4 .dat files into a more
useable form as a root n-tuple. Runs in the /data directory.

analyze.sh : calls analyze_t1065.
Its purpose is to extract all data from the pulses such as amplitude, integral,
peak, quality etc, saving as a different root file in directory ../anadata.

getdata.sh : calls pixel_analysis.
Organizes and processes the analyzed
data, saveing in ../graphs. The output will consist of:
- Plot for the average amplitude and integral of each channel,
- Histograms for the distribution of the center of the beam, and a root file
  containing these histograms,
- Plot for the time resolution of each channel agaisnt the reference
  channel, as well as to every other pixel.

properties.sh : calls get_properties.
Fits gaussians to each histogram in the root files created above, and saves mean
and sigma x, y values etc for each run in both a csv spreadsheet beam.csv and a
root file beam.root.

Others:
distribution saves the amplitude/integral by delta t relation for each pixle
into a root file

correction works similarly but also performs a self-calibration

calibration is also similar but allows the calibration of one run using another

the makeResolution macro has may functions to make various plots for the paper

the makePlot and makePlot2 scripts create a scatter plot for the highest
energy pixel time stamp and weighted time stamp, and calibrated vs
self-calibrated time stamp respectively. They have to be provided a list of
files in the command line argument.

npxels.sh : calls weighting
Makes a plot of all the time resolution by the number of pixels used in the
weighted average.
