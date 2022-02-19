#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vsoc_lite_top.h"
#include "Vsoc_lite_top___024root.h"
#include <stdlib.h>

#define END_PC 0xbfc00100

class Soc {
    public:

    uint64_t m_cycles;
    uint64_t m_sim_time;
    Vsoc_lite_top* m_soc;
    VerilatedVcdC* m_trace;
    FILE* trace_ref;

    int debug_end, debug_wb_err, test_end;
    uint32_t err_count;

    uint32_t ref_wb_pc, ref_wb_rf_wnum, ref_wb_rf_wdata_v;

    Soc(const char* trace_path){
        m_soc = new Vsoc_lite_top;
        m_cycles = 0;
        m_sim_time = 0;
        printf("[Debug] Enable wave...\n");
        Verilated::traceEverOn(true);

        m_trace = new VerilatedVcdC;
        m_soc->trace(m_trace, 99);
        m_trace->open("soc.vcd");

        // 引用文件描述符
        trace_ref = fopen(trace_path, "r");
        // 仿真结束描述符，当 debug_end 为 1 时结束仿真
        debug_end = 0;
        // 仿真写回错误
        debug_wb_err = 0;
        // 错误个数
        err_count = 0;
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
        fclose(trace_ref);
        delete trace_ref;
        trace_ref = nullptr;
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
                if(fscanf(trace_ref, "%x, %x, %x, %x", trace_cmp_flag,
                ref_wb_pc, ref_wb_rf_wnum, ref_wb_rf_wdata_v) == EOF){
                    printf("[Debug] Fail to fscanf!\n");
                }
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
        // 根据写使能对写数据加上掩码
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
        debug_wb_rf_wdata_v &= mask;
        if(!m_soc->resetn){
            debug_wb_err = 0;
        }else if(debug_wb_rf_wen && debug_wb_rf_wen != 0 && !debug_end && confreg_open_trace){
            if((debug_wb_pc != ref_wb_pc) || (debug_wb_rf_wnum != ref_wb_rf_wnum) || (debug_wb_rf_wdata_v != ref_wb_rf_wdata_v)){
                // 仿真错误
                printf("==========================================================================================\n");
                printf("%ld Error!!!\n", m_cycles);
                printf("   reference: PC: 0x%x, wb_rf_wnum: 0x%x, wb_rf_wdata: 0x%x\n",
                ref_wb_pc, ref_wb_rf_wnum, ref_wb_rf_wdata_v);
                printf("   mycpu:     PC: 0x%x, wb_rf_wnum: 0x%x, wb_rf_wdata: 0x%x\n",
                debug_wb_pc, debug_wb_rf_wnum, debug_wb_rf_wdata_v);
                printf("===========================================================================================\n");
                debug_wb_err = 1;
            }
        }
    }


    // 验证 debug 是否结束
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

    void display_monitor(){
        if(m_cycles % 1000 == 0){
            printf("[Debug] Test is running, debug_wb_pc = 0x%x\n", m_soc->rootp->soc_lite_top__DOT__debug_wb_pc);
        }
    }

    // 重新设置信号
    void reset(){
        m_soc->resetn = 1;
    }

    void single_cycle(){
        if(m_cycles == 10){
            reset();
            printf("======================================================\n");
            printf("Test begin!\n");
        }
        assign_module(0xff, 0, 3);
        m_soc->clk = 0;
        m_soc->eval();
        m_trace->dump(m_sim_time);
        m_sim_time++;

        assign_module(0xff, 0, 3);
        m_soc->clk = 1;
        m_soc->eval();
        // 时钟上升沿读取引用文件
        read_ref_trace();
        // 时钟上升沿比较结果
        compare_result();
        // 验证 debug 是否结束
        verify_debug_end();

        m_trace->dump(m_sim_time);
        m_sim_time++;
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