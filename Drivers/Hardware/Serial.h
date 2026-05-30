#ifndef __SERIAL_H
#define __SERIAL_H

void Serial_Init(void);
void Serial_RXCallBack(uint8_t byte);
void Serial_ProcessCmd(void);

#endif
