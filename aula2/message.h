#ifndef _MESSAGE_
#define _MESSAGE_

#define BIT(n) (0x01<<(n))

//flag
#define MSG_FLAG 0x7E

//address
#define MSG_ADDR_EMT 0x03
#define MSG_ADDR_REC 0x01

//control
#define MSG_CTRL_SET    0x03
#define MSG_CTRL_DISC   0x0B
#define MSG_CTRL_UA     0x07
#define MSG_CTRL_RR(r)  0x05 | (r ? BIT(7) : 0x00)
#define MSG_CTRL_REJ(r) 0x01 | (r ? BIT(7) : 0x00)

//bcc
#define MSG_BCC1(addr, ctrl)    ((addr) ^ (ctrl))

//other
#define MSG_WAIT_TIME   30 /* Will wait 3 seconds */
#define MSG_MIN_CHARS   1  /* Will wait until received 1 char */

#endif /* _MESSAGE_ */