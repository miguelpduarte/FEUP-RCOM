#ifndef _MESSAGE_DEFINES_
#define _MESSAGE_DEFINES_

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
#define MSG_CTRL_SET    0x03                        /* Setup */
#define MSG_CTRL_DISC   0x0B                        /* Disconnect */
#define MSG_CTRL_UA     0x07                        /* Unnumbered Acknowledgement */
#define MSG_CTRL_RR(r)  0x05 | (r ? BIT(7) : 0x00)  /* Receiver Ready (positive ACK) */
#define MSG_CTRL_REJ(r) 0x01 | (r ? BIT(7) : 0x00)  /* Reject (negative ACK) */
#define MSG_CTRL_IDX    2
#define MSG_CTRL_S(s)   (((s) % 2) << 6)            /* Message number to S control byte */

//bcc
#define MSG_BCC1(addr, ctrl)        ((addr) ^ (ctrl))   /* Message Parity */
#define MSG_BCC1_IDX                3
#define MSG_BCC2_IDX(data_size)      ((data_size) + 4)

//information
#define MSG_DATA_BASE_IDX           4

//message configurations
#define MSG_WAIT_TIME               30       /* Will wait 3 seconds */
#define MSG_MIN_CHARS               0        /* If MIN = 0 and TIME > 0, TIME serves as a timeout value. The read will be satisfied 
                                                    if a single character is read, or TIME is exceeded (t = TIME *0.1 s). If TIME is 
                                                    exceeded, no character will be returned */
#define MSG_NUM_READ_TRIES              3        /* Number of times reading the response is tried. After these number of tries, abort */
#define MSG_SUPERVISION_MSG_SIZE        5        /* Size of the supervision message */   
#define MSG_INFO_MSG_SIZE(data_size)    ((data_size) + 6)        /* Size of the information message */

//other
typedef unsigned char byte;
#define DATA_SIZE_FROM_INFO_MSG_LENGTH(msg_length)      ((msg_length) - 6)

#endif /* _MESSAGE_DEFINES_ */