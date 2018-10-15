#ifndef _MESSAGE_DEFINES_
#define _MESSAGE_DEFINES_

#include <stdlib.h>

#define BIT(n) (0x01<<(n))

//flag
#define MSG_FLAG                            0x7E    /* Flag indicating message start and message end */
#define MSG_FLAG_START_IDX                  0
#define MSG_FLAG_END_IDX                    4
#define MSG_INFO_FLAG_END_IDX(data_size)    ((data_size) + 5)


//address
#define MSG_ADDR_EMT    0x03    /* Command sent by the emitter and responses sent by the receiver */
#define MSG_ADDR_REC    0x01    /* Command sent by the receiver and responses sent by the emitter */
#define MSG_ADDR_IDX    1


//control
#define MSG_CTRL_SET            0x03                        /* Setup */
#define MSG_CTRL_DISC           0x0B                        /* Disconnect */
#define MSG_CTRL_UA             0x07                        /* Unnumbered Acknowledgement */
#define MSG_CTRL_RR(r)          0x05 | (r ? BIT(7) : 0x00)  /* Receiver Ready (positive ACK) */
#define MSG_CTRL_RR_0           0x05
#define MSG_CTRL_RR_1           0x85
#define MSG_CTRL_RR_DECODE(b)   ((b) >> 7);
#define MSG_CTRL_REJ(r)         0x01 | (r ? BIT(7) : 0x00)  /* Reject (negative ACK) */
#define MSG_CTRL_REJ_0          0x01
#define MSG_CTRL_REJ_1          0x81
#define MSG_CTRL_REJ_DECODE(b)  ((b) >> 7);
#define MSG_CTRL_IDX            2
#define MSG_CTRL_S(s)           (((s) % 2) << 6)            /* Message number to S control byte */
#define MSG_CTRL_S0             0
#define MSG_CTRL_S1             0x40


//bcc
#define MSG_BCC1(addr, ctrl)        ((addr) ^ (ctrl))   /* Message Parity */
#define MSG_BCC1_IDX                3
#define MSG_BCC2_IDX(data_size)      ((data_size) + 4)


//information
#define MSG_DATA_BASE_IDX           4


//message stuffing
#define MSG_ESCAPE_BYTE             0x7d
#define MSG_FLAG_STUFFING_BYTE      0x5e    /* result of MSG_FLAG XOR 0x20 = 0x7E XOR 0x20 */
#define MSG_ESCAPE_STUFFING_BYTE    0x5d    /* result of MSG_ESCAPE_BYTE XOR 0x20 = 0x7D XOR 0x20 */

//message configurations
#define MSG_WAIT_TIME               30       /* In tens of seconds - Wait time in seconds = 0.1*T   */
#define MSG_MIN_CHARS               0        /* If MIN = 0 and TIME > 0, TIME serves as a timeout value. The read will be satisfied 
                                                    if a single character is read, or TIME is exceeded (t = TIME *0.1 s). If TIME is 
                                                    exceeded, no character will be returned */
#define MSG_NUM_READ_TRIES              3                   /* Number of times reading the response is tried. After these number of tries, abort */
#define MSG_NUM_RESEND_TRIES            3                   /* Number of times trying to send a frame. After these number of tries, abort */
#define MSG_SUPERVISION_MSG_SIZE        5                   /* Size of the supervision message */   
#define MSG_INFO_MSG_SIZE(data_size)    ((data_size) + 6)   /* Size of the information message */
#define MSG_PART_MAX_SIZE               4096
#define MSG_STUFFING_BUFFER_SIZE        8192        /* MSG_MAX_SIZE*2  */
#define MSG_INFO_RECEIVER_BUFFER_SIZE   8193        /* MSG_STUFFING_BUFFER_SIZE + 1 */


//other
#define DATA_SIZE_FROM_INFO_MSG_LENGTH(msg_length)      ((msg_length) - 6)


#endif /* _MESSAGE_DEFINES_ */