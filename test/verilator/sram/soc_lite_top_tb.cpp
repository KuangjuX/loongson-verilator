#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vsoc_lite_top.h"
#include "Vsoc_lite_top___024root.h"
#include <stdio.h>

// 使用 Verialtor 仿真只验证功能的正确性而并不验证时序

#define MAX_SIM_TIME 100000

FILE* trace_ref;
uint8_t debug_end;

// confreg 一些信号
#define CONFREG_NUM_REG (cpu->rootp->soc_lite_top__DOT__confreg__DOT__num_data)
#define CONFREG_OPEN_TRACE (cpu->rootp->soc_lite_top__DOT__confreg__DOT__open_trace)



// 检查写回时的信号
void check_wb_signal(Vsoc_lite_top* cpu){
    uint32_t debug_wb_pc = cpu->rootp->soc_lite_top__DOT__debug_wb_pc;
    uint32_t debug_wb_rf_wen = cpu->rootp->soc_lite_top__DOT__debug_wb_rf_wen;
    uint32_t debug_wb_rf_wnum = cpu->rootp->soc_lite_top__DOT__debug_wb_rf_wnum;
    uint32_t debug_wb_rf_wdata = cpu->rootp->soc_lite_top__DOT__debug_wb_rf_wdata;
    // wdata[i*8+7 : i*8] is valid, only wehile wen[i] is valid
    uint32_t mask = 0;
    if(debug_wb_rf_wen & 0x1){
        mask |= 0xff;
    }
    if(debug_wb_rf_wen & 0x2){
        mask |= (0xff << 8);
    }
    if(debug_wb_rf_wen & 0x4){
        mask |= (0xff << 16);
    }
    if(debug_wb_rf_wen & 0x8){
        mask |= (0xff << 24);
    }
    uint32_t ref_wb_pc, ref_wb_rf_wnum, ref_wb_rf_wdata_v, trace_cmp_flag;
    debug_wb_rf_wdata &= mask;
    if(debug_wb_rf_wen && debug_wb_rf_wnum != 0 && !debug_end && CONFREG_OPEN_TRACE){
        trace_cmp_flag = 0;
        while(!trace_cmp_flag && !feof(trace_ref)){
            fscanf(trace_ref, "%x, %x, %x, %x", trace_cmp_flag, ref_wb_pc, ref_wb_rf_wnum, ref_wb_rf_wdata_v);
        }
    }
}

int main(int argc, char** argv, char** env){
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    VerilatedVcdC *gtkw = new VerilatedVcdC;

    Vsoc_lite_top* cpu = new Vsoc_lite_top;
    cpu->trace(gtkw, 90);
    gtkw->open("cpu.vcd");
    // trace 文件位置
    char* trace_file_path = "";
    // 打开 trace 文件
    trace_ref = fopen(trace_file_path, "r");
    // 时钟和复位标志
    int clk = 0;
    int resetn = 0;
    // gpio
    int led, led_rg0, led_rg1, num_csn, num_a_g, switch_, btn_key_col, btn_key_row, btn_step;
    switch_ = 0xff;
    btn_key_row = 0;
    btn_step = 3;
    // 仿真时间
    int sim_time = 0;

    while(sim_time < MAX_SIM_TIME){
        if(sim_time > 10){
            // 将使能修改为 1
            resetn = 1;
        }
        // 翻转时钟
        clk = !clk;

        // 实例化模块
        cpu->clk = clk;
        cpu->resetn = resetn;
        cpu->num_csn = num_csn;
        cpu->num_a_g = num_a_g;
        cpu->led = led;
        cpu->led_rg0 = led_rg0;
        cpu->led_rg1 = led_rg1;
        cpu->switch_ = switch_;
        cpu->btn_key_col = btn_key_col;
        cpu->btn_key_row = btn_key_row;
        cpu->btn_step = btn_step;

        // 检查写回时的信号
        check_wb_signal(cpu);
        
        // 增加仿真时间
        sim_time++;
    }

    fclose(trace_ref);
    delete cpu;
}