//
void tagPGM() {
  long dd = millis();
  int op = 0;
  st = "-- Tag ProGramMER --";
  sr = "COPY";
  ss = "Ready for copy tag..";
  alarmBeep(BUZZER, 200);
  delay(50);
  alarmBeep(BUZZER, 200);
  draw(SCAN, 0xFF); 
  delay(3000);      
  while (f_tagPGMR) {
      /*if ((digitalRead(BTN) == LOW) & (!tag_in)) {
          delay(200);
          if (digitalRead(BTN) == LOW) {
              Serial.println("BTN Press, QUIT tagPGMR");
              f_tagPGMR = false;
              if (f_nowPlaying) myDFPlayer.start();
              sysDelay = 1000;
              st = FW_VERSION;
              ss = "Leave tagPGMER..";
              sr = "QUIT";
              draw(SCAN, 0xF0);
              alarmBeep(BUZZER, 200); 
              delay(200);          
          }
      }*/
      switch (op) {
          case 0: {
              sr = "MASR";
              ss = "Put master tag..";
              draw(SCAN, 0xFF);       
              rwdIdle();
              if (isTagPresent()) {
                  int rr = t2_ReadPage(addrTkt-3, t_mem);
                  if (rr) {
                      op = 1;
                      dd = millis();
                      ss = "Read master OK..";
                      draw(SCAN, 0xFF);       
                      alarmBeep(BUZZER, 200); 
                      delay(1000);
                  }
              }
              if ((millis()-dd) > 10000) {
                  f_tagPGMR = false;
                  if (f_nowPlaying) myDFPlayer.start();
                  sysDelay = 1000;
                  Serial.println("BTN Press, QUIT tagPGMR");
                  st = FW_VERSION;
                  ss = "Leave tagPGMER..";
                  sr = "QUIT";
                  draw(SCAN, 0xFF);
                  alarmBeep(BUZZER, 200); 
                  delay(200);        
              }
              break;
          }
          case 1: {
              sr = "SLAV";
              ss = "Put any tag to copy..";
              draw(SCAN, 0xFF);
              rwdIdle();
              if (isTagPresent()) {
                  int i = addrTkt-3;
                  byte temp[4];
                  for (int ii=0; ii<4; ii++) {
                      for (int jj=0; jj<4; jj++) {
                          temp[jj] = t_mem[ii*4+jj];
                      }
                      t2_WritePage(i+ii, temp);
                  }
                  alarmBeep(BUZZER, 100); 
                  delay(300); 
                  ss = "Write OK, next..";
                  draw(SCAN, 0xFF);
                  delay(1000);       
                  dd = millis();
              }
              if ((millis()-dd) > 5000) {
                  op = 0;
                  dd = millis();
              }
              break;
          }
      }
  }
}
