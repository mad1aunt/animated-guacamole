/**********************************************************************************************************************************************************
* �� �� ����App_per_test_main.c
��
* ��    �ܣ�ʵ��ʮ�� �����ʲ���ʵ��
*
*            ��ʵ����Ҫ����ѧ��������CC2530RF���ܻ����ϡ�һ��������ͨ��Ӧ�ã���ʵ������������Բ�ͬ����
*            
*         ��ͬͨ�ž�����������Լ��źŵ�ǿ������ɱ�ʵ����Ҫ����ģ�飬һ������Ϊ����ģ�飬һ������Ϊ����
*
*         ģ�飻���з���ģ����Ҫ��ͨ�����ϰ������ò�ͬ�ķ��Ͳ�����Ȼ�������ݰ�������ģ����շ���ģ�����
*   
*         �ݰ���Ȼ����������ʺ��źŵ�ǿ�ȡ�
*
*            ���а������ܷ������£�
*                SW1 --- ��ʼ���ԣ����빦��ѡ��˵���
*                SW2 --- ���ù��ܼ�
*                SW3 --- ���ù��ܼ�
*                SW4 --- ȷ����ť
*
*               ��ÿ���һ���������û�ѡ�񣬶���ͨ��SW4��ȷ����Ȼ�������һ���������ã����з���ģʽ�µķ���
*
*           ��ʼ��ֹͣҲ��ͨ��SW4���Ƶġ��ڲ����У�����ģ�����ͨ��SW4����λ���Խ����
*
*               ����LED6Ϊ����ָʾ�ƣ�������������ʱ��LED5��Ϊ��״̬��
*
*
*           ��\include\�ļ���\source\�ļ��а����˺�RF��ص�һЩ��ͺ������û�ʹ����Щ��
*           �ͺ������Լ򻯶�CC2530��RF��������ߴ���Ŀɶ��ԣ���ʵ���о�ʹ�������е�һЩ��ͺ�����
*
* ע    �⣺��ʵ������Ӳ����Դ��
*           OURS-CC2530RF�� 2��
*           ��LCD���������� 2��
*           
*           
*
* ��    ����V1.0
* ��    �ߣ�wuxianhai
* ��    �ڣ�2011.2.16
* �¶�˹�Ƽ���ҳ��www.ourselec.com
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

// Ӧ��״̬
#define IDLE                      0
#define TRANSMIT_PACKET           1

//����
#define HAL_BUTTON_1              1
#define HAL_BUTTON_2              2
#define HAL_BUTTON_3              3
#define HAL_BUTTON_4              4
#define HAL_BUTTON_5              5
#define HAL_BUTTON_6              6

static basicRfCfg_t basicRfConfig;   //����RF�������ýṹ��
static perTestPacket_t txPacket;     //�������ݰ�
static perTestPacket_t rxPacket;     //�������ݰ�
static volatile uint8 appState;      //Ӧ��״̬
static volatile uint8 appStarted;    //������״̬����ʼ��ֹͣ��


static void appTimerISR(void);        //32K��ʱ���жϳ���    
static void appStartStop(void);       //32K��ʱ��������رգ����Կ�ʼ��ֹͣ��
static void appTransmitter();         //���Է��ͺ���
static void appReceiver();            //���Խ��պ���

extern void halboardinit(void);                      //Ӳ����ʼ������
extern void ctrPCA9554FLASHLED(uint8 led);           //IIC�ƿ��ƺ���
extern uint8 halkeycmd(void);                        //��ȡ����ֵ����
extern void ctrPCA9554LED(uint8 led,uint8 operation);//LED���ƺ���

/**************************************************************************************************
 * �������ƣ�appTimerISR
 *
 * ����������32K��ʱ���жϷ�����򣬸ı�Ӧ��״̬��
 *                     
 *
 * ��    ������
 *
 * �� �� ֵ����
 **************************************************************************************************/
static void appTimerISR(void)
{
    appState = TRANSMIT_PACKET;    //�ı�Ӧ��״̬
}

/**************************************************************************************************
 * �������ƣ�appStartStop
 *
 * ����������ͨ��������ر�32K��ʱ����ʵ�ֲ��ԵĿ�ʼ��رա�
 *                     
 *
 * ��    ������
 *
 * �� �� ֵ����
 **************************************************************************************************/
static void appStartStop(void)
{
    appStarted ^= 1;

    if(appStarted) {
        halTimer32kIntEnable();      //ʹ��32K��ʱ��
    }
    else {
        halTimer32kIntDisable();     //ʧ��32K��ʱ��
    }
}

/**************************************************************************************************
 * �������ƣ�appConfigTimer
 *
 * ����������ͨ����ʼ��32K��ʱ�������÷������ݵ��ٶȡ�
 *                     
 *
 * ��    ����rate -- �������ݵ����ݣ�������1��32768֮�䣩
 *
 * �� �� ֵ����
 **************************************************************************************************/
static void appConfigTimer(uint16 rate)
{
    halTimer32kInit(TIMER_32K_CLK_FREQ/rate);
    halTimer32kIntConnect(&appTimerISR);  //�жϺ���ָ��appTimerISR
}

/**************************************************************************************************
 * �������ƣ�appReceiver
 *
 * ��������������ģ�������򣬽��շ���ģ�鷢�͵����ݣ����������ʺ�RSSIֵ����󽫲��ԵĽ��ͨ��
 *           LCD��ʾ��
 *                     
 *
 * ��    ������
 *
 * �� �� ֵ����
 **************************************************************************************************/
static void appReceiver()
{
    uint32 segNumber=0;
    int16 perRssiBuf[RSSI_AVG_WINDOW_SIZE] = {0};    // RSSIֵ�洢
    uint8 perRssiBufCounter = 0;                     // RSSIֵ����������
    perRxStats_t rxStats = {0,0,0,0};                //����״̬
    int16 rssi;
    uint8 resetStats=FALSE; 
    
    char  s[16];
    uint8 i = 0;
    uint8 num;
    uint8 updatalcd;
    int8 rssisave;

    basicRfConfig.myAddr = RX_ADDR;                  //���������ַ��0xBEEF��
    if(basicRfInit(&basicRfConfig)==FAILED)          //��ʼ��RF���֣�������������жϺ��� basicRfRxFrmDoneIsr��
    {
      ctrPCA9554FLASHLED(5);                         //RF��ʼ�����ɹ�����LED5��˸
    }
    basicRfReceiveOn();                              //����RF�Ľ��չ���
    
    GUI_ClearScreen();                             //LCD����
    GUI_PutString5_7(25,6,"OURS-CC2530");          //��LCD����ʾ��Ӧ������             
    GUI_PutString5_7(10,22,"PER Tester  ");                          
    GUI_PutString5_7(10,35,"Receiver");
    GUI_PutString5_7(10,48,"Ready");
    LCM_Refresh();
    updatalcd = 1;
    // Main loop
    while (TRUE) 
    {
        while(!basicRfPacketIsReady());              //�ȴ�������ɣ���������һ�����ݰ����ڽ����жϺ���basicRfRxFrmDoneIsr��
                                                     //��rxi.isReady = TRUE��
         if(updatalcd)                               //�յ���һ���ݰ������Կ�ʼ��������ʾ
         {
             GUI_ClearScreen();                       //LCD����
             GUI_PutString5_7(25,6,"OURS-CC2530");    //��LCD����ʾ��Ӧ������             
             GUI_PutString5_7(10,22,"PER:");                          
             GUI_PutString5_7(10,35,"RSSI:");
             GUI_PutString5_7(10,48,"Recv'd:");
             LCM_Refresh();
             updatalcd = 0;
         }
        if(basicRfReceive((uint8*)&rxPacket, MAX_PAYLOAD_LENGTH, &rssi)>0)  //���ƽ��յ������� (rxi.pPayload)�͵õ�RSSIֵ
        {
	    UINT32_NTOH(rxPacket.seqNumber);
            segNumber = rxPacket.seqNumber;

            if(resetStats)                                         //��λͳ��ȷ�ϣ����±�ţ�
            {
              rxStats.expectedSeqNum = segNumber;
              resetStats=FALSE;
            }
        
            rxStats.rssiSum -= perRssiBuf[perRssiBufCounter];       //��ȥԭ��RSSI��ֵ
            perRssiBuf[perRssiBufCounter] =  rssi;                  //�洢�µ�RSSI��ֵ

            rxStats.rssiSum += perRssiBuf[perRssiBufCounter];        //�����µ�RSSI��ֵ
            if (++perRssiBufCounter == RSSI_AVG_WINDOW_SIZE)         //ȡ32��RSSI��ֵ����ƽ��
            {
                perRssiBufCounter = 0;                               // ��������
            }

            if (rxStats.expectedSeqNum == segNumber)                 //����յ������ݰ��Ƿ���ȷ
            {
                rxStats.expectedSeqNum++;                            //��ȷ����һ
            }
            else if (rxStats.expectedSeqNum < segNumber)             //���㶪ʧ���ݰ�����
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
            rxStats.rcvdPkts++;                                      //���յ������ݰ�����

            if(halkeycmd()==HAL_BUTTON_4)                          //���²��� SW4 ���������²�����
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
                            ((INT16)((int)((rxStats.lostPkts*1000)/(rxStats.lostPkts+rxStats.rcvdPkts))%10))); //�������ʽ��ת��Ϊ�ַ���ʽ
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
        GUI_PutString5_7(48,22,(char *)s);                      //��ʾ�����ʽ��           
        rssisave = -(rxStats.rssiSum/32);                       //��RSSIֵת��Ϊ��ֵ
        sprintf(s, (char*)"-%d%d dBm",  ((INT16)((int)rssisave/10)), ((INT16)((int)rssisave%10))); //��RSSIֵת��Ϊ�ַ���ʽ
        GUI_PutString5_7(48,35,(char *)s);                      //��ʾRSSIֵ        
        sprintf(s, (char*)"%d%d%d%d%d%d%d ",  ((INT16)((int)rxStats.rcvdPkts/1000000)), 
                            ((INT16)((int)rxStats.rcvdPkts%1000000/100000)),((INT16)((int)rxStats.rcvdPkts%100000/10000)),
                            ((INT16)((int)rxStats.rcvdPkts%10000/1000)),((INT16)((int)rxStats.rcvdPkts%1000/100)),
                            ((INT16)((int)rxStats.rcvdPkts%100/10)),((INT16)((int)rxStats.rcvdPkts%10))); //�����ս��ת��Ϊ�ַ���
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
        GUI_PutString5_7(48,48,(char *)s);                      //��ʾ���
        LCM_Refresh();                                          //���������ݸ��µ�LCD
         
    }
}

/**************************************************************************************************
 * �������ƣ�appTransmitter
 *
 * ��������������ģ�������򣬰������÷������ݰ����������书�ʡ��ٶȵȡ�
 *                               
 *
 * ��    ������
 *
 * �� �� ֵ����
 **************************************************************************************************/
static void appTransmitter()
{
    uint32 burstSize=0;
    uint32 pktsSent=0;
    uint8 appTxPower;
    char  s[16];
    uint8 n;

    basicRfConfig.myAddr = TX_ADDR;              //���������ַ��2530��
    if(basicRfInit(&basicRfConfig)==FAILED) 
    {
      ctrPCA9554FLASHLED(5);                    //RF��ʼ�����ɹ��������е�LED5��˸
    }

    appTxPower = appSelectOutputPower();         //���书������
    halRfSetTxPower(appTxPower);                 //����TXPOWER���ʴ�С�Ĵ���

    burstSize = appSelectBurstSize();            //�������ݰ���С���ã�1000��ʼ��

    basicRfReceiveOff();                         //�ر�RF�Ľ��չ���

    n= appSelectRate();                          //���÷����ٶȣ������ݰ����ͼ�����ã�
    appConfigTimer(n);                           //���÷��ͼ���õĶ�ʱ��                           

    txPacket.seqNumber = 0;                     //��ʼ�����ݰ�����Ч����
    for(n = 0; n < sizeof(txPacket.padding); n++) 
    {
        txPacket.padding[n] = n;
    }

    
   GUI_ClearScreen();                             //LCD����
   GUI_PutString5_7(25,6,"OURS-CC2530");          //��LCD����ʾ��Ӧ������             
   GUI_PutString5_7(10,22,"PER Tester  ");                          
   GUI_PutString5_7(10,35,"SW4 Push");
   GUI_PutString5_7(10,48,"start/stop");
   LCM_Refresh();

    // Main loop
    while (TRUE) 
    {
        while(halkeycmd() != HAL_BUTTON_4);         //�ȴ�SW4���£���ʼ����
        appStartStop();                             //��ʼ����
       
        GUI_ClearScreen();                          //LCD����
        GUI_PutString5_7(25,6,"OURS-CC2530");       //��LCD����ʾ��Ӧ������             
        GUI_PutString5_7(10,22,"PER Tester  ");                          
        GUI_PutString5_7(10,35,"Transmitter");
        GUI_PutString5_7(10,48,"Sent: ");
        LCM_Refresh();

        while(appStarted) 
        {
            if(halkeycmd() == HAL_BUTTON_4)        //ʹ��SW4���Ʒ��ͻ�ֹͣ
            {
                appStartStop();
            }

            if (pktsSent < burstSize) 
            {
                if( appState == TRANSMIT_PACKET )  //�����У�appStateͨ����ʱ���жϺ�������λ��
                {
                    UINT32_HTON(txPacket.seqNumber);

                    basicRfSendPacket(RX_ADDR, (uint8*)&txPacket, PACKET_SIZE);  //��������

                    UINT32_NTOH(txPacket.seqNumber);
                    txPacket.seqNumber++;

                    pktsSent++;
                    sprintf(s,(char *)" %d         ",pktsSent);  
                    GUI_PutString5_7(48,48,(char *)s);                 //��ʾ���
                    LCM_Refresh();
                    appState = IDLE;                                   //�ȴ���ʱ�����жϣ���appState = TRANSMIT_PACKET ������һ�����ݰ�����
                    //ctrPCA9554LED(0,1);                              //����LED1����ָʾ�豸��������  
                }
            }
            else
             appStarted = !appStarted;                                 //���һ�β���
        }

        pktsSent = 0;                                                  //���ͼ�������
        txPacket.seqNumber = 0;                                        //�����ֽ�����
        ctrPCA9554LED(0,0);                                            //�ر�LED3
        GUI_ClearScreen();                                             //LCD����
        GUI_PutString5_7(25,6,"OURS-CC2530");                          //��LCD����ʾ��Ӧ������             
        GUI_PutString5_7(10,22,"PER Tester  ");                          
        GUI_PutString5_7(10,35,"SW4 Push");
        GUI_PutString5_7(10,48,"start/stop");
        LCM_Refresh();
    }
}

/**************************************************************************************************
 * �������ƣ�main
 *
 * ������������ʼ������CC2530����ͨ�ŵ�һЩ����������ͨ������ѡ��ģ���Ӧ�����ͣ����ͻ���գ���
 *                               
 *
 * ��    ������
 *
 * �� �� ֵ����
 **************************************************************************************************/
void main (void)
{
    uint8 appMode;

    appState = IDLE;
    appStarted = FALSE;

    basicRfConfig.panId = PAN_ID;              //��������PANIDΪ2010   
    basicRfConfig.ackRequest = FALSE;          //��������ʱ������ACK��Ϣ

    halboardinit();                           //��ʼ�������Χ�豸(����LED LCD �Ͱ�����)

    if(halRfInit()==FAILED)                   //��ʼ��RF��Ӳ������
    {                  
      ctrPCA9554FLASHLED(5);                 //RF��ʼ�����ɹ��������е�LED5��˸
    }

    ctrPCA9554FLASHLED(6);                   //����LED6����ָʾ�豸��������    
    
    GUI_PutString5_7(10,22,"PER test");      //��LCD����ʾ��Ӧ������
    GUI_PutString5_7(10,35,"SW1 -> Start");
    LCM_Refresh();

    while (halkeycmd() != HAL_BUTTON_1);    //�ȴ�����1���£�������һ���˵�
    halMcuWaitMs(350);                      //��ʱ

    basicRfConfig.channel = appSelectChannel(); //RF���ֵ��ŵ�ѡ������

    appMode = appSelectMode();              //ѡ���豸�Ĺ���ģʽ

    if(appMode == MODE_TX) {                //����ģʽ
        appTransmitter();                      
    }
    else if(appMode == MODE_RX) {           //����ģʽ
        appReceiver();
    }

}



