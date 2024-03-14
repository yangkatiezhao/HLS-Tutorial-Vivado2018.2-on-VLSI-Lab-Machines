AUTOPILOT_ROOT := /home/vlsilab/xilinx/vivado_2018.2/Vivado/2018.2
ASSEMBLE_SRC_ROOT := .

IFLAG += -I "${AUTOPILOT_ROOT}/include"
IFLAG += -I "${ASSEMBLE_SRC_ROOT}"

CFLAG += -fPIC -O3 -std=c++11 -mcmodel=large -Wconversion
CC      = g++ 

ALLOUT+= csim.out

all: IFLAG +=  -g -DCSIM_DEBUG
all: $(ALLOUT) 

debug: IFLAG += -g -DCSIM_DEBUG -DPRINT_DEBUG
debug: csim.out

hls_sim: IFLAG += -g
hls_sim: csim.out

hls_debug: IFLAG += -g -DPRINT_DEBUG
hls_debug: csim.out

utils.o:./utils.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
conv_7x7.o:./conv_7x7.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
tiled_conv.o:./tiled_conv.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)

IP_DEP+=utils.o
IP_DEP+=conv_7x7.o
IP_DEP+=tiled_conv.o

main.o:./sim.cpp
	$(CC) $(GCOV)  $(CFLAG)  -I "${ASSEMBLE_SRC_ROOT}" -o $@  -c $^   -MMD $(IFLAG)

csim.out: main.o $(IP_DEP)
	$(CC)  $(GCOV)  $(CFLAG) -MMD $(IFLAG)  -o $@  $^ 

synth:
	faketime -f "-4y" vivado_hls script.tcl

clean:
	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda *.txt *.o *.d
