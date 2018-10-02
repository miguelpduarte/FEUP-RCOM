#ifndef _MESSAGE_DEFINES_
#define _MESSAGE_DEFINES_

#define BIT(n) (0x01<<(n))

//flag
#define MSG_FLAG                0x7E    /* Flag indicating message start and message end */
#define MSG_FLAG_START_IDX      0
#define MSG_FLAG_END_IDX        4

//address
#define MSG_ADDR_EMT    0x03    /* Command sent by the emitter and responses sent by the receiver */
#define MSG_ADDR_REC    0x01    /* Command sent by the receiver and responses sent by the emitter */
#define MSG_ADDR_IDX    1

//control
#define MSG_CTRL_SET    0x03                        /* Setup */
#define MSG_CTRL_DISC   0x0B                        /* Disconnect */
#define MSG_CTRL_UA     0x07                        /* Unnumbered Acknowledgement */
#define MSG_CTRL_RR(r)  0x05 | (r ? BIT(7) : 0x00)  /* Receiver Ready (positive ACK) */
#define MSG_CTRL_REJ(r) 0x01 | (r ? BIT(7) : 0x00)  /* Reject (negative ACK) */
#define MSG_CTRL_IDX    2

//bcc
#define MSG_BCC1(addr, ctrl)    ((addr) ^ (ctrl))   /* Message Parity */
#define MSG_BCC1_IDX    3

//message configurations
#define MSG_WAIT_TIME               30       /* Will wait 3 seconds */
#define MSG_MIN_CHARS               1        /* Will wait until received 1 char */
#define MSG_SUPERVISION_MSG_SIZE    5        /* Size of the supervision message */   
#define MSG_NUM_READ_TRIES          3        /* Number of times reading the response is tried. After these number of tries, abort */

//other
typedef unsigned char byte;

#endif /* _MESSAGE_DEFINES_ */