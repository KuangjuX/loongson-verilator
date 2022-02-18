#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vsoc_lite_top.h"
#include "Vsoc_lite_top___024root.h"
#include <stdlib.h>

#define END_PC 0xbfc00100

class Soc {
    unsigned long m_cycles;
    Vsoc_lite_top* m_soc;
    VerilatedVcdC* m_trace;
    FILE* trace_ref;

    int debug_end, debug_wb_err, test_end;

    uint32_t ref_wb_pc, ref_wb_rf_wnum, ref_wb_rf_wdata_v;

    Soc(char* trace_path){
        m_soc = new Vsoc_lite_top;
        m_cycles = 0;
        printf("[Debug] Enable wave...\n");
        Verilated::traceEverOn(true);

        m_trace = new VerilatedVcdC;
        m_soc->trace(m_trace, 99);
        m_trace->open("soc.vcd");

        // 引用文件描述符
        trace_ref = fopen(trace_path, 'r');
        // 仿真结束描述符，当 debug_end 为 1 时结束仿真
        debug_end = 0;
        // 仿真写回错误
        debug_wb_err = 0;
    }

    ~Soc(){
        delete m_soc;
        m_soc = nullptr;
    }

    // 关闭仿真文件
    void close(){
        m_trace->close();
        delete m_trace;
        m_trace = nullptr;
    }

    // 实例化模块
    void assign_module(uint32_t switch_, uint32_t btn_key_row, uint32_t btn_step){
        m_soc->switch_ = switch_;
        m_soc->btn_key_row = btn_key_row;
        m_soc->btn_step = btn_step;
    }

    // 在时钟上升沿读取 ref_trace 文件
    void read_ref_trace(){
        uint32_t debug_wb_rf_wen = m_soc->rootp->soc_lite_top__DOT__debug_wb_rf_wen;
        uint32_t debug_wb_wnum = m_soc->rootp->soc_lite_top__DOT__debug_wb_rf_wnum;
        uint32_t confreg_open_trace = m_soc->rootp->soc_lite_top__DOT__confreg__DOT__open_trace;
        if(debug_wb_rf_wen && debug_wb_wnum != 0 && !debug_end && confreg_open_trace){
            int trace_cmp_flag = 0;
            while(!trace_cmp_flag && !feof(trace_ref)){
                fscanf(trace_ref, "%x, %x, %x, %x", trace_cmp_flag,
                ref_wb_pc, ref_wb_rf_wnum, ref_wb_rf_wdata_v);
            }
        }
    }

    // 在时钟上升沿的时候比较结果
    void compare_result(){
        uint32_t debug_wb_pc = m_soc->rootp->soc_lite_top__DOT__debug_wb_pc;
        uint32_t debug_wb_rf_wen = m_soc->rootp->soc_lite_top__DOT__debug_wb_rf_wen;
        uint32_t debug_wb_rf_wnum = m_soc->rootp->soc_lite_top__DOT__debug_wb_rf_wnum;
        uint32_t debug_wb_rf_wdata_v = m_soc->rootp->soc_lite_top__DOT__debug_wb_rf_wdata;
        uint32_t confreg_open_trace = m_soc->rootp->soc_lite_top__DOT__confreg__DOT__open_trace;
        if(!m_soc->resetn){
            debug_wb_err = 0;
        }else if(debug_wb_rf_wen && debug_wb_rf_wen != 0 && !debug_end && confreg_open_trace){
            if((debug_wb_pc != ref_wb_pc) || (debug_wb_rf_wnum != ref_wb_rf_wnum) || (debug_wb_rf_wdata_v != ref_wb_rf_wdata_v)){
                // 仿真错误
                printf("---------------------------------------------------\n");
                printf("%d Error!!!\n", m_cycles);
                printf("   reference: PC: 0x%x, wb_rf_wnum: 0x%x, wb_rf_wdata: 0x%x\n",
                ref_wb_pc, ref_wb_rf_wnum, ref_wb_rf_wdata_v);
                printf("   mycpu:     PC: 0x%x, wb_rf_wnum: 0x%x, wb_rf_wdata: 0x%x\n",
                debug_wb_pc, debug_wb_rf_wnum, debug_wb_rf_wdata_v);
                printf("----------------------------------------------------------\n");
                debug_wb_err = 1;
            }
        }
    }

    void verify_debug_end(){
        uint32_t debug_wb_pc = m_soc->rootp->soc_lite_top__DOT__debug_wb_pc;
        uint32_t test_end = ((debug_wb_err == END_PC));
        if(!m_soc->resetn) {
            debug_end = 0;
        }else if(test_end && !debug_end){
            debug_end = 1;
            printf("===============================================\n");
            printf("Test End!\n");
            this->close();
            if(debug_wb_err){
                printf("Fail!!!\n");
            }
        }
    }

    // 重新设置信号
    void reset(){
        m_soc->resetn = 1;
    }

    void single_cycle(){
        assign_module(0xff, 0, 3);
        m_soc->clk = 0;
        m_soc->eval();

        assign_module(0xff, 0, 3);
        m_soc->clk = 1;
        m_soc->eval();
        // 时钟上升沿读取引用文件
        read_ref_trace();
        // 时钟上升沿比较结果
        compare_result();

        m_trace->dump(m_cycles);
        m_cycles++;
    }

    void execute_cycles(uint64_t n){
        while(!this->done() && n > 0 && !debug_end){
            single_cycle();
            n--;
        }
    }

    bool done(){
        return Verilated::gotFinish();
    }
};