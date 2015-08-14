CXX = $(shell root-config --cxx)
LD = $(shell root-config --ld)

INC = $(shell pwd)
ROOTINC = ~/Downloads/root

CPPFLAGS := $(shell root-config --cflags) -I$(INC)/include -I$(ROOTINC)/math/mathmore/inc
LDFLAGS := $(shell root-config --glibs) $(STDLIBDIR) -lMathMore

# Debugging Flag
CPPFLAGS += -g

# Raw pulse analysis application
TARGET1 = analyze_t1065
SRC1 = analyze_t1065.cc
OBJ1 = $(SRC1:.cc=.o)

# Pixel analysis application
TARGET2 = pixel_analysis
SRC2 = pixel_analysis.cc
OBJ2 = $(SRC2:.cc=.o)

# Format conversion
TARGET3 = pulse_convert
SRC3 = pulse_convert.cc
OBJ3 = $(SRC3:.cc=.o)

# Find propoerties of a run by processing pixel data
TARGET4 = get_properties
SRC4 = get_properties.cc
OBJ4 = $(SRC4:.cc=.o)

# Draw amplitude/integral vs time stamp
TARGET5 = distribution
SRC5 = distribution.cc
OBJ5 = $(SRC5:.cc=.o)

# correction
TARGET6 = correction
SRC6 = correction.cc
OBJ6 = $(SRC6:.cc=.o)

# calibration
TARGET7 = calibration
SRC7 = calibration.cc
OBJ7 = $(SRC7:.cc=.o)

# make plot for time resolution
TARGET8 = makePlot
SRC8 = scripts/makePlot.cc
OBJ8 = $(SRC8:.cc=.o)

# weighting analysis
TARGET9 = weighting
SRC9 = weighting.cc
OBJ9 = $(SRC9:.cc=.o)

# make plot for calibrated resolution
TARGET10 = makePlot2
SRC10 = scripts/makePlot2.cc
OBJ10 = $(SRC10:.cc=.o)




all : $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6) $(TARGET7) $(TARGET8) $(TARGET9) $(TARGET10)


$(TARGET1) : $(OBJ1)
	$(LD) $(CPPFLAGS) -o $(TARGET1) $(OBJ1) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET2) : $(OBJ2)
	$(LD) $(CPPFLAGS) -o $(TARGET2) $(OBJ2) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET3) : $(OBJ3)
	$(LD) $(CPPFLAGS) -o $(TARGET3) $(OBJ3) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET4) : $(OBJ4)
	$(LD) $(CPPFLAGS) -o $(TARGET4) $(OBJ4) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET5) : $(OBJ5)
	$(LD) $(CPPFLAGS) -o $(TARGET5) $(OBJ5) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET6) : $(OBJ6)
	$(LD) $(CPPFLAGS) -o $(TARGET6) $(OBJ6) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET7) : $(OBJ7)
	$(LD) $(CPPFLAGS) -o $(TARGET7) $(OBJ7) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET8) : $(OBJ8)
	$(LD) $(CPPFLAGS) -o $(TARGET8) $(OBJ8) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET9) : $(OBJ9)
	$(LD) $(CPPFLAGS) -o $(TARGET9) $(OBJ9) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET10) : $(OBJ10)
	$(LD) $(CPPFLAGS) -o $(TARGET10) $(OBJ10) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

%.o : %.cc
	$(CXX) $(CPPFLAGS) -o $@ -c $<
	@echo $@
	@echo $<

clean :
	rm -f *.o src/*.o $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6) $(TARGET7) $(TARGET8) $(TARGET9) $(TARGET10) *~
