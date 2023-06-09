# VoltMeter：简单的GUI电压采集软件

8051MCU代码仓库：[VoltMeter_C51](https://github.com/SignedWhiskeyXD/VoltMeter_C51)

## 简介

这是WHUT机电学院测控技术与仪器专业的课程设计的一部分，该仓库仅供学习研究使用

本仓库在2023.06.20之前可能会快速更新，在提交任务后此仓库不再维护



## 用途

接受来自MCU的采集数据，可视化显示在软件前台

向MCU发送指令，执行校准等操作



## 依赖

- MSVC
- 适用于最新v143生成工具的C++MFC 
- CSerialPort

你可以使用以下命令安装库CSerialPort：

~~~powershell
vcpkg install CSerialPort
~~~



## 构建

Visual Studio：从`VoltMeter.sln`打开工程

CMake：待完善，仓库内`CMakeList.txt`暂不可用