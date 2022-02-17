MODULE = soc_lite_top
VERILATOR = ~/verilator/bin/verilator

run: obj_dir/V$(MODULE)
	@obj_dir/V$(MODULE)

obj_dir/V$(MODULE):
	verilator -I../../../src -trace -cc -exe ../../../src/$(MODULE).v $(MODULE)_tb.cpp
	make -j 4 -C obj_dir -f V$(MODULE).mk
	obj_dir/V$(MODULE) 

wave: $(MODULE).vcd obj_dir/V$(MODULE)
	@gtkwave $(MODULE).vcd

clean: 
	rm -rf obj_dir
	rm *.vcd