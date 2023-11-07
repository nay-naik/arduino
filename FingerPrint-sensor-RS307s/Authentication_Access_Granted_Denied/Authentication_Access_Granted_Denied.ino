#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 3);
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
Serial.begin(9600);
pinMode(13, OUTPUT);
while (!Serial);  
delay(100);
finger.begin(57600);
delay(5);
if (finger.verifyPassword()) {
  Serial.println("Found fingerprint sensor!");
} 
else {
  Serial.println("Did not find fingerprint sensor :(");
  while (1) { delay(1); }
}
}

void loop() {
uint8_t p = finger.getImage();
if (p == FINGERPRINT_NOFINGER)  goto NoFinger; 
else if (p == FINGERPRINT_OK) {
  p = finger.image2Tz();
  
  if (p != FINGERPRINT_OK)  goto NoMatch;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  goto NoMatch;
  Serial.print("Welcome! Your ID is ");
  Serial.print(finger.fingerID);
  Serial.println(". ACCESS GRANTED!.");
  if (finger.fingerID == 2)
  digitalWrite(13,HIGH);
  if (finger.fingerID == 1)
  digitalWrite(13,LOW);
  delay(1500);
  return; 
}
NoMatch: 
{
Serial.println("ACCESS DENIED"); 
delay(1500); 
return;
} 
NoFinger: 
{
Serial.println("No finger detected");
delay(1500);
} 
}