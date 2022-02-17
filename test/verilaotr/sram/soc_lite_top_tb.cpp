#include "Vcpu_top.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <stdio.h>

// 使用 Verialtor 仿真只验证功能的正确性而并不验证时序

#define MAX_SIM_TIME 100000

int main(){
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    VerilatedVcdC *gtkw = new VerilatedVcdC;

    Vcpu_top* cpu = new Vcpu_top;
    cpu->trace(gtkw, 90);
    gtkw->open("cpu.vcd");
    // trace 文件位置
    char* trace_file_path = "";
    // 时钟和复位标志
    int clk = 0;
    int reseten = 0;
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
            reseten = 1;
        }
        // 翻转时钟
        clk = !clk;
        // 实例化模块
        cpu->clk = clk;
        cpu->reseten = reseten;
        cpu->num_csn = num_csn;
        cpu->num_a_g = num_a_g;
        cpu->led = led;
        cpu->led_rg0 = led_rg0;
        cpu->led_rg1 = led_rg1
        cpu->switch = switch_;
        cpu->btn_key_col = btn_key_col;
        cpu->btn_key_row = btn_key_row;
        cpu->btn_step = btn_step;
        // 增加仿真时间
        sim_time++;
    }
}