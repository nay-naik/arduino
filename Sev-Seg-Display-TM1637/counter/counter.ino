#include <TM1637Display.h>

#define CLK 3   
#define DIO 2  

TM1637Display display(CLK, DIO);

void setup() {
  
  display.setBrightness(5); //(7 max - 0 min -> brightness)
}

uint16_t i = 0;

void loop() {

  // print variable 'i' on the display (decimal format with leading zeros)
  display.showNumberDec(i, true);

  i++;           
  if(i > 9999)
    i = 0;

  delay(1000);

}