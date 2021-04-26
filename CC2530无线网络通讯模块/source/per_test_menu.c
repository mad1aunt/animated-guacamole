/**********************************************************************************************************************************************************
* �� �� ����per_test_menu.c
��
* ��    �ܣ����ò���ǰ������ģ��Ĺ��������͸��������ò˵�����ʾ��         
*
*
*
* ��    ����V1.0
* ��    �ߣ�WUXIANHAI
* ��    �ڣ�2011.2.16
* �¶�˹�Ƽ���ҳ��www.ourselec.com
**********************************************************************************************************************************************************/

#include "per_test_menu.h"
#include "hal_rf.h"
#include "LCD.h"


extern uint8 halkeycmd(void);              //��ȡ����ֵ����

/**************************************************************************************************
 * �������ƣ�appSelectChannel
 *
 * ����������ͨ���ŵ�ѡ��
 *                               
 *
 * ��    ������
 *
 * �� �� ֵ��channel -- ��ѡ����ŵ���11 - 26��
 **************************************************************************************************/
uint8 appSelectChannel(void)        
{
  uint8 key;
  uint8 channel = 11;
    
  GUI_ClearScreen();                             //LCD����
  GUI_PutString5_7(25,6,"OURS-CC2530");          //��LCD����ʾ��Ӧ������             
  GUI_PutString5_7(10,22,"Channel: ");                          
  GUI_PutString5_7(10,35,"SW2 +   SW3 -");
  GUI_PutString5_7(10,48,"11 - 2405 MHz");
  LCM_Refresh();
  
  do                                            //ͨ��SW2��SW3ѡ���ŵ�
 {
   key = halkeycmd();
   if(key == 2)                                 //SW2���£��ŵ���һ
   {
     channel = channel+1;
     if(channel > 26)
     {
       channel = 26;
     }
   }
    if(key == 3)                                 //SW3���£��ŵ���һ
   {
     channel = channel-1;
     if(channel < 11)
     {
       channel = 11;
     }
   }
  switch(channel)                                //�ŵ�ѡ��˵���ʾ
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
 }while(key != 4);                                //�ȴ�SW4���£��������
 
return channel;
}

/**************************************************************************************************
 * �������ƣ�appSelectMode
 *
 * ��������������ģ��Ӧ��ģʽ
 *                               
 *
 * ��    ������
 *
 * �� �� ֵ��MODE_TX -- ����ģʽ
 *           MODE_RX -- ����ģʽ
 **************************************************************************************************/
uint8 appSelectMode(void)
{   
   uint8 key;
   uint8 mode;
  GUI_ClearScreen();                             //LCD����
  GUI_PutString5_7(25,6,"OURS-CC2530");          //��LCD����ʾ��Ӧ������             
  GUI_PutString5_7(10,22,"Operating Mode: ");                          
  GUI_PutString5_7(10,35,"SW2 -> MODE_TX");
  GUI_PutString5_7(10,48,"SW3 -> MODE_RX");
  LCM_Refresh();
  
 do                                              //ͨ��SW2��SW3ѡ��ѡ��ģʽ
 {
   key = halkeycmd();
     if(key == 2)                                //SW2����ģʽ
    {
      GUI_ClearScreen();
      GUI_PutString5_7(25,6,"OURS-CC2530");      //��LCD����ʾ��Ӧ������              
      GUI_PutString5_7(10,22,"Device Mode: ");                          
      GUI_PutString5_7(10,35,"Transmitter");
      LCM_Refresh();
      
      mode = MODE_TX;
    }
     if(key == 3)                                //SW3����ģʽ
    {
      GUI_ClearScreen();
      GUI_PutString5_7(25,6,"OURS-CC2530");      //��LCD����ʾ��Ӧ������          
      GUI_PutString5_7(10,22,"Device Mode: ");                            
      GUI_PutString5_7(10,35,"Receiver");
      LCM_Refresh();
      
      mode = MODE_RX;
    }
 }while(key != 4);                               //�ȴ�SW4���£��������

    return mode;  
}

/**************************************************************************************************
 * �������ƣ�appSelectRate
 *
 * �������������÷������ݰ����ٶ�
 *                               
 *
 * ��    ������
 *
 * �� �� ֵ��ratesave -- �������ݰ��ٶȣ�5��10��20��50��
 **************************************************************************************************/
uint8 appSelectRate(void)
{
  uint8 key;
  uint8 rate = 1;
  uint8 ratesave;
    
  GUI_ClearScreen();                             //LCD����
  GUI_PutString5_7(25,6,"OURS-CC2530");          //��LCD����ʾ��Ӧ������             
  GUI_PutString5_7(10,22,"Packet rate:  ");                          
  GUI_PutString5_7(10,35,"SW2 +   SW3 -");
  GUI_PutString5_7(10,48,"10/second");
  LCM_Refresh();
  
  do                                             //ͨ��SW2��SW3ѡ���͵����ݰ����ٶ�
 {
   key = halkeycmd();                            //��ȡ����ֵ
   if(key == 2)                                  //���ݰ������ٶȼ�һ���ȼ�               
   {
     rate = rate+1;
     if(rate > 4)
     {
       rate = 4;
     }
   }
    if(key == 3)                                 //���ݰ������ٶȼ�һ���ȼ�  
   {
     rate = rate-1;
     if(rate < 1)
     {
       rate = 1;
     }
   }
  switch(rate)                                    //�������ݰ��ٶ���˵���ʾ
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
}while(key != 4);                                   //�ȴ�SW4���£��������
 return ratesave;
}

/**************************************************************************************************
 * �������ƣ�appSelectBurstSize
 *
 * �������������÷������ݰ�������
 *                               
 *
 * ��    ������
 *
 * �� �� ֵ��burstSizes -- �������ݰ�������1000��10000��100000��1000000��
 **************************************************************************************************/
uint32 appSelectBurstSize(void)
{
  uint8 key;
  uint32 burstSizes = 1000;

  GUI_ClearScreen();                             //LCD����
  GUI_PutString5_7(25,6,"OURS-CC2530");          //��LCD����ʾ��Ӧ������             
  GUI_PutString5_7(10,22,"Burst Size:  ");                          
  GUI_PutString5_7(10,35,"SW2 +   SW3 -");
  GUI_PutString5_7(10,48,"1000 Pkts");
  LCM_Refresh();
  
   do                                            //ͨ��SW2��SW3ѡ���͵����ݰ�����
 {
   key = halkeycmd();                            //��ȡ����ֵ
   if(key == 2)                                  //���ݰ�������һ���ȼ�
   {
     burstSizes = burstSizes*10;
     if(burstSizes > BURST_SIZE_4)
     {
       burstSizes = BURST_SIZE_4;
     }
   }
    if(key == 3)                                 //���ݰ�������һ���ȼ�
   {
     burstSizes = burstSizes/10;
     if(burstSizes < BURST_SIZE_1)
     {
       burstSizes = BURST_SIZE_1;
     }
   }
  switch(burstSizes)                             //�������ݰ���ѡ��˵���ʾ
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
}while(key != 4);                                  //�ȴ�SW4���£��������

  return burstSizes;
  
} 

/**************************************************************************************************
 * �������ƣ�appSelectOutputPower
 *
 * �������������÷��͹��ʡ�
 *                               
 *
 * ��    ������
 *
 * �� �� ֵ��power -- ���͹���
 **************************************************************************************************/
uint8 appSelectOutputPower(void)
{   
   uint8 key;
   int8 power = 0;
    
  GUI_ClearScreen();                             //LCD����
  GUI_PutString5_7(25,6,"OURS-CC2530");          //��LCD����ʾ��Ӧ������             
  GUI_PutString5_7(10,22,"TX Output Power: ");                          
  GUI_PutString5_7(10,35,"SW2 +   SW3 -");
  GUI_PutString5_7(10,48,"-20dBm");
  LCM_Refresh();
  
   do                                            //ͨ��������ѡ���͹���
 {
   key = halkeycmd();                            //��ȡ����ֵ
   if(key == 2)                                  //����2����
   {
     power = power+1;                            //���͹��ʼ�һ���ȼ�
     if(power > 3)
     {
       power = 3;
     }
   }
    if(key == 3)                                 //����3����
   {
     power = power-1;                            //���͹��ʼ�һ���ȼ�
     if(power < 0)
     {
       power = 0;
     }
   }
  switch(power)                                 //���͹������ò˵���ʾ
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
}while(key != 4);                                //�ȴ�SW4���£��������

  return power;                                  //��������ֵ
}



