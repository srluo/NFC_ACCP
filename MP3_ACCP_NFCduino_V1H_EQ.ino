/*****************************************************************
 * * Copyright (c) Washow Corp. All rights reserved.
 * This program is designed with Seeeduino XIAO to demo
 * NFC MP3player V1 by using SICRE31 RFID Reader 
 * To detect interactive things and play program
 * code written by Roger Luo @ Washow Corp, 2019/JAN/10 Created. 2021/OCT/17 Modified.
 * ----------------Folder/Track List---------------------------------------------
 * /01/001-255 Reserved for system use
 * /91/001 ~ /99/255 Reserved for ACCP Demo
 *****************************************************************/
#include <stdio.h>
#include <Arduino.h>
#include "LowPower.h"
#include <SPI.h>
#include <PCF8563.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_I2C
  #include <Wire.h>
#endif
#include "DFRobotDFPlayerMini.h"

#define NAME         "Acoustic Companion"
#define FW_VERSION   "ACCP NFCduino v1.H"

int numTrack = 1;
int currTrack = 1; 
int numFolder = 0;
int maxTrack = 0;
uint8_t startTrack;
uint8_t sleepTimer = 0;
uint8_t timerMin = 0;
uint8_t timerHour = 0;
uint8_t langType = 0;
uint8_t currListAddr;
uint8_t startListAddr;
uint8_t countList;
uint8_t offsetList;
uint8_t listFolder;
uint8_t listTrack;
int hpGame = 0;
int wpGame = 0;
int vol = 10;
int err = 1;
int rsp_value;
bool f_loop = false;
bool f_single = false;
bool f_nowPlaying = false;
bool f_newTag = false;
bool f_validRTC = true;
bool f_gamePlaying = false;
bool f_eqOn = false;
bool f_tagPGMR = false;

#define DEBUG
//******* Registers of SICRE31/RE41*******************
#define COMMAND_REG 0x01
#define FIFO_DATA_REG 0x02
#define PRIMARY_STATUS_REG 0x03
#define FIFO_LENGTH_REG 0x04
#define SECONDARY_STATUS_REG 0x05
#define INTERRUPT_ENABLE_REG 0x06
#define INTERRUPT_FLAG_REG 0x07
#define CONTROL_REG 0x09
#define ERROR_FLAG_REG 0x0a
#define COLL_POS_REG 0x0b
#define TIMER_VALUE_REG 0x0c
#define CRC_RESULT_LSB_REG 0x0d
#define CRC_RESULT_MSB_REG 0x0e
#define BIT_FRAMING_REG 0x0f
#define TX_CONTROL_REG 0x11
#define TX_CFG_CW_REG 0x12
#define TX_CFG_MOD_REG 0X13
#define CODER_CONTROL_REG 0X14
#define MOD_WIDTH_REG 0x15
#define MOD_WIDTH_SOF_REG 0x16
#define TYPEB_FRAMING_REG 0x17
#define RX_CONTROL1_REG 0x19
#define DECODER_CONTROL_REG 0x1a
#define BITPHASE_REG 0X1b
#define RX_THRESHOLD_REG 0X1c
#define BPSK_DEM_CONTROL_REG 0x1d
#define RX_CONTROL2_REG 0x1e
#define RX_CONTROL3_REG 0x1f
#define RX_WAIT_REG 0x21
#define CHANNEL_REDUNDANCY_REG 0x22
#define CRC_PRESET_LSB_REG 0x23
#define CRC_PRESET_MSB_REG 0x24
#define TIMER_CLOCK_REG 0x2a
#define TIMER_RELOAD_VALUE_REG 0x2c
#define DEVICE_TYPE_REG 0x38
#define ANALOG_ADJUST1_REG 0x3e
#define GAIN_ST3_REG 0x3f
#define ANALOG_ADJUST3_REG 0x3c
#define GAIN_ST3 0x3f

//******* RE31/RE41 command ***************************************
#define IDLE_CMD 0x00
#define TRANSMIT_CMD 0x1a
#define RECEVIE_CMD 0x16
#define TRANSCEIVE_CMD 0x1e
#define WIRTE_EEPROM_CMD 0x01
#define READ_EEPROM_CMD 0x03
#define LOAD_CONFIG_E2_CMD 0x07
#define CALCULATE_CRC_CMD 0x12
#define LOAD_KEY_EEPROM_CMD 0x0b
#define LOAD_KEY_FIFO_CMD 0x19
#define AUTHENT_CMD 0x1c
#define TUNE_FILTER_CMD 0x10

//------Board Setting
//Seeeduino XIAO
#define ADC_REF 3.3 //reference voltage of ADC is 3v.
#define BUZZER  3
#define SS      2
#define LED_B   PIN_LED_TXL
#define LED_G   PIN_LED_RXL
#define LED_R   LED_BUILTIN

//low voltage limit
#define LOW_VOLTAGE_THRESHOLD1 30
#define LOW_VOLTAGE_THRESHOLD2 10

//operational/display state
#define SLEEP  0
#define SCAN  1
#define PLAYER 2
#define GAMER 3
#define CONFIG 4
#define NG_ERR 5
#define TRK 6
#define TIME 7
#define LLB 8
#define NOP 9
#define LST 10
#define TIMER 11
#define OFF 12

//Language 
#define TW 0
#define EN 1
#define JP 2

//Demo folder 
#define GFOLDER 93 //0x5D

String ss = "";
String st = "";
String sr = "";

//------Global Variables
byte uid[8] = {0};
byte validUID[8] = {0};
byte pass[4] = {0};
byte pack[4] = {0x55, 0xAA, 0, 0};
byte tamper[2] = {0xFF};
byte uid_temp[15];
byte mUidLength;
byte txbuf[18];
byte rxbuf[32];
byte t_mem[32];
byte regbuf;
byte length;
int AUTO = 0;
bool tkt_kind = false;
int battLife;
int sensorValue;
int tag_in = 0;
uint8_t state = 0;
uint8_t mode = 0;
uint8_t err_code = 0;
uint8_t ran[255];
byte currOpMode = 0x00;
long batt_chk_time;
long BATT_CHK_TIME;
long sleep_timer;
long sysDelay;
long scanInterval;

//------ACCP Sound Ticket
#define addrTkt         0x27
#define sndFolder       0
#define sndTrack        1
#define sndCount        2
#define playMode        3

//------Module Connection
/* RTC via I2C*/
PCF8563 pcf;

/* MP3Player via UART*/
DFRobotDFPlayerMini myDFPlayer;

/* OLED Display via I2C*/
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

/* SICRE31 NFC Re-Writer via SPI */
class RE31_SPI{
public:  
  RE31_SPI(SPIClass &spi, uint8_t ss);
  void begin();
  void writeReg(byte addr, byte* data, uint8_t len);
  void writeReg(byte addr, byte data);
  void writeCmd(byte cmd);
  void writeFIFO(byte* data, uint8_t len);
  int16_t readReg(byte addr, byte* data, uint8_t len);
  byte readReg(byte addr);
  void readFIFO(byte* data, uint8_t len);
  uint8_t getFIFOLength();
      
  inline void select(boolean e){
    if(!e){
      digitalWrite(_ss, HIGH);
    }else{
      digitalWrite(_ss, LOW);
    }
  }
private:
  uint8_t _ss;
  SPIClass* _spi;
  inline void write(uint8_t data) {
        _spi->transfer(data);
  };
  inline byte read(byte addr) {          
      return _spi->transfer(addr);         
  };   
  
};
RE31_SPI::RE31_SPI(SPIClass &spi, uint8_t ss){
  _spi = &spi;
  _ss = ss;
}
void RE31_SPI::begin(){
  pinMode(_ss, OUTPUT);  
  _spi->begin();
  _spi->setDataMode(SPI_MODE0);
  _spi->setBitOrder(MSBFIRST);
  _spi->setClockDivider(SPI_CLOCK_DIV16);
}
void RE31_SPI::writeReg(byte addr, byte* data, uint8_t len){
  digitalWrite(_ss, LOW);  
  write(addr << 1);
  for(int i = 0; i < len; i++){
    write(data[i]);
  }
  digitalWrite(_ss, HIGH);  
}
int16_t RE31_SPI::readReg(byte addr, byte* data, uint8_t len){
  digitalWrite(_ss, LOW);  
  write(addr << 1 | 0x80);
  for(int i = 0; i < len; i++){
    data[i] = read(addr << 1);
  }
  digitalWrite(_ss, HIGH);  
}
void RE31_SPI::writeReg(byte addr, byte data){
  digitalWrite(_ss, LOW);  
  write(addr << 1);
  write(data);
  digitalWrite(_ss, HIGH);  
}
void RE31_SPI::writeCmd(byte cmd){
  writeReg(COMMAND_REG, cmd);
}
void RE31_SPI::writeFIFO(byte* data, uint8_t len){
  writeReg(FIFO_DATA_REG, data, len);
}
byte RE31_SPI::readReg(byte addr){
  digitalWrite(_ss, LOW);  
  write(addr << 1 | 0x80);
  byte buf = read(0);  
  digitalWrite(_ss, HIGH);  
  return buf;
}
void RE31_SPI::readFIFO(byte* data, uint8_t len){
  digitalWrite(_ss, LOW);  
  write(FIFO_DATA_REG << 1 | 0x80);
  for(int i = 0; i < len; i++){
    data[i] = read(FIFO_DATA_REG << 1); 
  }
  digitalWrite(_ss, HIGH);  
}
uint8_t RE31_SPI::getFIFOLength(){
  return readReg(FIFO_LENGTH_REG);
}

RE31_SPI RE31_SPI(SPI, SS);

/* Battery Power monitor via PIN0 */
void lowPowerHandler() {
  if (millis() - batt_chk_time > BATT_CHK_TIME) {
    batt_chk_time = millis();
    // read the input on analog pin 0:
    sensorValue = analogRead(0);
    delay(200); // wait for satable
    sensorValue = analogRead(0);
    randomSeed(analogRead(1));
#ifdef DEBUG
    Serial.print("LLB detect, sensorValue ="); Serial.println(sensorValue, DEC);
#endif
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 4.3V):
    // 0.969543 is here for compensating difference between measured and AnalogRead value;
    float voltage = (sensorValue * (4.3 / 1023.0)) * 0.969543;
    battLife = (voltage - 3.4) * 100 / (4.12-3.4);
    //battLife = voltage * 100;
    if (battLife > 100) {
        battLife = 100;
    }
    if (battLife <= LOW_VOLTAGE_THRESHOLD2) {
        alarmBeep(BUZZER,1000);
        delay(1000);
        ss = "STOP! BATT is LOW!";
        draw(LLB, 0xFF);
        delay(5000);
        sleepTimer = 99;
        return;
    } else if (battLife <= LOW_VOLTAGE_THRESHOLD1) {
        alarmBeep(BUZZER,500);
        delay(500);
        ss = "Recharge! BATT<30%";
        draw(LLB, 0xFF);
        delay(2000);
        sysDelay = 20000;
        BATT_CHK_TIME = 5000;
        return;
    }
    BATT_CHK_TIME = 20000;
    //ss = ss + "[B:" + String(battLife) + "%]";
    printTime();
  }
}

void setup(){
  String sl1; String sl2; String sl3;
  u8g2.begin();  
  u8g2.enableUTF8Print();
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_R,OUTPUT);
  pinMode(LED_G,OUTPUT);
  pinMode(LED_B,OUTPUT);
  pinMode(0,INPUT);
  digitalWrite(0, HIGH);
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);
#ifdef DEBUG
  Serial.begin(115200);
  //while (!Serial) { };
  Serial.println(NAME);
  digitalWrite(LED_R, LOW);
  delay(200);
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  delay(200);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
  delay(200);
  digitalWrite(LED_B, HIGH);
  digitalWrite(LED_G, LOW);
#endif
  alarmBeep(BUZZER,500);
  st = NAME;
  ss = "System ready...";
  RE31_SPI.begin();
  delay(100);
  if (SIC931_Check_Device() < 0) {
        sl1 = "-NFC RWD is FAIL..";
        digitalWrite(LED_R, LOW);
        ss = "System is some fault...";
  } else {
        sl1 = "-NFC RWD is OK..";
        SIC931_Config_14443a();
  }
  pcf.init();//initialize the clock
  if (pcf.checkClockIntegrity()) {//Check clock integrity. 
      sl2 = "-RTC=OK";
      smartVol();
  } else {
      sl2 = "-RTC=NG";
      f_validRTC = false;
      ss = "System is some fault...";
  } 
// see if the card is present and can be initialized:
  Serial1.begin(9600);
  while(!Serial1);
  bool tt = myDFPlayer.begin(Serial1);
  delay(1000);
  if (!tt) {  //Use softwareSerial to communicate with mp3.
    sl3 = "-MP3/SD failed..";
    ss = "System is some fault...";
    digitalWrite(LED_R, LOW);
  } else {
    sl3 = "-MP3/SD is ready..";
    delay(1000);
    myDFPlayer.setTimeOut(1000); // 設定通訊逾時為500ms
    myDFPlayer.volume(vol);  //Set volume value. From 0 to 30
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL); //
    myDFPlayer.playFolder(1, 1);
  }
  batt_chk_time = -20000;
  analogReference(AR_DEFAULT);
  lowPowerHandler();
  Serial.println("Battery checking");
  sl2 = sl2 + ", BATT=" + String(battLife) + "%";
  drawTXT(sl1,sl2,sl3);
  //while (digitalRead(BTN) == HIGH) { };
  delay(2000);
  sysDelay = 500;
  BATT_CHK_TIME = 20000;
  state = SCAN;
}

void loop()
{
  switch (state) {
    
    case SLEEP: { //enter sleep state
      st = "SLEEP NOW";
      ss = "after 2 sec..";
      draw(TIME, 0xFF);
      delay(2000);
      rwdIdle();
      rfOff();
      RE31_SPI.writeReg(CONTROL_REG, 0x3f); //set to standby/powerdown
      USBDevice.detach();
      st = ""; ss = "";
      draw(SLEEP, 0xF0);
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, HIGH);
      delay(100);
      myDFPlayer.pause();
      myDFPlayer.sleep();
      delay(1000);
      //digitalWrite(SYS_EN, LOW);
      // Enter standby mode
      LowPower.standby();
      break;  
    }
    
    case SCAN: {
      rwdIdle();
      if (isTagPresent()) {
         lowPowerHandler();
         t2_ReadPage(0x03, rxbuf); //uid[0]==0x39 && uid[1]==0x49) || uid[0]==0x04
         if (rxbuf[2] >= 0x12) { //check CC files, memoy size 
            tag_in = 1; 
            if (t2_ReadPage(addrTkt, t_mem)) {
                digitalWrite(LED_B, LOW);
                if ( t_mem[0] > 0 && t_mem[0] < 0x65 ) {
                    f_newTag= false; //
                    for (int i = 0; i < 7; i++) {
                        if (uid[i] != validUID[i]){
                            validUID[i] = uid[i];
                            f_newTag= true;
                        } 
                    }
                    if ( t_mem[0] == 0x64 ) {
                        state = CONFIG;
                    } else {
                        state = PLAYER;
                    }
                    st = "Folder/Track = [";
                    char tt[5]; 
                    sprintf(tt, "%02X/%02X", t_mem[0], t_mem[1]);
                    st = st + String(tt) +"] ";
                    ss = "Read Tag: OK..";
                    printTime();
                    draw(TIME, 0xFF);
                } else if (t_mem[1] < 1 || t_mem[2] < 1) {
                    // no such track
                    err_code = 0xE2;
                    state = NG_ERR;
                    break;
                } else {
                    // no such folder
                    err_code = 0xE0;
                    state = NG_ERR;
                    break;
                }
            } else {
              // read tag error
              err_code = 0xE3;
              state = NG_ERR;
              break; 
            }
         } else {
            // Not ACCP Tag
             digitalWrite(LED_B, HIGH);
             digitalWrite(LED_R, LOW);
             err_code = 0xE1;
             state = NG_ERR; 
             delay(100);
             alarmBeep(BUZZER, 100);
             delay(150);
             alarmBeep(BUZZER, 200);
             digitalWrite(LED_R, HIGH);
             break;
         } 
      } else {
        //no valid tag
        if (f_loop) {
            if (t_mem[0] == 0x64) {
                state = NOP;
            } else {
                state = PLAYER;
            }
        } else {
            delay(100);
            tag_in = 0;
            digitalWrite(LED_B, HIGH);
            st = FW_VERSION;
            ss= "Ready to scan..[B:" + String(battLife) + "%]";
            state = NOP;
        }
      }
      scanInterval = millis();
      break;
    }          
    
    case PLAYER: {
      numFolder = t_mem[0];
      mode = t_mem[3] & 0x0F;
#ifdef DEBUG
      Serial.print("New tag=");Serial.print(f_newTag, DEC);
      Serial.print(", folder=");Serial.print(numFolder, DEC);
      Serial.print(", track=");Serial.print(numTrack, DEC);
      Serial.print(", maxTrack=");Serial.print(maxTrack, DEC);
      Serial.print(", playMode=");Serial.println(mode, DEC);
#endif
      
      switch(mode) { 
            case 1: { // A-B | Single playing mode;
              st = " A-B REPEAT / SINGLE";
              f_loop = false;
              if (f_newTag) {
                  langType = 0;
                  setEQ(t_mem[3]);
                  numTrack = t_mem[1];
                  maxTrack = t_mem[1] + t_mem[2]-1;
                  myDFPlayer.playFolder(numFolder, numTrack);
                  delay(1000);
                  if (chkState() > 0) {
                      f_nowPlaying = true;
                      ss = "A-B Play Once..";draw(TRK, 0x01);
                  } else { //no such track;
                      f_nowPlaying = false;
                      ss = "Can't play..";draw(NG_ERR, 0x03);
                  }
                  delay(2000);
                  break;
              } else {
                  if (f_nowPlaying) { 
                      ss = "playing now..[B:" + String(battLife) + "%]";
                      draw(TRK, 0x01);
                      delay(2000);
                      sysDelay = 2000;
                  } else {
                      maxTrack = t_mem[1] + t_mem[2] -1; 
                      if (numTrack == maxTrack) {
                          numTrack = t_mem[1];
                          ss = "play from starting..";
                          delay(300); 
                      } else {
                          numTrack = numTrack + 1;
                          ss = "play next.."; 
                          delay(300);
                      }
                      myDFPlayer.playFolder(numFolder, numTrack);
                      delay(1000);
                      if (chkState() > 0) {
                          f_nowPlaying = true; draw(TRK, 0x01);
                          sysDelay = 2000;
                      } else {
                          f_nowPlaying = false;
                          ss = "Can't play.."; draw(NG_ERR, 0x03);
                          sysDelay = 500;
                      }
                      delay(2000);
                 }
              }
              break;
            }
            
            case 0: { // Album Loop All playing mode;
              st = " ----- ALBUM PLAY ----- ";
              f_loop = false;
              numTrack = t_mem[1];
              maxTrack = t_mem[2];
              if (f_newTag) {
                  langType = 0;
                  setEQ(t_mem[3]);
                  myDFPlayer.playFolder(numFolder, numTrack);
                  delay(1000);
                  if (chkState() > 0) {
                      f_nowPlaying = true;
                      ss = "play all folder.."; draw(TRK, 0x01);
                      sysDelay = 2000;
                  } else { //no such track;
                      f_nowPlaying = false;
                      ss = "Can't play.."; draw(NG_ERR, 0x03);
                      sysDelay = 500;
                  }
                  delay(2000);
              } else { //Tag was still in
                  delay(500);
                  if (f_nowPlaying) { 
                      ss = "playing now..[B:" + String(battLife) + "%]";
                      draw(TRK, 0x01);
                      delay(2000);
                      sysDelay = 2000;
                  } else { 
                      if (numTrack == maxTrack) {
                          numTrack = 1;
                          ss = "play loop.."; 
                      } else {
                          numTrack = numTrack + 1;
                          ss = "play next.."; 
                      }
                      t_mem[1] = numTrack;
                      int rr = t2_WritePage(addrTkt, t_mem);
                      if (rr ==1) {
                          ss = ss + "write OK";
                      } else {
                          ss = ss + "write FAIL";
                      }
                      myDFPlayer.playFolder(numFolder, numTrack);
                      delay(1000);
                      if (chkState() > 0) {
                          f_nowPlaying = true;
                          draw(TRK, 0x01);
                          sysDelay = 2000;
                      } else { //no such track;
                          f_nowPlaying = false;
                          ss = ss + ", & can't play.."; draw(NG_ERR, 0x03);
                          sysDelay = 500;
                      }
                      delay(2000);
                 }
              }
              break;
            }
            
            case 2: { // Audio Guide mode;
              st = " ----- AUDIO GUIDE ----- ";
              f_loop = false;
              numTrack = t_mem[1];
              if (f_newTag) {    
                  maxTrack = 1;
                  myDFPlayer.playFolder(numFolder, numTrack+langType*100);
                  delay(1000); //must wait
                  if (chkState() > 0) {
                      ss = "playing now..";
                      f_nowPlaying = true;
                      draw(TRK, 0x01);
                      sysDelay = 2000;
                  } else { //no such track;
                      f_nowPlaying = false;
                      ss = "can't play.."; draw(NG_ERR, 0x03);
                      sysDelay = 500;
                  }
                  delay(2000);
              } else { //Tag was still in
                  delay(500);
                  if (f_nowPlaying) { 
                      ss = "playing now..[B:" + String(battLife) + "%]";
                      draw(TRK, 0x01);
                      delay(2000);
                  } else { //repeat once
                      myDFPlayer.playFolder(numFolder, numTrack+langType*100);
                      delay(1000); //must wait
                      if (chkState() > 0) {
                          ss = "play repeat..";
                          f_nowPlaying = true;
                          draw(TRK, 0x01);
                          sysDelay = 2000;
                      } else { //no such track;
                          f_nowPlaying = false;
                          ss = "can't play.."; draw(NG_ERR, 0x03);
                          sysDelay = 500;
                      }
                      delay(2000);
                  }
              }
              break;
            }

            case 3: { // Background Music(BGM) playing mode;
              st = " ----- BGM PLAYER ----- ";
              if (f_newTag) {
                  maxTrack = t_mem[2];
                  setEQ(t_mem[3]);
                  genRanList(1, maxTrack, maxTrack);
                  currTrack = 1;
                  langType = 0;
                  f_loop = true;
                  //if (numTrack == 0) numTrack = 1;
                  numTrack = ran[currTrack];
                  myDFPlayer.playFolder(numFolder, ran[numTrack]);
                  delay(1000); //must wait
                  if (chkState() > 0) {
                      ss = "play all folder..";
                      f_nowPlaying = true;
                      draw(TRK, 0x01);
                      sysDelay = 2000;
                  } else { //no such track;
                      f_nowPlaying = false;
                      ss = "can't play.."; draw(NG_ERR, 0x03);
                      sysDelay = 500;
                  }
                  f_newTag = false;
                  delay(2000);
              } else {
                  if (f_nowPlaying) { 
                      ss = "playing now..[B:" + String(battLife) + "%]";
                      numTrack = ran[currTrack];
                      draw(TRK, 0x01);
                      delay(2000);
                      sysDelay = 2000;
                  } else { 
                      currTrack ++;
                      if (currTrack > maxTrack) {
                          currTrack = 1;
                      }
                      numTrack = ran[currTrack];
                      myDFPlayer.playFolder(numFolder, numTrack);
                      delay(1000); //must wait
                      if (chkState() > 0) {
                          ss = "play next..";
                          f_nowPlaying = true;
                          draw(TRK, 0x01);
                          sysDelay = 2000;
                      } else { //no such track;
                          f_nowPlaying = false;
                          ss = "can't play.."; draw(NG_ERR, 0x03);
                          sysDelay = 500;
                      }
                      delay(2000);
                 }
              }
             break;
            }

            case 4: { // Playlist playing Mode;
              st = " ----- PLAYLIST GO ----- ";
              f_loop = false;
              if (f_newTag) {
                  langType = 0;
                  setEQ(t_mem[3]);
                  currListAddr = t_mem[1];
                  startListAddr = t_mem[0];
                  countList = t_mem[2];
                  offsetList = currListAddr - startListAddr;
                  ss = "Playlist loading..[B:" + String(battLife) + "%]";
                  if (t2_ReadPage(currListAddr, rxbuf)) {
                      listFolder = rxbuf[0];
                      listTrack = rxbuf[1];
                      long pbDelay = rxbuf[2]*1000;
                      myDFPlayer.playFolder(listFolder, listTrack);
                      delay(1000); //must wait
                      if (chkState() > 0) {
                          f_nowPlaying = true;
                          countList = countList - offsetList -1;
                          offsetList++;
                          draw(LST, 0x02);
                          sysDelay = 2000;
                          if (pbDelay > 0) {
                              delay(pbDelay);
                              myDFPlayer.pause();
                              f_nowPlaying = false;
                          }
                      } else { //no such track;
                          f_nowPlaying = false;
                          ss = "can't play.."; draw(NG_ERR, 0x03);
                          sysDelay = 500;
                      }
                      delay(2000);
                  } else {
                      f_nowPlaying = false;
                      ss = "can't READ tag.."; draw(NG_ERR, 0x03);
                      sysDelay = 500;
                  }
              } else { //Tag was still in
                  delay(500);
                  if (f_nowPlaying) { 
                      ss = "playing now..[B:" + String(battLife) + "%]";
                      draw(LST, 0x02);
                      delay(2000);
                      sysDelay = 2000;
                  } else { 
                      if (countList == 0) {
                          t_mem[1] = startListAddr;
                          ss = "play done..[B:" + String(battLife) + "%]"; 
                          draw(TIME, 0xFF);
                          t2_WritePage(addrTkt, t_mem);
                          f_newTag = 0;
                      } else {
                          currListAddr = startListAddr + offsetList;
                          if (t2_ReadPage(currListAddr, rxbuf)) {
                              listFolder = rxbuf[0];
                              listTrack = rxbuf[1];
                              long pbDelay = rxbuf[2]*1000;
                              myDFPlayer.playFolder(listFolder, listTrack);
                              delay(1000); //must wait
                              if (chkState() > 0) {
                                  ss = "play next..[B:" + String(battLife) + "%]";
                                  t_mem[1] = currListAddr;
                                  countList --; offsetList++;
                                  f_nowPlaying = true;
                                  draw(LST, 0x02);
                                  sysDelay = 2000;
                                  t2_WritePage(addrTkt, t_mem);
                                  if (pbDelay > 0) {
                                      delay(pbDelay);
                                      myDFPlayer.pause();
                                      f_nowPlaying = false;
                                  }
                              } else { //no such track;
                                  f_nowPlaying = false;
                                  ss = "can't play.."; draw(NG_ERR, 0x03);
                                  sysDelay = 500;
                              }
                              delay(2000);
                          } else {
                              f_nowPlaying = false;
                              ss = "can't READ tag.."; draw(NG_ERR, 0x03);
                              sysDelay = 500;
                          }
                      }
                 }
              }
              break;
            }

            case 5: { // Quiz Game Mode;
              st = " ----- QUIZ GMAE ----- ";
              ss="PLS Remove GameDisc..";
              draw(GAMER, 0xFF);
              f_loop = false;              
              if (!f_gamePlaying) { //new start game
                  langType = 0;
                  setEQ(t_mem[3]);
                  hpGame = 3;
                  wpGame = 0;
                  myDFPlayer.playFolder(GFOLDER, 21);
                  delay(16000);
                  randomSeed(analogRead(1));
              } 
              quizGame();
              st = FW_VERSION;
              ss="Ready to scan..[B:" + String(battLife) + "%]";
              draw(SCAN, 0xFF);
              delay(2000);
              state = NOP;
              break;
            }
            
            case 6: { // Reserve for Future;
              numTrack = t_mem[1];
              if (tps_ReadTamper(tamper)) {
                  if (tamper[0] == 0 && tamper[1] == 0) {
                      numTrack = numTrack + 1; 
                      if (numTrack > maxTrack) numTrack = 1;
                      st = "Folder/track = [" + String(numFolder) + "/";
                      st = st + String(numTrack) + "]";
                      ss = "play next..[B:" + String(battLife) + "%]";
                      draw(TIME, 0xFF);
                      myDFPlayer.playFolder(numFolder, numTrack);
                      t_mem[1] = numTrack;
                      t2_WritePage(addrTkt, t_mem);
                      break;
                  } else {
                      numTrack = t_mem[1];
                      st = "Folder/track = [" + String(numFolder) + "/";
                      st = st + String(numTrack) + "]";
                      ss = "playing now..[B:" + String(battLife) + "%]";
                      draw(TIME, 0xFF);
                      myDFPlayer.playFolder(numFolder, numTrack);
                      break;
                  }
              } else {
                  ss = "No tamper flag..";draw(TIME, 0xFF);
              }
              break;
            }
        }
        state = NOP;
        break;
    }
    
    case CONFIG: {
        if (t_mem[1] == 0x44) {
            f_validRTC = true;
            st = "Setting RTC";
            ss = "Hold you tag a while..";
            switch (t_mem[2]) {
              case 0xA0: {
                 if(t2_ReadPage(addrTkt-3, t_mem)) { 
                     resetRTC(); //set RTC
                 }
                 state = NOP;
                 sysDelay = 2000; 
                 break;
              }
              case 0xA1: {
                 if (f_newTag) delay(500); 
                 fastRTC();
                 sysDelay = 0;
                 state = SCAN;
                 break;
              }
              case 0xA2: {
                if (f_newTag) delay(500); 
                slowRTC();
                sysDelay = 0;
                state = SCAN;
                break;
              }

              case 0xA8: { 
                if (!f_tagPGMR) {
                    if (f_nowPlaying) myDFPlayer.pause();
                    rfOn();
                    Serial.println("BTN Press, IN tagPGMR");
                    f_tagPGMR = true;
                    tagPGM();
                }
                sysDelay = 500;
                st = FW_VERSION;
                state = NOP;
                break;
              }
              
              case 0xAA: {
                if (f_newTag) {
                 state = SLEEP;
                 break;
                }
              }
              
              case 0xAF: { //set sleep timer
                st = "Set Sleep Timer (min.)";
                switch (sleepTimer) {
                  case 0: 
                      sleepTimer = 5; break;
                  case 5:
                      sleepTimer = 25; break;
                  case 25:
                      sleepTimer = 50; break;
                  case 50:
                      sleepTimer = 0; break;
                }
                setTimer();
                sysDelay = 2000;
                state = NOP;
                break;
              }
              
              case 0xB0: {
                st = "Setting LANG=";
                switch(t_mem[3]) {
                    case EN:
                       langType = 1; st += "EN"; break;
                    case JP:
                       langType = 2; st += "JP"; break;
                    default:
                       langType = 0; st += "TW"; break;
                }
                state = NOP;
                sysDelay = 2000;
                break;
              }
            }
            draw(TIME, 0xFF);
            delay(100);
        } else {
            state = NOP;
        }
        break;
    }
    
    case NG_ERR: {
      err = err +1;  if (err>10) err = 10;
      sr = "ERR";
      switch (err_code) {
        case 0xE0: 
          ss = "Folder/Track was wrong.."; break;
        case 0xE1: 
          ss = "Not a valid ACCP Tag.."; break;
        case 0xE2: 
          ss = "No track is exist.."; break;
        case 0xE3: 
          ss = "Tag can't be read.."; break;
        default:
          ss = "Unknown error.."; break;          
      }
      draw(NG_ERR, 0xFF);
      if (!f_nowPlaying) myDFPlayer.playFolder(1, err);
      delay(2000);
      sr = "SCAN";
      ss = "Put valid tag..";
      draw(SCAN, 0xFF);
      state = NOP;
      break;
    }
    
    case NOP: {
      lowPowerHandler();
      //chkState();
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_B, HIGH);
      rfOff();
      if (myDFPlayer.available()) {
          delay(100);
          uint8_t type = myDFPlayer.readType();
          int vv = myDFPlayer.read();
          #ifdef DEBUG
          Serial.print("myDFPlayer type=");Serial.print(type, DEC);
          Serial.print(", rsp_value=");Serial.println(vv, HEX);
          #endif
          switch (type) {
            case DFPlayerPlayFinished: {
                f_nowPlaying = false;
                st = FW_VERSION;
                ss="Ready to scan..[B:" + String(battLife) + "%]";
                draw(SCAN, 0xFF);
                sysDelay = 500;
                break;
            }
            default: {
              break;
            }
          }
      }
      if ((millis() - scanInterval) > sysDelay) {
          state = SCAN;
          rfOn();
          delay(20);
      } else {
          printTime();
          if (sleepTimer == 99) {
              sleepTimer = 0;
              state = SLEEP;
          } else {
              state = NOP;
          }
      }
      break;
    }
  
  }
}

void alarmBeep(int pin, int tme) {
  tone(pin, 5800, tme);
  //printTime();
  //delay(2000); 
}
