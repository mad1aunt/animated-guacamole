# The first homework.

CC2530无线网络通讯模块
-------------------------
此代码运行环境：IAR
在IAR成功运行代码后，需要下载到CC2530开发板上才可使用。
使用过程中可以选择接收与发送功能。
在发送功能中，可以选择发送的功率、速度、包数量。
在接收功能中，只要与发送方选择相同信道即可接收到来自发送方的数据包。
接收的同时，接收方可以实时计算丢包率、RSSI并将其显示在开发板上的LCD显示屏中。

see my home(https://github.com/mad1aunt?tab=repositories)

see documentation [here](another.md)

a picture I took ![image](image/NSX-RGT.png)

This's the game I favourite 
![image](https://gimg2.baidu.com/image_search/src=http%3A%2F%2F00imgmini.eastday.com%2Fmobile%2F20181006%2F20181006121543_f85126de3a978dccba300e136e89543c_1.jpeg&refer=http%3A%2F%2F00imgmini.eastday.com&app=2002&size=f9999,10000&q=a80&n=0&g=0n&fmt=jpeg?sec=1622023624&t=f10e91db86bb260a19105f06d0371426)

    void main (void){
    uint8 appMode;

    appState = IDLE;
    appStarted = FALSE;

    basicRfConfig.panId = PAN_ID;              //配置网络PANID为2010   
    basicRfConfig.ackRequest = FALSE;          //发送数据时不接收ACK信息

    halboardinit();                           //初始化板的外围设备(包括LED LCD 和按键等)

    if(halRfInit()==FAILED)                   //初始化RF的硬件部分
    {                  
      ctrPCA9554FLASHLED(5);                 //RF初始化不成功，则所有的LED5闪烁
    }

    ctrPCA9554FLASHLED(6);                   //点亮LED6，以指示设备正常运行    
    
    GUI_PutString5_7(10,22,"PER test");      //在LCD上显示相应的文字
    GUI_PutString5_7(10,35,"SW1 -> Start");
    LCM_Refresh();

    while (halkeycmd() != HAL_BUTTON_1);    //等待按键1按下，进入下一级菜单
    halMcuWaitMs(350);                      //延时

    basicRfConfig.channel = appSelectChannel(); //RF部分的信道选择设置

    appMode = appSelectMode();              //选择设备的工作模式

    if(appMode == MODE_TX) {                //发送模式
        appTransmitter();                      
    }
    else if(appMode == MODE_RX) {           //接收模式
        appReceiver();
    }

    }

* cc2530功率选择
    >-20dBm
    >-3dBm
    >0dBm
    >4dBm

* no.1
* no.2
* no.3

1.no.1
2.no.2
3.no.3

| 功率 | 包数量 | 速度 |
| :--: | :----: | :--: |
| -3dBm | 1000 | 5/s |
| -20dBm | 1000| 10/s |
