void resetRTC() {

  pcf.stopClock();//stop the clock

  //set time to to 31/3/2018 17:33:0

  pcf.setYear(t_mem[1]);//set year
  pcf.setMonth(t_mem[2]);//set month
  pcf.setDay(t_mem[3]);//set dat
  pcf.setHour(t_mem[5]);//set hour
  pcf.setMinut(t_mem[6]);//set minut
  pcf.setSecond(t_mem[7]);//set second

  pcf.startClock();//start the clock
  sleepTimer = 0;
  printTime();
}

void fastRTC() {
  Time nowTime = pcf.getTime();//get current time
  pcf.stopClock();//stop the clock
  if (nowTime.minute == 59) {
      pcf.setHour(nowTime.hour + 1);
      pcf.setMinut(0);//set minut
  } else {
      pcf.setMinut(nowTime.minute + 1);//set minut
  }
  pcf.setSecond(0);//set second
  pcf.startClock();//start the clock
  sleepTimer = 0;
  st = "Setting Time +";
  ss = "hold tag to continue..";
  printTime();
}

void slowRTC() {
  Time nowTime = pcf.getTime();//get current time
  pcf.stopClock();//stop the clock
  if (nowTime.minute == 0) {
        if (nowTime.hour == 0) {
              pcf.setHour(23);
        } else {
              pcf.setHour(nowTime.hour - 1);
        }
        pcf.setMinut(59);//set minut
  } else {
      pcf.setMinut(nowTime.minute - 1);//set minut
  }
  pcf.setSecond(0);//set second
  pcf.startClock();//start the clock
  sleepTimer = 0;
  st = "Setting Time -";
  ss = "hold tag to continue..";
  printTime();
}

void setTimer() {
  Time nowTime = pcf.getTime();//get current time
  if (f_validRTC) {
      timerMin = sleepTimer + nowTime.minute;
      if (timerMin > 60) {
          timerMin = timerMin -60;
          timerHour = nowTime.hour + 1; 
      } else {
          timerHour = nowTime.hour;
      }
  }
  Serial.print("sleepTime=");Serial.print(sleepTimer, DEC);
  Serial.print(", Hour=");Serial.print(timerHour, DEC);
  Serial.print(", Min=");Serial.println(timerMin, DEC);
  if (sleepTimer == 0) {
      sr = "OFF";
  } else {
      sr = String(sleepTimer);
  }
  draw(TIMER, 0xF1);
  delay(2000);
  if (timerHour < 10) {
      sr = "0" + String(timerHour);
  } else {
      sr = String(timerHour);
  }
  if (timerMin < 10) {
      sr = sr + ":0" + String(timerMin);
  } else {
      sr = sr + ":" + String(timerMin);
  }
  if (sleepTimer == 0) {
      sr = "--:--";
  }
  draw(OFF, 0xFF);
  delay(2000);
}

void printTime() {
  Time nowTime = pcf.getTime();//get current time
  if (f_validRTC) {
      if (nowTime.hour < 10) {
          sr = "0" + String(nowTime.hour);
      } else {
          sr = String(nowTime.hour);
      }
      if (nowTime.minute < 10) {
          sr = sr + ":0" + String(nowTime.minute);
      } else {
          sr = sr + ":" + String(nowTime.minute);
      }
  }
  
  if (sleepTimer > 0) {
      if ((nowTime.hour == timerHour) && (nowTime.minute == timerMin)) {
          sleepTimer = 99;
      }
      draw(TIMER, 0xFF);
  } else {
      draw(TIME, 0xFF);
  }
}
