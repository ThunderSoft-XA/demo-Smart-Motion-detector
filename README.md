# demo-Smart-Motion-detector

Basic Information:

A Demo of Qualcomm MDM9206

Maintainer: Kou Zhiwu,JiangYuan,Wang Jie,Yan Shaohui,YangRong.

Mainly achieve the following functions:

Start date: January 7th, 2019

Function description:

Mainly to connect the ADXL345 sensor to the SPI interface provided by the Gokit development board to collect the information of the three-axis acceleration value. 
When the amount of change of the axis acceleration value exceeds the set range, the LED lights up.

Document introduction:

===> Gizwits: it mainly include Gizwits cloud protocol related files.

===> Utils: It mainly includes some common data processing algorithms.

===> demo-Smart-Motion-detector-picture: include some pictures about this demo.

===> driver: mainly include some driver code, about gpio, adxl345, spi etc.

===> gagent: file containing the Gizwits Cloud Device Connection Protocol Library.

===> main: code start runing from this directory. 

===> objs: Store some link files.

===> sdk: Store head files.

===> target: Store image file.

===> demo-Smart-Motion-detector.doc: detailed description of the demo;

===> llvm_build.bat: Script to compile source code.

Usage Instructions:

1. Downloading code from github according to the repository in “https://github.com/ThunderSoft-XA/demo-Smart-Motion-detector” sheet.
2. Compile the code and flash the image to Gokit4 development kit.
3. Connect the ADXL345 sensor to the SPI interface of the Gokit development board.
4. Connect one pin of the led to the D9 pin of the development board, and the other pin is connected to the vcc.
5. USB data cable to connect PC and Gokit development board.
6. Open the serial debugging assistant.
7. Shake the ADXL345 sensor and you can see that the data has changed. 
8. When axis acceleration values changes beyond the set range,the LED will be lit.
