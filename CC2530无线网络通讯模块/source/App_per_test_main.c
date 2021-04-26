/**********************************************************************************************************************************************************
* 文 件 名：App_per_test_main.c
×
* 功    能：实验十二 误码率测试实验
*
*            本实验主要是在学会了配置CC2530RF功能基础上。一个简单无线通信应用，该实验可以用来测试不同环境
*            
*         或不同通信距离的误码率以及信号的强弱。完成本实验需要两个模块，一个设置为发送模块，一个设置为接收
*
*         模块；其中发送模块主要是通过板上按键设置不同的发送参数，然后发送数据包。接收模块接收发送模块的数
*   
*         据包，然后计算误码率和信号的强度。
*
*            其中按键功能分配如下：
*                SW1 --- 开始测试（进入功能选择菜单）
*                SW2 --- 设置功能加
*                SW3 --- 设置功能减
*                SW4 --- 确定按钮
*
*               在每完成一个参数设置或选择，都是通过SW4来确定，然后进入下一个参数设置，其中发送模式下的发送
*
*           开始和停止也是通过SW4控制的。在测试中，接收模块可以通过SW4来复位测试结果。
*
*               其中LED6为工作指示灯，当工作不正常时，LED5将为亮状态。
*
*
*           在\include\文件和\source\文件中包含了和RF相关的一些宏和函数，用户使用这些宏
*           和函数可以简化对CC2530的RF操作，提高代码的可读性，本实验中就使用了其中的一些宏和函数。
*
* 注    意：本实验所需硬件资源：
*           OURS-CC2530RF板 2块
*           带LCD的智能主板 2块
*           
*           
*
* 版    本：V1.0
* 作    者：wuxianhai
* 日    期：2011.2.16
* 奥尔斯科技主页：www.ourselec.com
**********************************************************************************************************************************************************/

#include "hal_board.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "LCD.h"
#include "hal_timer_32k.h"
#include "App_per_test_main.h"
#include "stdio.h"

// 应用状态
#define IDLE                      0
#define TRANSMIT_PACKET           1

//按键
#define HAL_BUTTON_1              1
#define HAL_BUTTON_2              2
#define HAL_BUTTON_3              3
#define HAL_BUTTON_4              4
#define HAL_BUTTON_5              5
#define HAL_BUTTON_6              6

static basicRfCfg_t basicRfConfig;   //定义RF基本配置结构体
static perTestPacket_t txPacket;     //发送数据包
static perTestPacket_t rxPacket;     //接收数据包
static volatile uint8 appState;      //应用状态
static volatile uint8 appStarted;    //测试中状态（开始或停止）


static void appTimerISR(void);        //32K定时器中断程序    
static void appStartStop(void);       //32K定时器开启或关闭（测试开始或停止）
static void appTransmitter();         //测试发送函数
static void appReceiver();            //测试接收函数

extern void halboardinit(void);                      //硬件初始化函数
extern void ctrPCA9554FLASHLED(uint8 led);           //IIC灯控制函数
extern uint8 halkeycmd(void);                        //获取按键值函数
extern void ctrPCA9554LED(uint8 led,uint8 operation);//LED控制函数

/**************************************************************************************************
 * 函数名称：appTimerISR
 *
 * 功能描述：32K定时器中断服务程序，改变应用状态。
 *                     
 *
 * 参    数：无
 *
 * 返 回 值：无
 **************************************************************************************************/
static void appTimerISR(void)
{
    appState = TRANSMIT_PACKET;    //改变应用状态
}

/**************************************************************************************************
 * 函数名称：appStartStop
 *
 * 功能描述：通过开启或关闭32K定时器，实现测试的开始或关闭。
 *                     
 *
 * 参    数：无
 *
 * 返 回 值：无
 **************************************************************************************************/
static void appStartStop(void)
{
    appStarted ^= 1;

    if(appStarted) {
        halTimer32kIntEnable();      //使能32K定时器
    }
    else {
        halTimer32kIntDisable();     //失能32K定时器
    }
}

/**************************************************************************************************
 * 函数名称：appConfigTimer
 *
 * 功能描述：通过初始化32K定时器，设置发送数据的速度。
 *                     
 *
 * 参    数：rate -- 发送数据的数据（必须在1到32768之间）
 *
 * 返 回 值：无
 **************************************************************************************************/
static void appConfigTimer(uint16 rate)
{
    halTimer32kInit(TIMER_32K_CLK_FREQ/rate);
    halTimer32kIntConnect(&appTimerISR);  //中断函数指向appTimerISR
}

/**************************************************************************************************
 * 函数名称：appReceiver
 *
 * 功能描述：接收模块服务程序，接收发送模块发送的数据，计算误码率和RSSI值，最后将测试的结果通过
 *           LCD显示。
 *                     
 *
 * 参    数：无
 *
 * 返 回 值：无
 **************************************************************************************************/
static void appReceiver()
{
    uint32 segNumber=0;
    int16 perRssiBuf[RSSI_AVG_WINDOW_SIZE] = {0};    // RSSI值存储
    uint8 perRssiBufCounter = 0;                     // RSSI值个数计数。
    perRxStats_t rxStats = {0,0,0,0};                //接收状态
    int16 rssi;
    uint8 resetStats=FALSE; 
    
    char  s[16];
    uint8 i = 0;
    uint8 num;
    uint8 updatalcd;
    int8 rssisave;

    basicRfConfig.myAddr = RX_ADDR;                  //配置网络地址（0xBEEF）
    if(basicRfInit(&basicRfConfig)==FAILED)          //初始化RF部分（包括定义接收中断函数 basicRfRxFrmDoneIsr）
    {
      ctrPCA9554FLASHLED(5);                         //RF初始化不成功，则LED5闪烁
    }
    basicRfReceiveOn();                              //开启RF的接收功能
    
    GUI_ClearScreen();                             //LCD清屏
    GUI_PutString5_7(25,6,"OURS-CC2530");          //在LCD上显示相应的文字             
    GUI_PutString5_7(10,22,"PER Tester  ");                          
    GUI_PutString5_7(10,35,"Receiver");
    GUI_PutString5_7(10,48,"Ready");
    LCM_Refresh();
    updatalcd = 1;
    // Main loop
    while (TRUE) 
    {
        while(!basicRfPacketIsReady());              //等待接收完成（当接收完一个数据包后，在接收中断函数basicRfRxFrmDoneIsr中
                                                     //将rxi.isReady = TRUE）
         if(updatalcd)                               //收到第一数据包，测试开始，更新显示
         {
             GUI_ClearScreen();                       //LCD清屏
             GUI_PutString5_7(25,6,"OURS-CC2530");    //在LCD上显示相应的文字             
             GUI_PutString5_7(10,22,"PER:");                          
             GUI_PutString5_7(10,35,"RSSI:");
             GUI_PutString5_7(10,48,"Recv'd:");
             LCM_Refresh();
             updatalcd = 0;
         }
        if(basicRfReceive((uint8*)&rxPacket, MAX_PAYLOAD_LENGTH, &rssi)>0)  //复制接收到的数据 (rxi.pPayload)和得到RSSI值
        {
	    UINT32_NTOH(rxPacket.seqNumber);
            segNumber = rxPacket.seqNumber;

            if(resetStats)                                         //复位统计确认（重新编号）
            {
              rxStats.expectedSeqNum = segNumber;
              resetStats=FALSE;
            }
        
            rxStats.rssiSum -= perRssiBuf[perRssiBufCounter];       //减去原来RSSI的值
            perRssiBuf[perRssiBufCounter] =  rssi;                  //存储新的RSSI的值

            rxStats.rssiSum += perRssiBuf[perRssiBufCounter];        //加上新的RSSI的值
            if (++perRssiBufCounter == RSSI_AVG_WINDOW_SIZE)         //取32个RSSI的值来求平均
            {
                perRssiBufCounter = 0;                               // 计数清零
            }

            if (rxStats.expectedSeqNum == segNumber)                 //检查收到的数据包是否正确
            {
                rxStats.expectedSeqNum++;                            //正确，加一
            }
            else if (rxStats.expectedSeqNum < segNumber)             //计算丢失数据包个数
            {
                rxStats.lostPkts += segNumber - rxStats.expectedSeqNum;
                rxStats.expectedSeqNum = segNumber + 1;
            }

            else 
            {
                rxStats.expectedSeqNum = segNumber + 1;
                rxStats.rcvdPkts = 0;
                rxStats.lostPkts = 0;
            }
            rxStats.rcvdPkts++;                                      //接收到的数据包计数

            if(halkeycmd()==HAL_BUTTON_4)                          //重新测量 SW4 （用于重新测量）
            {
                resetStats = TRUE;
                rxStats.rcvdPkts = 1;
                rxStats.lostPkts = 0;
            }
                                     
          //  ctrPCA9554LED(0,0);
        } 
        sprintf(s, (char*)"%d%d%d%d/1000",  ((INT16)((int)((rxStats.lostPkts*1000)/(rxStats.lostPkts+rxStats.rcvdPkts))/1000)), 
                            ((INT16)((int)((rxStats.lostPkts*1000)/(rxStats.lostPkts+rxStats.rcvdPkts))%1000/100)),
                            ((INT16)((int)((rxStats.lostPkts*1000)/(rxStats.lostPkts+rxStats.rcvdPkts))%100/10)),
                            ((INT16)((int)((rxStats.lostPkts*1000)/(rxStats.lostPkts+rxStats.rcvdPkts))%10))); //将误码率结果转换为字符形式
        do
        {
          if(s[i] == '0')
          {
             s[i] = ' ';
             num = 1;
          }
         else num = 0;
         i++;
        }while(num);
         i = 0;
        GUI_PutString5_7(48,22,(char *)s);                      //显示误码率结果           
        rssisave = -(rxStats.rssiSum/32);                       //将RSSI值转化为正值
        sprintf(s, (char*)"-%d%d dBm",  ((INT16)((int)rssisave/10)), ((INT16)((int)rssisave%10))); //将RSSI值转换为字符形式
        GUI_PutString5_7(48,35,(char *)s);                      //显示RSSI值        
        sprintf(s, (char*)"%d%d%d%d%d%d%d ",  ((INT16)((int)rxStats.rcvdPkts/1000000)), 
                            ((INT16)((int)rxStats.rcvdPkts%1000000/100000)),((INT16)((int)rxStats.rcvdPkts%100000/10000)),
                            ((INT16)((int)rxStats.rcvdPkts%10000/1000)),((INT16)((int)rxStats.rcvdPkts%1000/100)),
                            ((INT16)((int)rxStats.rcvdPkts%100/10)),((INT16)((int)rxStats.rcvdPkts%10))); //将光照结果转换为字符串
        do
       {
          if(s[i] == '0')
          {
             s[i] = ' ';
              num = 1;
          }
          else num = 0;
          i++;
       }while(num);
        i = 0;
        GUI_PutString5_7(48,48,(char *)s);                      //显示结果
        LCM_Refresh();                                          //将缓存内容更新到LCD
         
    }
}

/**************************************************************************************************
 * 函数名称：appTransmitter
 *
 * 功能描述：发送模块服务程序，包括设置发送数据包个数、发射功率、速度等。
 *                               
 *
 * 参    数：无
 *
 * 返 回 值：无
 **************************************************************************************************/
static void appTransmitter()
{
    uint32 burstSize=0;
    uint32 pktsSent=0;
    uint8 appTxPower;
    char  s[16];
    uint8 n;

    basicRfConfig.myAddr = TX_ADDR;              //配置网络地址（2530）
    if(basicRfInit(&basicRfConfig)==FAILED) 
    {
      ctrPCA9554FLASHLED(5);                    //RF初始化不成功，则所有的LED5闪烁
    }

    appTxPower = appSelectOutputPower();         //发射功率设置
    halRfSetTxPower(appTxPower);                 //配置TXPOWER功率大小寄存器

    burstSize = appSelectBurstSize();            //发送数据包大小设置（1000开始）

    basicRfReceiveOff();                         //关闭RF的接收功能

    n= appSelectRate();                          //设置发送速度（即数据包发送间隔设置）
    appConfigTimer(n);                           //配置发送间隔用的定时器                           

    txPacket.seqNumber = 0;                     //初始化数据包的有效负荷
    for(n = 0; n < sizeof(txPacket.padding); n++) 
    {
        txPacket.padding[n] = n;
    }

    
   GUI_ClearScreen();                             //LCD清屏
   GUI_PutString5_7(25,6,"OURS-CC2530");          //在LCD上显示相应的文字             
   GUI_PutString5_7(10,22,"PER Tester  ");                          
   GUI_PutString5_7(10,35,"SW4 Push");
   GUI_PutString5_7(10,48,"start/stop");
   LCM_Refresh();

    // Main loop
    while (TRUE) 
    {
        while(halkeycmd() != HAL_BUTTON_4);         //等待SW4按下，开始测试
        appStartStop();                             //开始发送
       
        GUI_ClearScreen();                          //LCD清屏
        GUI_PutString5_7(25,6,"OURS-CC2530");       //在LCD上显示相应的文字             
        GUI_PutString5_7(10,22,"PER Tester  ");                          
        GUI_PutString5_7(10,35,"Transmitter");
        GUI_PutString5_7(10,48,"Sent: ");
        LCM_Refresh();

        while(appStarted) 
        {
            if(halkeycmd() == HAL_BUTTON_4)        //使用SW4控制发送或停止
            {
                appStartStop();
            }

            if (pktsSent < burstSize) 
            {
                if( appState == TRANSMIT_PACKET )  //发送中（appState通过定时器中断函数来置位）
                {
                    UINT32_HTON(txPacket.seqNumber);

                    basicRfSendPacket(RX_ADDR, (uint8*)&txPacket, PACKET_SIZE);  //发送数据

                    UINT32_NTOH(txPacket.seqNumber);
                    txPacket.seqNumber++;

                    pktsSent++;
                    sprintf(s,(char *)" %d         ",pktsSent);  
                    GUI_PutString5_7(48,48,(char *)s);                 //显示结果
                    LCM_Refresh();
                    appState = IDLE;                                   //等待定时器的中断，将appState = TRANSMIT_PACKET 进程下一个数据包发送
                    //ctrPCA9554LED(0,1);                              //点亮LED1，以指示设备正常运行  
                }
            }
            else
             appStarted = !appStarted;                                 //完成一次测试
        }

        pktsSent = 0;                                                  //发送计数清零
        txPacket.seqNumber = 0;                                        //网络字节清零
        ctrPCA9554LED(0,0);                                            //关闭LED3
        GUI_ClearScreen();                                             //LCD清屏
        GUI_PutString5_7(25,6,"OURS-CC2530");                          //在LCD上显示相应的文字             
        GUI_PutString5_7(10,22,"PER Tester  ");                          
        GUI_PutString5_7(10,35,"SW4 Push");
        GUI_PutString5_7(10,48,"start/stop");
        LCM_Refresh();
    }
}

/**************************************************************************************************
 * 函数名称：main
 *
 * 功能描述：初始化出差CC2530无线通信的一些基本参数，通过按键选择模块的应用类型（发送或接收）。
 *                               
 *
 * 参    数：无
 *
 * 返 回 值：无
 **************************************************************************************************/
void main (void)
{
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



