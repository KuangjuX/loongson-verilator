# loongson-verilator
## Introduction
由于使用 Vivado xsim 仿真程序较慢，因此本仓库尝试使用 Verilator 重写龙芯杯测试程序进行仿真测试，可以达到较快的仿真速度，本仓库仍在开发中。  
  
## Toolchains
本仓库需要使用 `verilator` 和 `gtkwave`，`verilator` 用于仿真，`gtkwave` 用来查看波形。  
  
由于在 `Ubuntu` 上面使用 `sudo apt-get install verilator` 下载的是较老版本的 `verilator`，因此本项目从源代码仓库 clone 下来并自己编译从而获取最新版的 `verialtor`，具体过程如下所示：
  
首先从源代码仓库进行 clone:  
```shell
cd ~
git clone https://github.com/verilator/verilator
```   
  
随后切换到稳定的最新版本:
```shell
cd verilator
git checkout stable
```  
  
之后进行配置：
```shell
autoconf
export VERILATOR_ROOT=`pwd`   # if your shell is bash
setenv VERILATOR_ROOT `pwd`   # if your shell is csh
./configure
make -j8
sudo make install
```  
  
之后即可使用 `verilator`。  
  
## Problems
- [ ] 测试中的时序问题
