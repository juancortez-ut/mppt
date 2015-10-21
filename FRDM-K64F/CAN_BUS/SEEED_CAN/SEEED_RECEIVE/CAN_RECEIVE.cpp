/*************************** CAN_RECEIVE.cpp ***************************************************
* Maximum Power Point Tracker Project for EE 464R
* 
* CAN_RECEIVE Program - This program is loaded onto one of the FRDM boards with the CAN-BUS Shield
* attached. This program receives data via CAN-BUS. The results of the program running is located
* in the SEEED_RECEIVE.png file.
* 
* Author: Juan Cortez
* Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
* Created on: October 20, 2015 at 16:05
* Revised on: October 20, 2015 at 18:01
* 
* Serial Communication on MAC
* 
* $ cd /dev && screen `ls | grep tty.usbmodem`
*
* Example found in: // http://langster1980.blogspot.com/2014/10/mbed-can-bus-tutorial.html
****************************************************************************************/


#include "mbed.h"
#include "seeed_can.h"

void CAN_Interrupt_Received(void);

SEEED_CAN can(SEEED_CAN_CS,SEEED_CAN_IRQ, SEEED_CAN_MOSI, SEEED_CAN_MISO, SEEED_CAN_CLK , 500000);
SEEED_CANMessage msg; // create empty CAN message
Serial pc(USBTX, USBRX);                                  

DigitalOut led1(LED1);
DigitalOut led2(LED2);


int main() {
    printf("SEEED_RECEIVE Program Starting...\r\n");
    
    int can_open_status = can.open(500000, SEEED_CAN::Normal);  // initialize CAN-BUS Shield
    if(can_open_status){
        printf("CAN Bus Shield successfully initialized!\r\n");    
    } else{
        printf("CAN BUS Shield initialization failed...\r\n");    
    }
    
    
    can.mask(0, 0x1FFFFFFF); // Configure Mask 0 to check all bits of a Standard CAN message Id
    can.mask(1, 0x1FFFFFFF, CANStandard); // Configure Mask 1 to check all bits of a Standard CAN message Id
    can.filter(0, 7);  // ONLY ACCEPTS 0X09
    can.attach(CAN_Interrupt_Received, SEEED_CAN::AnyIrq); // when an interrupt is triggered, it will call CAN_Interrupt_Received
    
  while(1) {
    led1 = !led1; // RED heartbeat to make sure that the program is running
    wait(1);
  }
}

// 
void CAN_Interrupt_Received(void){
    int counter = 0;
    if(can.read(msg)) {  // if message is available, read into msg
      printf("Message received!\r\n");
      printf("The id is: %d.\r\n", msg.id);
      printf("The length of the message is: %d.\r\n", msg.len);
      for(counter = 0; counter < msg.len; counter++){
        printf("msg[%d] = %c\r\n", counter, msg.data[counter]);
      }
      led2 = !led2; // Yellow toggle receive status LED
    }    
}