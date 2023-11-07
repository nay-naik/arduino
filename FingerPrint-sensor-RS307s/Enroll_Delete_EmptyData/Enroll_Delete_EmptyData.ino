#include <Adafruit_Fingerprint.h>
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 3);
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t operation;
uint8_t id;

void detectFingerprintScanner(){
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor detected!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }  
  Serial.println("");
  Serial.println("");
}

void verifyScannerParameters(){
  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  Serial.println("");
  Serial.println("");
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(100);
  Serial.println("Welcome to Security System");
  finger.begin(57600);
  detectFingerprintScanner();
  verifyScannerParameters();
}

uint8_t readUserInput(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

uint8_t getFingerprintEnroll() {
//Scan for First Time
int p = -1;
Serial.print("Waiting for valid finger to enroll as #"); 
Serial.println(id);
while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

// OK success!
p = finger.image2Tz(1);
switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
}
Serial.println("Remove finger");
delay(2000);
p = 0;
while (p != FINGERPRINT_NOFINGER) {p = finger.getImage();}
Serial.println("");
Serial.print("ID "); Serial.println(id);
p = -1;

//Scan for Second Time
Serial.println("Place same finger again");
while (p != FINGERPRINT_OK) {
  p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }
p = finger.image2Tz(2);
switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
Serial.println("");

//Creating Model 
Serial.print("Creating model for #");  Serial.println(id);
p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

//Storing Model
Serial.print("ID "); Serial.println(id);
p = finger.storeModel(id);
if (p == FINGERPRINT_OK) {
Serial.println("Congratulations! Fingerprint is successfully enrolled.");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

Serial.println("");
Serial.println("");
return true;
}

void enrollFingerprint(){
Serial.println("Ready to enroll a fingerprint!");
Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
id = readUserInput();
if (id == 0) {return;}
Serial.print("Enrolling ID #");
Serial.println(id);
while (! getFingerprintEnroll());
}


void deleteFingerprint(){
Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
uint8_t id = readUserInput();
if (id == 0) {return;}
Serial.print("Deleting ID #");
Serial.println(id);
uint8_t p = -1;
p = finger.deleteModel(id);
if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }
Serial.println("");
Serial.println("");
return p;
}

void deleteDatabase(){
Serial.println("Do you want to delete all fingerprint templates!");
Serial.println("Press 'Y' key to continue");
while (1) {
    if (Serial.available() && (Serial.read() == 'Y')) {
      break;
    }
  }
finger.emptyDatabase();
Serial.println("Now the database is empty.)");
Serial.println("");
Serial.println("");
}

void loop() {
Serial.println("Select an option: ");
Serial.println("Press 1 to enroll a fingerprint");
Serial.println("Press 2 to delete a fingerprint");
Serial.println("Press 3 to empty fingerprint database");
operation = readUserInput();
if (operation == 1){enrollFingerprint();}
else if (operation == 2){deleteFingerprint();}
else if (operation == 3){deleteDatabase();}
else return;
}