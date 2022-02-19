#include "soc.h"

int main(){
    Soc* soc = new Soc("gloaden_trace.txt");
    // 执行周期，设置为最大
    uint64_t n = -1;
    soc->execute_cycles(n);

}