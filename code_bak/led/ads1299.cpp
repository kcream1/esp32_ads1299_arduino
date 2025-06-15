#include "ads1299.h"


void WREG(byte _address, byte _value,int CS) {
    byte opcode1 = _WREG + _address; //001rrrrr; _RREG = 00100000 and _address = rrrrr
    digitalWrite(CS, LOW); //Low to communicated
    SPI.transfer(_SDATAC); //SDATAC
    SPI.transfer(opcode1);
    SPI.transfer(0x00);
    SPI.transfer(_value);
    SPI.transfer(_RDATAC);
    digitalWrite(CS, HIGH); //Low to communicated
    // Serial.print("Register 0x");
    // Serial.print(_address, HEX);
    // Serial.println(" modified.");
}

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
    
}


