
/***********************************************************************************
    Filename:     per_test.h

    Description:  PER test header file

***********************************************************************************/

#ifndef PER_TEST_H
#define PER_TEST_H

/***********************************************************************************
* INCLUDES
*/
#include "hal_types.h"
#include "per_test_menu.h"

/***********************************************************************************
* TYPEDEFS
*/

// 测试数据包
typedef struct {
    uint32 seqNumber;
    uint8 padding[6];
} perTestPacket_t;

// 测试接收状态
typedef struct {
    uint32 expectedSeqNum;
    uint32 rcvdPkts;
    uint32 lostPkts;
    int16 rssiSum;
} perRxStats_t;

/***********************************************************************************
* CONSTANTS AND DEFINES
*/

// BasicRF definitions
#define PAN_ID                0x2167
#define TX_ADDR               0x2530
#define RX_ADDR               0xBEEF
#define MAX_PAYLOAD_LENGTH       103
#define PACKET_SIZE           sizeof(perTestPacket_t)

#define RSSI_AVG_WINDOW_SIZE   32  // 去32个RSSI值，然后计算平均值

#endif



