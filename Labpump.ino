/*
 * Project Labpump
 * Description: Control of 12VDC Pump with Particle Boron and Nextion Display
 * Author:  Patrick Bolton
 * Begin Date:  4/18/21
 * Rev Date:    4/18/21
 */

#include <Nextion.h>

USARTSerial& nexSerial = Serial1;

#define DEBUG_SERIAL_ENABLE
#define dbSerial Serial

int pmp = A0;

char buffer[100] = {0};

NexButton b0 = NexButton(0, 6, "b0");
NexDSButton bt0 = NexDSButton(0, 1, "bt0");
NexNumber n0 = NexNumber(0, 7, "n0");
NexNumber n1 = NexNumber(0, 8, "n1");
NexNumber n2 = NexNumber(0, 9, "n2");

NexTouch *nex_listen_list[] = 
{
    &b0,
    &bt0,
    &n0,
    &n1,
    &n2,
    NULL
};

void b0PushCallback(void *ptr)  // read value from n0 and set to n1
{
  dbSerial.println("b0PushCallback");

  uint32_t num0 = 0;

  n0.getValue(&num0);
  
  n1.setValue(num0);
  
  dbSerial.printlnf("Pump Set Point Selected: %d", num0);
  num0 = 0;
}

void bt0PushCallback(void *ptr)  
{
  uint32_t dual_state = 0;
  uint32_t num1 = 0;

  dbSerial.println("bt0PushCallback");

  bt0.getValue(&dual_state);

  n1.getValue(&num1);

  if(dual_state==1)  // read value from n1 and set to n2
  {
    dbSerial.println("DS ON");
    
    n2.setValue(num1);

    float pmpsp_f = 255.0f * float(num1) / 100.f;

    int pmpsp_i =  int(pmpsp_f);

    analogWrite(A0, pmpsp_i);

    dbSerial.printlnf("Pump Set Point: %d", num1);
    dbSerial.printlnf("Pump Speed: %d", pmpsp_i);
  }
  else
  {
    dbSerial.println("DS OFF");

    n2.setValue(0);
    
    analogWrite(A0, 0);
    dbSerial.printlnf("Pump Set Point: %d", num1);
    dbSerial.printlnf("Pump Speed: %d", 0);
  }
}

void bt0PopCallback(void *ptr)  
{
  uint32_t dual_state = 0;
  uint32_t num1 = 0;

  dbSerial.println("bt0PopCallback");

  bt0.getValue(&dual_state);

  n1.getValue(&num1);

  if(dual_state==0)  // read value from n1 and set to n2
  {
    dbSerial.println("DS OFF");

    n2.setValue(0);

    analogWrite(A0, 0);
    dbSerial.printlnf("Pump Set Point: %d", num1);
    dbSerial.printlnf("Pump Speed: %d", 0);
  }
  else
  {
    dbSerial.println("DS ON");
    
    n2.setValue(num1);

    float pmpsp_f = 255.0f * float(num1) / 100.f;

    int pmpsp_i =  int(pmpsp_f);

    analogWrite(A0, pmpsp_i);

    dbSerial.println("n2");
    dbSerial.println(num1);
    dbSerial.println("pmp");
    dbSerial.println(pmpsp_i);
  }
}



void setup() 
{
  pinMode(A0, OUTPUT);
  
  nexInit();

  delay(100);

  b0.attachPush(b0PushCallback, &b0);
  bt0.attachPush(bt0PushCallback, &bt0);
  bt0.attachPop(bt0PopCallback, &bt0);

  dbSerial.println("Touch Listens Set Up");   
}

void loop() 
{
   nexLoop(nex_listen_list);
}