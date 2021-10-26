int SIC931_Check_Device() {
  byte buf = RE31_SPI.readReg(DEVICE_TYPE_REG);
  if (buf == 0x3B) {
    return 311; //RE31
  } 
  return -1;
}

void sic9310_CongifGain(uint8_t power){
  byte buf;
  power <<= 3;
  RE31_SPI.writeReg(GAIN_ST3, power);
}

//**********************************************************
//* Configure Registers for ISO14443A_106 Standard Operation
//**********************************************************
void SIC931_Config_14443a(){
//  byte buf;
  //tx session
  RE31_SPI.writeReg(TX_CONTROL_REG, 0x5b);
  RE31_SPI.writeReg(TX_CFG_CW_REG, 0x3f); //cw conductance  
  RE31_SPI.writeReg(CODER_CONTROL_REG, 0x19);
  RE31_SPI.writeReg(MOD_WIDTH_REG, 0x0f);//mod width
  RE31_SPI.writeReg(MOD_WIDTH_SOF_REG, 0x0f);  //mod width sof
  RE31_SPI.writeReg(CONTROL_REG, 0x00);
//  RE31_SPI.writeReg(TYPEB_FRAMING_REG, 0x3b);
  //rx session
  RE31_SPI.writeReg(RX_CONTROL1_REG, 0x6b);
  RE31_SPI.writeReg(DECODER_CONTROL_REG, 0x28);
  RE31_SPI.writeReg(BITPHASE_REG, 0x3d);  //set bitphase
  RE31_SPI.writeReg(RX_THRESHOLD_REG, 0x8c);  //set colllevel, minlevel
//  RE31_SPI.writeReg(BPSK_DEM_CONTROL_REG, 0x02);//set hp2off, taud(01), Agc_en(1), tauagc(0)
  RE31_SPI.writeReg(RX_CONTROL2_REG, 0x41);
  
  //RF-timming channel redundancy
  RE31_SPI.writeReg(RX_WAIT_REG, 0x07);//set rxwait
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x03);//crc, no parity, preset crc lsb = 0xff, crc msb = 0xff
  RE31_SPI.writeReg(CRC_PRESET_LSB_REG, 0x63);//set rxwait
  RE31_SPI.writeReg(CRC_PRESET_MSB_REG, 0x63);//set rxwait
  sic9310_CongifGain(0x03); // ** Gain of the last state amplifier
}

void rfOn(){
    byte buf = RE31_SPI.readReg(TX_CONTROL_REG);  
    buf |= 0x03;
    RE31_SPI.writeReg(TX_CONTROL_REG, buf);  
}

void rfOff(){
    byte buf = RE31_SPI.readReg(TX_CONTROL_REG);  
    buf &= 0xfc;
    RE31_SPI.writeReg(TX_CONTROL_REG, buf);  
}

//**********************************************************************
//* Function Name: Perform Request command in ISO14443A standard
//* Return
//**********************************************************************
//reqa
uint16_t reqa(){
  byte txbuf[1];
  byte rxbuf[2];
  uint8_t length;  
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x03);//crc, no parity, preset crc lsb = 0xff, crc msb = 0xff
  RE31_SPI.writeReg(BIT_FRAMING_REG, 7);
  txbuf[0] = 0x26;
  RE31_SPI.writeFIFO(txbuf, 1);  
  length = RE31_SPI.getFIFOLength();
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(1);
  length = RE31_SPI.getFIFOLength(); 
  if(length > 0) {
    RE31_SPI.readFIFO(rxbuf, length);
    return (rxbuf[1] << 8) | rxbuf[0];
  }else{
    return -1;
  } 
}

//**********************************************************************
//* Function Name: make a anti-collision transaction to 14443a tag, 
//           this function no implements yet to read multi-tag.
//* param: 
//  type, cascade
//* return: 0, success
//         -1, false
int antiColl14443A(uint8_t type){
  byte txbuf[7];
  byte rxbuf[5];
  uint8_t length;
  byte regbuf = RE31_SPI.readReg(CONTROL_REG);
  regbuf |= 1;
  RE31_SPI.writeReg(CONTROL_REG, regbuf);
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x03);//crc, no parity, preset crc lsb = 0xff, crc msb = 0xff
  if(type == 1){
    txbuf[0] = 0x93;
  }else if(type == 2) {
    txbuf[0] = 0x95;
  }
  txbuf[1] = 0x20;
  RE31_SPI.writeFIFO(txbuf, 2);
  length = RE31_SPI.getFIFOLength();  
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(3);
  length = RE31_SPI.getFIFOLength();  
  if(length == 5) {
    RE31_SPI.readFIFO(rxbuf, length);
    for(int i = 0; i< length; i++){
      uid_temp[i + ((type - 1) * 5)] = rxbuf[i];
    } 
    return 0;
  }     
  return -1;
}

//**********************************************************************     
//* make a select transaction to 14443a tag
//* param: type, cascade
//* return: 
//         sak, success
//         -1, false
uint8_t select14443A(uint8_t type){
  byte txbuf[7];
  byte rxbuf[1];
  uint8_t length;
  byte regbuf = RE31_SPI.readReg(CONTROL_REG);
  regbuf |= 1;
  RE31_SPI.writeReg(CONTROL_REG, regbuf);
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x0f);//crc, no parity, preset crc lsb = 0xff, crc msb = 0xff 
  if(type == 1){
    txbuf[0] = 0x93;
  }else if(type == 2){
    txbuf[0] = 0x95;
  }
  txbuf[1] = 0x70;
  for(int i = 0; i < 5; i++){
    txbuf[2 + i] = uid_temp[i + ((type - 1) * 5)];
  }
  RE31_SPI.writeFIFO(txbuf, 7);
  length = RE31_SPI.getFIFOLength();  
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(2);
  length = RE31_SPI.getFIFOLength();  
  if(length == 1) {
    RE31_SPI.readFIFO(rxbuf, length);
  if(length == 1){
    if(type == 1){
      mUidLength = 4;
    }else if(type ==2){
      mUidLength = 7;
    }
    return rxbuf[0];
  }
  return -1;
  }
}

//*********************************************************
// SIC4310 commands
//*********************************************************
uint8_t sic4310_trxru_command(byte* fifo, int len, uint16_t rspt){
  byte buf[64]; 
  byte controlReg = RE31_SPI.readReg(CONTROL_REG);
  controlReg |= 1;
  RE31_SPI.writeReg(CONTROL_REG, controlReg);//flush FIFO
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x0f);//tx crc, parity
  for(int i = 0; i< len; i++){
    buf[i+1] = fifo[i];
  }
  buf[0] = 0xb3;
  RE31_SPI.writeFIFO(buf, len+1);
  //delay(2);
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(rspt); //;
  len = RE31_SPI.getFIFOLength();
  RE31_SPI.readFIFO(fifo, len);
  Serial.print("...ack_len="); Serial.print(len, DEC);
  Serial.print(", rspt="); Serial.print(rspt, DEC); Serial.println("ms"); 
  if (len > 2) {
    //RE31_SPI.readFIFO(fifo, len);
    return len;
  }
  Serial.println("...RECEIVE_Timeout"); 
  return 0;
}

uint8_t sic4310_clear_flag(byte code, byte* fifo){
  int len; 
  byte buf[64];
  byte controlReg = RE31_SPI.readReg(CONTROL_REG);
  controlReg |= 1;
  RE31_SPI.writeReg(CONTROL_REG, controlReg);//flush FIFO
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x0f);//tx crc, parity
  buf[0] = 0xb4;
  buf[1] = code;
  RE31_SPI.writeFIFO(buf, 2);
  //delay(1); //prev = 2ms;
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(1); //prev = 10ms;
  len = RE31_SPI.getFIFOLength();
  //Serial.print("LENGTH="); Serial.println(len, DEC); 
  if(len > 0){
    RE31_SPI.readFIFO(fifo, len);
    return len;
  }
  return 0;
}

uint8_t sic4310_read_reg(byte addr, byte* fifo){
  int len; 
  byte buf[64];
  byte controlReg = RE31_SPI.readReg(CONTROL_REG);
  controlReg |= 1;
  RE31_SPI.writeReg(CONTROL_REG, controlReg);//flush FIFO
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x0f);//tx crc, parity
  buf[0] = 0xb5;
  buf[1] = addr;
  RE31_SPI.writeFIFO(buf, 2);
  delay(2);
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(10);
  len = RE31_SPI.getFIFOLength();
  //Serial.print("LENGTH="); Serial.println(len, DEC); 
  if(len > 0){
    RE31_SPI.readFIFO(fifo, len);
    return len;
  }
  return 0;
}

int sic4310_write_reg(byte addr, byte data, byte* fifo){
  int len; 
  byte buf[64];
  byte controlReg = RE31_SPI.readReg(CONTROL_REG);
  controlReg |= 1;
  RE31_SPI.writeReg(CONTROL_REG, controlReg);//flush FIFO
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x0f);//tx crc, parity
  buf[0] = 0xb6;
  buf[1] = addr;
  buf[2] = data;
  RE31_SPI.writeFIFO(buf, 3);
  delay(2);
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(10);
  len = RE31_SPI.getFIFOLength();
  //Serial.print("LENGTH="); Serial.println(len, DEC); 
  if(len > 0){
    RE31_SPI.readFIFO(fifo, len);
    return len;
  }
  return 0;
}

void clear_flag(uint8_t value){
  int len;
  len = sic4310_clear_flag(0xff, rxbuf); 
}

//*********************************************************
// T2 Tag commands
//*********************************************************
uint8_t t2_ReadPage(int page, byte* data){
  byte txbuf[2]; 
  byte rxbuf[16];
  uint8_t length;
  byte controlReg = RE31_SPI.readReg(CONTROL_REG);
  controlReg |= 1;
  RE31_SPI.writeReg(CONTROL_REG, controlReg);//flush FIFO
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x0f);//tx crc, parity
  txbuf[0] = 0x30; txbuf[1] = page;
  RE31_SPI.writeFIFO(txbuf, 2);
  delay(2);
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(8); //must for 43NT.verE
  length = RE31_SPI.getFIFOLength(); 
  if(length == 16){
    RE31_SPI.readFIFO(rxbuf, length);
    for(int i = 0; i < 16; i++){
      data[i] = rxbuf[i];
    }
    return 1;
  }
  return 0;
}

uint8_t t2_WritePage(int page, byte* data){
  byte txbuf[6]; 
  byte rxbuf[1];
  
  uint8_t length;
  byte controlReg = RE31_SPI.readReg(CONTROL_REG);
  controlReg |= 1;
  RE31_SPI.writeReg(CONTROL_REG, controlReg);//flush FIFO
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x07);//crc, parity
  txbuf[0] = 0xa2; txbuf[1] = page;
  for(int i = 0; i < 4; i++){
    txbuf[i + 2] = data[i];
  }
  RE31_SPI.writeFIFO(txbuf, 6);
  delay(2);  
      
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(30); //
  length = RE31_SPI.getFIFOLength();      

  if(length == 1){
    RE31_SPI.readFIFO(rxbuf, length);          
    if(rxbuf[0] == 0x0a){
      return 1;
    }
  }
  return 0;
}

//reading the tamper evidence in the SIC43NT’s hidden memory.
//param: int, byte*
//  tamper_data...2 bytes data
//return: 
//  1, success
//  0, false
uint8_t tps_ReadTamper(byte* tamper){
  byte txbuf[2]; 
  byte rxbuf[2];
  uint8_t len;
  byte controlReg = RE31_SPI.readReg(CONTROL_REG);
  controlReg |= 1;
  RE31_SPI.writeReg(CONTROL_REG, controlReg);//flush FIFO
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x0f);//tx crc, parity
  txbuf[0] = 0xAF; txbuf[1] = 0x00;
  RE31_SPI.writeFIFO(txbuf, 2);
  delay(2);
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(2);
  len = RE31_SPI.getFIFOLength();
  Serial.print("LENGTH="); Serial.print(len, DEC); 
  if(len == 2){
    RE31_SPI.readFIFO(rxbuf, len);
    tamper[0] = rxbuf[0];
    tamper[1] = rxbuf[1];
    Serial.print(" ,TAM="); Serial.print(tamper[0], HEX); 
    Serial.print(", ");Serial.println(tamper[1], HEX);
    return 1;
  } else {
    Serial.println("..FAIL");
    return 0;
  }
  return 0;
}

uint8_t tps_pwdAUTH(byte* pass){
  byte txbuf[5]; 
  byte rxbuf[2];
  uint8_t length;
  byte controlReg = RE31_SPI.readReg(CONTROL_REG);
  controlReg |= 1;
  RE31_SPI.writeReg(CONTROL_REG, controlReg);//flush FIFO
  RE31_SPI.writeReg(CHANNEL_REDUNDANCY_REG, 0x0f);//tx crc, parity
  txbuf[0] = 0x1B; 
  txbuf[1] = pass[0]; txbuf[2] = pass[1]; txbuf[3] = pass[2]; txbuf[4] = pass[3];
  RE31_SPI.writeFIFO(txbuf, 5);
  delay(2);
  RE31_SPI.writeCmd(TRANSCEIVE_CMD);
  delay(20);
  length = RE31_SPI.getFIFOLength();
  Serial.print("LENGTH="); Serial.println(length, DEC); 
  if(length >= 1){
    RE31_SPI.readFIFO(rxbuf, length);
    Serial.print("PACK=");
    Serial.print(rxbuf[0], HEX);Serial.println(rxbuf[1], HEX);
    return 1;
  } else {
    Serial.print("..FAIL");
    return 0;
  }
}

int getTagId(byte* id){
  if(mUidLength == 4){
    for(int i = 0; i < mUidLength; i++){
          id[i] = uid_temp[i];
    }
    return 4;
  }else if(mUidLength == 7){
    for(int i = 0; i < mUidLength; i++){
      if(i<3){
          id[i] = uid_temp[i+1];
      }else{
          id[i] = uid_temp[i+2];
      }
    }   
    return 7;
  }
  return 0;
}

//**********************************************************************
//* isTagPresent: detect tag in field ＊＊＊＊此迴圈內無法使用sprintf＊＊＊＊
//* return: boolean [true|false]
boolean isTagPresent()
{
  byte uidLength = 0;
  if(reqa() == 0x44){
    delay(1);
    for(int type = 0; type < 3; type ++){
      if(antiColl14443A(type + 1) != 0){
        break;
      }
      delay(1);
      int sak = select14443A(type + 1);
      if(sak == 0x00){
          uidLength = getTagId(uid);
          if (uidLength == 7) {
              return true; 
          } else {
              return false;    
          }
      }else if(sak == 0x04){
        continue;
      }
    }
  }  
  return false;
}

void rwdIdle(){
  RE31_SPI.writeCmd(IDLE_CMD);
}
