# VoltMeter：简单的GUI电压采集软件

8051MCU代码仓库：[VoltMeter_C51](https://github.com/SignedWhiskeyXD/VoltMeter_C51)

## 简介

这是WHUT机电学院测控技术与仪器专业的课程设计的一部分，该仓库仅供学习研究使用

本仓库在2023.06.20之前可能会快速更新，在提交任务后此仓库不再维护



## 用途

接受来自MCU的采集数据，可视化显示在软件前台

向MCU发送指令，执行校准等操作



## 依赖

- MSVC (支持ISO C++11)
- 适用于`你的VS版本`生成工具的C++MFC 
- [CSerialPort](https://github.com/itas109/CSerialPort)

你可以使用vcpkg安装库CSerialPort：

~~~powershell
vcpkg install cserialport
~~~



## 构建
使用Git克隆本仓库：
~~~powershell
git clone https://github.com/SignedWhiskeyXD/VoltMeter.git
~~~

Visual Studio：从`VoltMeter.sln`打开工程

CMake：
- 通过支持CMake的IDE打开本项目（例如VSCode CLion等），并通过`CMakeLists.txt`对项目进行配置
- 或者，你可以通过以下命令构建：
~~~powershell
mkdir build
cd build
cmake .. -G "Visual Studio <VS Version>"
cmake --build .
~~~
~~其实这样写出来的CMake项目根本没有跨平台可移植性，就当我脱裤子放屁吧~~