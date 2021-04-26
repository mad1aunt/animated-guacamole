/**********************************************************************************************************************************************************
* 文 件 名：per_test_menu.c
×
* 功    能：设置测试前，各个模块的工作参数和个参数设置菜单的显示。         
*
*
*
* 版    本：V1.0
* 作    者：WUXIANHAI
* 日    期：2011.2.16
* 奥尔斯科技主页：www.ourselec.com
**********************************************************************************************************************************************************/

#include "per_test_menu.h"
#include "hal_rf.h"
#include "LCD.h"


extern uint8 halkeycmd(void);              //获取按键值函数

/**************************************************************************************************
 * 函数名称：appSelectChannel
 *
 * 功能描述：通信信道选择
 *                               
 *
 * 参    数：无
 *
 * 返 回 值：channel -- 所选择的信道（11 - 26）
 **************************************************************************************************/
uint8 appSelectChannel(void)        
{
  uint8 key;
  uint8 channel = 11;
    
  GUI_ClearScreen();                             //LCD清屏
  GUI_PutString5_7(25,6,"OURS-CC2530");          //在LCD上显示相应的文字             
  GUI_PutString5_7(10,22,"Channel: ");                          
  GUI_PutString5_7(10,35,"SW2 +   SW3 -");
  GUI_PutString5_7(10,48,"11 - 2405 MHz");
  LCM_Refresh();
  
  do                                            //通过SW2和SW3选择信道
 {
   key = halkeycmd();
   if(key == 2)                                 //SW2按下，信道加一
   {
     channel = channel+1;
     if(channel > 26)
     {
       channel = 26;
     }
   }
    if(key == 3)                                 //SW3按下，信道减一
   {
     channel = channel-1;
     if(channel < 11)
     {
       channel = 11;
     }
   }
  switch(channel)                                //信道选择菜单显示
 {
   case 11:
   GUI_PutString5_7(10,48,"11 - 2405 MHz");
   break;
   case 12:
   GUI_PutString5_7(10,48,"12 - 2410 MHz");
   break;
   case 13:
   GUI_PutString5_7(10,48,"13 - 2415 MHz");
   break;
   case 14:
   GUI_PutString5_7(10,48,"14 - 2420 MHz");
   break;
   case 15:
   GUI_PutString5_7(10,48,"15 - 2425 MHz");
   break;
   case 16:
   GUI_PutString5_7(10,48,"16 - 2430 MHz");
   break;
   case 17:
   GUI_PutString5_7(10,48,"17 - 2435 MHz");
   break;
   case 18:
   GUI_PutString5_7(10,48,"18 - 2440 MHz");
   break;
   case 19:
   GUI_PutString5_7(10,48,"19 - 2445 MHz");
   break;
   case 20:
   GUI_PutString5_7(10,48,"20 - 2450 MHz");
   break;
   case 21:
   GUI_PutString5_7(10,48,"21 - 2455 MHz");
   break;
   case 22:
   GUI_PutString5_7(10,48,"22 - 2460 MHz");
   break;
   case 23:
   GUI_PutString5_7(10,48,"23 - 2465 MHz");
   break;
   case 24:
   GUI_PutString5_7(10,48,"24 - 2470 MHz");
   break;
   case 25:
   GUI_PutString5_7(10,48,"25 - 2475 MHz");
   break;
   case 26:
   GUI_PutString5_7(10,48,"26 - 2480 MHz");
   break;
   default: break;
 }
  LCM_Refresh();
 }while(key != 4);                                //等待SW4按下，完成设置
 
return channel;
}

/**************************************************************************************************
 * 函数名称：appSelectMode
 *
 * 功能描述：设置模块应用模式
 *                               
 *
 * 参    数：无
 *
 * 返 回 值：MODE_TX -- 发送模式
 *           MODE_RX -- 接收模式
 **************************************************************************************************/
uint8 appSelectMode(void)
{   
   uint8 key;
   uint8 mode;
  GUI_ClearScreen();                             //LCD清屏
  GUI_PutString5_7(25,6,"OURS-CC2530");          //在LCD上显示相应的文字             
  GUI_PutString5_7(10,22,"Operating Mode: ");                          
  GUI_PutString5_7(10,35,"SW2 -> MODE_TX");
  GUI_PutString5_7(10,48,"SW3 -> MODE_RX");
  LCM_Refresh();
  
 do                                              //通过SW2和SW3选择选择模式
 {
   key = halkeycmd();
     if(key == 2)                                //SW2发送模式
    {
      GUI_ClearScreen();
      GUI_PutString5_7(25,6,"OURS-CC2530");      //在LCD上显示相应的文字              
      GUI_PutString5_7(10,22,"Device Mode: ");                          
      GUI_PutString5_7(10,35,"Transmitter");
      LCM_Refresh();
      
      mode = MODE_TX;
    }
     if(key == 3)                                //SW3接收模式
    {
      GUI_ClearScreen();
      GUI_PutString5_7(25,6,"OURS-CC2530");      //在LCD上显示相应的文字          
      GUI_PutString5_7(10,22,"Device Mode: ");                            
      GUI_PutString5_7(10,35,"Receiver");
      LCM_Refresh();
      
      mode = MODE_RX;
    }
 }while(key != 4);                               //等待SW4按下，完成设置

    return mode;  
}

/**************************************************************************************************
 * 函数名称：appSelectRate
 *
 * 功能描述：设置发送数据包的速度
 *                               
 *
 * 参    数：无
 *
 * 返 回 值：ratesave -- 发送数据包速度（5、10、20、50）
 **************************************************************************************************/
uint8 appSelectRate(void)
{
  uint8 key;
  uint8 rate = 1;
  uint8 ratesave;
    
  GUI_ClearScreen();                             //LCD清屏
  GUI_PutString5_7(25,6,"OURS-CC2530");          //在LCD上显示相应的文字             
  GUI_PutString5_7(10,22,"Packet rate:  ");                          
  GUI_PutString5_7(10,35,"SW2 +   SW3 -");
  GUI_PutString5_7(10,48,"10/second");
  LCM_Refresh();
  
  do                                             //通过SW2和SW3选择发送的数据包的速度
 {
   key = halkeycmd();                            //读取按键值
   if(key == 2)                                  //数据包发送速度加一个等级               
   {
     rate = rate+1;
     if(rate > 4)
     {
       rate = 4;
     }
   }
    if(key == 3)                                 //数据包发送速度减一个等级  
   {
     rate = rate-1;
     if(rate < 1)
     {
       rate = 1;
     }
   }
  switch(rate)                                    //发送数据包速度择菜单显示
 {
   case 1:
    ratesave = 5; 
   GUI_PutString5_7(10,48,"5/second");
   break;
   case 2:
    ratesave = 10;
   GUI_PutString5_7(10,48,"10/second");
   break;
   case 3:
     ratesave = 20;
   GUI_PutString5_7(10,48,"20/second");
   break;
   case 4:
    ratesave = 50;
   GUI_PutString5_7(10,48,"50/second");
   break;
   default: break;
 }
 LCM_Refresh();
}while(key != 4);                                   //等待SW4按下，完成设置
 return ratesave;
}

/**************************************************************************************************
 * 函数名称：appSelectBurstSize
 *
 * 功能描述：设置发送数据包数量。
 *                               
 *
 * 参    数：无
 *
 * 返 回 值：burstSizes -- 发送数据包数量（1000、10000、100000、1000000）
 **************************************************************************************************/
uint32 appSelectBurstSize(void)
{
  uint8 key;
  uint32 burstSizes = 1000;

  GUI_ClearScreen();                             //LCD清屏
  GUI_PutString5_7(25,6,"OURS-CC2530");          //在LCD上显示相应的文字             
  GUI_PutString5_7(10,22,"Burst Size:  ");                          
  GUI_PutString5_7(10,35,"SW2 +   SW3 -");
  GUI_PutString5_7(10,48,"1000 Pkts");
  LCM_Refresh();
  
   do                                            //通过SW2和SW3选择发送的数据包数量
 {
   key = halkeycmd();                            //读取按键值
   if(key == 2)                                  //数据包数量加一个等级
   {
     burstSizes = burstSizes*10;
     if(burstSizes > BURST_SIZE_4)
     {
       burstSizes = BURST_SIZE_4;
     }
   }
    if(key == 3)                                 //数据包数量减一个等级
   {
     burstSizes = burstSizes/10;
     if(burstSizes < BURST_SIZE_1)
     {
       burstSizes = BURST_SIZE_1;
     }
   }
  switch(burstSizes)                             //发送数据包量选择菜单显示
 {
   case BURST_SIZE_1:
   GUI_PutString5_7(10,48,"1000 Pkts ");
   break;
   case BURST_SIZE_2:
   GUI_PutString5_7(10,48,"10000 Pkts ");
   break;
   case BURST_SIZE_3:
   GUI_PutString5_7(10,48,"100000 Pkts ");
   break;
   case BURST_SIZE_4:
   GUI_PutString5_7(10,48,"1000000 Pkts ");
   break;
   default: break;
 }
 LCM_Refresh();
}while(key != 4);                                  //等待SW4按下，完成设置

  return burstSizes;
  
} 

/**************************************************************************************************
 * 函数名称：appSelectOutputPower
 *
 * 功能描述：设置发送功率。
 *                               
 *
 * 参    数：无
 *
 * 返 回 值：power -- 发送功率
 **************************************************************************************************/
uint8 appSelectOutputPower(void)
{   
   uint8 key;
   int8 power = 0;
    
  GUI_ClearScreen();                             //LCD清屏
  GUI_PutString5_7(25,6,"OURS-CC2530");          //在LCD上显示相应的文字             
  GUI_PutString5_7(10,22,"TX Output Power: ");                          
  GUI_PutString5_7(10,35,"SW2 +   SW3 -");
  GUI_PutString5_7(10,48,"-20dBm");
  LCM_Refresh();
  
   do                                            //通过按键，选择发送功率
 {
   key = halkeycmd();                            //读取按键值
   if(key == 2)                                  //按键2按下
   {
     power = power+1;                            //发送功率加一个等级
     if(power > 3)
     {
       power = 3;
     }
   }
    if(key == 3)                                 //按键3按下
   {
     power = power-1;                            //发送功率减一个等级
     if(power < 0)
     {
       power = 0;
     }
   }
  switch(power)                                 //发送功率设置菜单显示
 {
   case 0:
   GUI_PutString5_7(10,48,"-20dBm");
   break;
   case 1:
   GUI_PutString5_7(10,48,"-3dBm ");
   break;
   case 2:
   GUI_PutString5_7(10,48,"0dBm ");
   break;
   case 3:
   GUI_PutString5_7(10,48,"4dBm ");
   break;
   default: break;
 }
 LCM_Refresh();
}while(key != 4);                                //等待SW4按下，完成设置

  return power;                                  //返回设置值
}



