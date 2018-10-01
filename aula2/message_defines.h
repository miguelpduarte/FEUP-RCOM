#ifndef _MESSAGE_DEFINES_
#define _MESSAGE_DEFINES_

#define BIT(n) (0x01<<(n))

//flag
#define MSG_FLAG                0x7E
#define MSG_FLAG_START_IDX      0
#define MSG_FLAG_END_IDX        4

//address
#define MSG_ADDR_EMT    0x03
#define MSG_ADDR_REC    0x01
#define MSG_ADDR_IDX    1

//control
#define MSG_CTRL_SET    0x03
#define MSG_CTRL_DISC   0x0B
#define MSG_CTRL_UA     0x07
#define MSG_CTRL_RR(r)  0x05 | (r ? BIT(7) : 0x00)
#define MSG_CTRL_REJ(r) 0x01 | (r ? BIT(7) : 0x00)
#define MSG_CTRL_IDX    2

//bcc
#define MSG_BCC1(addr, ctrl)    ((addr) ^ (ctrl))
#define MSG_BCC1_IDX    3

//message configurations
#define MSG_WAIT_TIME               30       /* Will wait 3 seconds */
#define MSG_MIN_CHARS               1        /* Will wait until received 1 char */
#define MSG_SUPERVISION_MSG_SIZE    5        /* Size of the supervision message */   

//other
typedef unsigned char byte;

#endif /* _MESSAGE_DEFINES_ */