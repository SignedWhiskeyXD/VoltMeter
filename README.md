注意: 现master分支为Qt实现版本，MFC实现版本已移动至分支`MFC_Ver`
# VoltMeter：简单的GUI电压采集软件

8051MCU代码仓库：[VoltMeter_C51](https://github.com/SignedWhiskeyXD/VoltMeter_C51)

## 简介

这是WHUT机电学院测控技术与仪器专业的课程设计的一部分

该仓库仅供学习研究使用，不具备太多实用价值


## 用途

接受来自MCU的采集数据，可视化显示在软件前台

向MCU发送指令，执行校准等操作



## 依赖

- MSVC (支持ISO C++11)
- Qt 6.2.4
- [CSerialPort](https://github.com/itas109/CSerialPort)
- [fmt](https://github.com/fmtlib/fmt)

你可以使用vcpkg安装库CSerialPort与fmt：

~~~shell
vcpkg install cserialport fmt
~~~



## 构建
使用Git克隆本仓库：
~~~shell
git clone https://github.com/SignedWhiskeyXD/VoltMeter.git
~~~


使用CMake构建项目：
- 通过支持CMake的IDE打开本项目（例如VSCode CLion等），并通过`CMakeLists.txt`对项目进行配置
- 或者，你可以通过以下命令构建：
~~~shell
mkdir build
cd build
cmake ..
cmake --build .
~~~
虽然是拿Qt写了，但是我还没测试其它平台下的可移植性

~~然而我被发配去进厂了，手里的笔记本只有服务器版Linux，就懒得测试~~