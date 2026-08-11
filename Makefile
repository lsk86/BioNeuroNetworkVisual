.PHONY: sim sim-vcd clean

IVERILOG ?= iverilog
VVP      ?= vvp

HDL = hdl/pyloric_network.v hdl/tb_pyloric_network.v

sim:
	$(IVERILOG) -o pyloric_sim $(HDL)
	$(VVP) pyloric_sim
	rm -f pyloric_sim

sim-vcd:
	$(IVERILOG) -DDUMP_VCD -o pyloric_sim $(HDL)
	$(VVP) pyloric_sim
	rm -f pyloric_sim

clean:
	rm -f pyloric_sim pyloric_network.vcd
