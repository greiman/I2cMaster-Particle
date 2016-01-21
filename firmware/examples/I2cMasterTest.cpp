// Some of my development tests with a DS1307.
#include "application.h"
#include "I2cMaster/I2cMaster.h"

const uint8_t DS1307_I2C_ADDRESS = 0X68;
I2cMaster I2C;

// Must use macro to avoid predefined Wire object.
WireMaster WireAlt;
#define Wire WireAlt

// Print fail message with return info.
void failMsg(const char* msg) {
  Serial.print(msg);
  Serial.print(", rtn: ");
  Serial.println(I2C.rtn());
}
//-----------------------------------------------------------------------------
void printData(uint8_t* data, uint8_t count) {
  for (int r = 0; r < count; r += 8) {
    if (r < 16) Serial.print('0');
    Serial.print(r, HEX);
    Serial.print("  ");
    for (int i = 0; i < 8 && (i+r) < count; i++) {
      Serial.print(' ');
      if (data[i+r] < 16) {
        Serial.print('0');
      }
      Serial.print(data[i+r], HEX);
    }
    Serial.println();
  }  
}
//-----------------------------------------------------------------------------
bool rtcRead(uint8_t memAdd, uint8_t* buf, size_t count) {
  return I2C.write(DS1307_I2C_ADDRESS, &memAdd, 1) &&
         I2C.read(DS1307_I2C_ADDRESS, buf, count);
}
//------------------------------------------------------------------------------
bool rtcReadWire(uint8_t memAdd, uint8_t* buf, size_t count) {
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(memAdd);
  Wire.endTransmission();
  if (Wire.requestFrom(DS1307_I2C_ADDRESS, count) != count) {
    return false;
  }
  for (size_t i = 0; i < count; i++) {
    buf[i] = Wire.read();
  }
  return true;
}
//-----------------------------------------------------------------------------
bool rtcWrite(uint8_t memAdd, uint8_t* buf, uint8_t count) {
  return I2C.write(DS1307_I2C_ADDRESS, &memAdd, 1, false) &&
         I2C.write(buf, count, true);
}
//-----------------------------------------------------------------------------
void clearRam() {
  uint8_t zero[56];
  memset(zero, 0, 56); 
  if (!I2C.begin()) { 
    failMsg("I2C.begin failed");
    return;
  }
  if (!rtcWrite(8, zero, 56)) { 
    failMsg("rtcWrite failed");
    return;
  }  
  Serial.println("Done"); 
  I2C.end();   
}
//----------------------------------------------------------------------------
void dumpAll() {
   uint8_t reg[64];

  if (!I2C.begin()) {   
    failMsg("I2C.begin failed");
    return;
  }
  if (!rtcRead(0, reg, 64)) {
    failMsg("rtcRead failed");
    return;
  }
  printData(reg, 64);
  I2C.end(); 
}
//-----------------------------------------------------------------------------
void setRam() {
  uint8_t memAdd = 8;
  if (!I2C.begin()) {
    Serial.println(I2C.rtn());    
    failMsg("I2C.begin failed");
    return;
  }
  if (!I2C.write(DS1307_I2C_ADDRESS, &memAdd, 1, false)) {
    failMsg("write failed");
  }
  for (uint8_t i = 8; i < 64; i++) {
    if (!I2C.write(i, false)) {
      failMsg("write byte failed");      
    }
  }
  I2C.stop();
  Serial.println("Done");  
}
//-----------------------------------------------------------------------------
void scanBus() {
  I2C.begin(100000);
  uint8_t tmp;
  uint8_t add = 0;
  bool found = false;
  do {
    bool wr = I2C.write(add, &tmp, 0);
    bool rd = I2C.read(add, &tmp, 1);
    
    if (rd || wr) {
      found = true;
      Serial.print("Device at address: 0X");
      Serial.print(add, HEX);
      Serial.print(" responds to ");
      if (rd) Serial.print("Read");
      if (rd && wr) Serial.print(" and ");
      if (wr) Serial.print("Write");
      Serial.println('.');
    }
    add += 1;
  } while (add < 0X80);

  if (!found) {
    Serial.println("No devices found.");
  }
  Serial.println("Done"); 
  I2C.end();  
}
//-----------------------------------------------------------------------------
void testWire(uint8_t n) {
  uint8_t reg[64];

  Wire.begin();

  if (!rtcReadWire(0, reg, n)) {
    failMsg("rtcReadWire failed");
    return;
  }
  printData(reg, n);
  Wire.end();
}
//-----------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  while (!Serial.available()) {
    Serial.println("Type any character");
    for (int i = 0; !Serial.available() && i < 20; i++) {
      delay(100);
    }    
  }
}
//-----------------------------------------------------------------------------
void loop() {
  int c;
  do {delay(10);} while (Serial.read() >= 0);
  Serial.println("Type '1' scan bus, '2' dump all, '3' setRam");
  Serial.println("     '4' clearRam, '5' testWire");
  while ((c = Serial.read()) < 0);
  switch (c) {
    case '1':
      scanBus();
      break;
    
    case '2':
      dumpAll();
      break;
      
    case '3':
      setRam();
      break;
      
    case '4':
      clearRam();
      break;
      
    case '5':
      testWire(32);
      break;
      
    default:
      Serial.println("Invalid selection");
  }
}