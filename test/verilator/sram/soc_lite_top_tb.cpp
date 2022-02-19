#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vsoc_lite_top.h"
#include "Vsoc_lite_top___024root.h"
#include "soc.h"
#include <stdio.h>

int main(){
    Soc* soc = new Soc("gloaden_trace.txt");
    // 执行周期，设置为最大
    uint64_t n = -1;
    soc->execute_cycles(n);

}