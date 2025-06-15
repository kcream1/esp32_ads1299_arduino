
#include "ads1299.h"
#include "pins_arduino.h"


void setup(int DRDY, int CS,int outputCount){
    
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SS, OUTPUT);
  
  digitalWrite(SCK, LOW);
  digitalWrite(MOSI, LOW);
  digitalWrite(MISO, LOW);
  digitalWrite(SS, HIGH);
  
  //SPISettings settings(1000000, MSBFIRST, SPI_MODE1);


  pinMode(DRDY, INPUT);
  pinMode(CS, OUTPUT);
  
  //tCLKs = 0.000666; //666 ns (Datasheet, pg. 8)
  outputCount = 0;
}

//System Commands
void WAKEUP(int CS,float tCLKs) {
    digitalWrite(CS, LOW); //Low to communicate
    SPI.transfer(_WAKEUP);
    digitalWrite(CS, HIGH); //High to end communication
    delay(4*tCLKs);  //must way at least 4 tCLK cycles before sending another command (Datasheet, pg. 35)
}
void STANDBY(int CS) {
    digitalWrite(CS, LOW);
    SPI.transfer(_STANDBY);
    digitalWrite(CS, HIGH);
}
void RESET(int CS) {
    digitalWrite(CS, LOW);
    SPI.transfer(_RESET);
    delay(10);
//    delay(18.0*tCLK); //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
    digitalWrite(CS, HIGH);
}
void START(int CS) {
    digitalWrite(CS, LOW);
    SPI.transfer(_START);
    digitalWrite(CS, HIGH);
}
void STOP(int CS) {
    digitalWrite(CS, LOW);
    SPI.transfer(_STOP);
    digitalWrite(CS, HIGH);
}
//Data Read Commands
void RDATAC(int CS) {
    digitalWrite(CS, LOW);
    SPI.transfer(_RDATAC);
    digitalWrite(CS, HIGH);
}
void SDATAC(int CS) {
    digitalWrite(CS, LOW);
    SPI.transfer(_SDATAC);
    digitalWrite(CS, HIGH);
}
void RDATA(int CS) {
    digitalWrite(CS, LOW);
    SPI.transfer(_RDATA);
    digitalWrite(CS, HIGH);
}

//Register Read/Write Commands
void getDeviceID(int CS) {
    digitalWrite(CS, LOW); //Low to communicated
    SPI.transfer(_SDATAC); //SDATAC
    SPI.transfer(_RREG); //RREG
    SPI.transfer(0x00); //Asking for 1 byte
    byte data = SPI.transfer(0x00); // byte to read (hopefully 0b???11110)
    SPI.transfer(_RDATAC); //turn read data continuous back on
    digitalWrite(CS, HIGH); //Low to communicated
    Serial.println("getDeviceID:");
    Serial.println(data, BIN);
}

//读取单个寄存器的数值
void RREG(byte _address,int CS) {
    byte opcode1 = _RREG + _address; //001rrrrr; _RREG = 00100000 and _address = rrrrr
    digitalWrite(CS, LOW); //Low to communicated
    SPI.transfer(_SDATAC); //SDATAC
    SPI.transfer(opcode1); //RREG
    SPI.transfer(0x00); //opcode2
    byte data = SPI.transfer(0x00); // returned byte should match default of register map unless edited manually (Datasheet, pg.39)
    printRegisterName(_address);
    Serial.print("0x");
    if(_address<16) Serial.print("0");
    Serial.print(_address,HEX);
    Serial.print(", ");
    Serial.print("0x");
    if(data<16) Serial.print("0");
    Serial.print(data,HEX);
    Serial.print(", ");
    for(byte j = 0; j<8; j++){
        Serial.print(bitRead(data, 7-j), BIN);
        if(j!=7) Serial.print(", ");
    }
    SPI.transfer(_RDATAC); //turn read data continuous back on
    digitalWrite(CS, HIGH); //High to end communication
    Serial.println();
}

//循环读取寄存器里面的数值
void RREGS(byte _address, byte _numRegistersMinusOne,int CS) {
    byte opcode1 = _RREG + _address; //001rrrrr; _RREG = 00100000 and _address = rrrrr
    digitalWrite(CS, LOW); //Low to communicated
    SPI.transfer(_SDATAC); //SDATAC
    SPI.transfer(opcode1); //RREG
    SPI.transfer(_numRegistersMinusOne); //opcode2
    for(byte i = 0; i <= _numRegistersMinusOne; i++){
        byte data = SPI.transfer(0x00); // returned byte should match default of register map unless previously edited manually (Datasheet, pg.39)
        printRegisterName(i);
        Serial.print("0x");
        if(i<16) Serial.print("0"); //lead with 0 if value is between 0x00-0x0F to ensure 2 digit format
        Serial.print(i, HEX);
        Serial.print(", ");
        Serial.print("0x");
        if(data<16) Serial.print("0"); //lead with 0 if value is between 0x00-0x0F to ensure 2 digit format
        Serial.print(data, HEX);
        Serial.print(", ");
        for(byte j = 0; j<8; j++){
            Serial.print(bitRead(data, 7-j), BIN);
            if(j!=7) Serial.print(", ");
        }
        Serial.println();
    }
    SPI.transfer(_RDATAC); //turn read data continuous back on
    digitalWrite(CS, HIGH); //High to end communication
}

void WREG(byte _address, byte _value,int CS) {
    byte opcode1 = _WREG + _address; //001rrrrr; _RREG = 00100000 and _address = rrrrr
    digitalWrite(CS, LOW); //Low to communicated
    SPI.transfer(_SDATAC); //SDATAC
    SPI.transfer(opcode1);
    SPI.transfer(0x00);
    SPI.transfer(_value);
    SPI.transfer(_RDATAC);
    digitalWrite(CS, HIGH); //Low to communicated
    Serial.print("Register 0x");
    Serial.print(_address, HEX);
    Serial.println(" modified.");
}
//
//void ADS1299::WREG(byte _address, byte _value, byte _numRegistersMinusOne) {
//    
//}

void updateData(int DRDY,int CS,int outputCount){
    Serial.print("DRDY........");
    if(digitalRead(DRDY) == LOW){
        Serial.print("DRDYLOW");
        digitalWrite(CS, LOW);
//        long output[100][9];
        long output[9];
        long dataPacket;
        for(int i = 0; i<9; i++){
            for(int j = 0; j<3; j++){
                byte dataByte = SPI.transfer(0x00);
                dataPacket = (dataPacket<<8) | dataByte;
            }
//            output[outputCount][i] = dataPacket;
            output[i] = dataPacket;
            dataPacket = 0;
        }
        digitalWrite(CS, HIGH);
        Serial.print(outputCount);
        Serial.print(", ");
        for (int i=0;i<9; i++) {
            Serial.print(output[i], HEX);
            if(i!=8) Serial.print(", ");
            
        }
        Serial.println();
        outputCount++;
    }
}

// String-Byte converters for RREG and WREG
void printRegisterName(byte _address) {
    if(_address == ID){
        Serial.print("ID,");
    }
    else if(_address == CONFIG1){
        Serial.print("CONFIG1, ");
    }
    else if(_address == CONFIG2){
        Serial.print("CONFIG2, ");
    }
    else if(_address == CONFIG3){
        Serial.print("CONFIG3, ");
    }
    else if(_address == LOFF){
        Serial.print("LOFF, ");
    }
    else if(_address == CH1SET){
        Serial.print("CH1SET, ");
    }
    else if(_address == CH2SET){
        Serial.print("CH2SET, ");
    }
    else if(_address == CH3SET){
        Serial.print("CH3SET, ");
    }
    else if(_address == CH4SET){
        Serial.print("CH4SET, ");
    }
    else if(_address == CH5SET){
        Serial.print("CH5SET, ");
    }
    else if(_address == CH6SET){
        Serial.print("CH6SET, ");
    }
    else if(_address == CH7SET){
        Serial.print("CH7SET, ");
    }
    else if(_address == CH8SET){
        Serial.print("CH8SET, ");
    }
    else if(_address == BIAS_SENSP){
        Serial.print("BIAS_SENSP, ");
    }
    else if(_address == BIAS_SENSN){
        Serial.print("BIAS_SENSN, ");
    }
    else if(_address == LOFF_SENSP){
        Serial.print("LOFF_SENSP, ");
    }
    else if(_address == LOFF_SENSN){
        Serial.print("LOFF_SENSN, ");
    }
    else if(_address == LOFF_FLIP){
        Serial.print("LOFF_FLIP, ");
    }
    else if(_address == LOFF_STATP){
        Serial.print("LOFF_STATP, ");
    }
    else if(_address == LOFF_STATN){
        Serial.print("LOFF_STATN, ");
    }
    else if(_address == GPIO){
        Serial.print("GPIO, ");
    }
    else if(_address == MISC1){
        Serial.print("MISC1, ");
    }
    else if(_address == MISC2){
        Serial.print("MISC2, ");
    }
    else if(_address == CONFIG4){
        Serial.print("CONFIG4, ");
    }
}

// //SPI communication methods
// byte ADS1299::transfer(byte _data) {
//   SPI.transfer(_data);
//   //   SPDR = _data;
//   //   while (!(SPSR & _BV(SPIF)))
//   //       ;
//   //   return SPDR;
// }




