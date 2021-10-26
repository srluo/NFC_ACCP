//0-Normal/1-Pop/2-Rock/3-Jazz/4-Classic/5-Bass
void setEQ(uint8_t _eq) {
  _eq = _eq >> 4;
  f_eqOn = true;
  switch(_eq) {
    case 0: myDFPlayer.EQ(DFPLAYER_EQ_NORMAL); 
            f_eqOn = false; break;
    case 1: myDFPlayer.EQ(DFPLAYER_EQ_POP); break;
    case 2: myDFPlayer.EQ(DFPLAYER_EQ_ROCK); break;
    case 3: myDFPlayer.EQ(DFPLAYER_EQ_JAZZ); break;
    case 4: myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC); break;
    case 5: myDFPlayer.EQ(DFPLAYER_EQ_BASS); break;
  }
}

void smartVol() {
  Time nowTime = pcf.getTime();//get current time
  if ((nowTime.hour < 8) || (nowTime.hour >= 22))  {
      vol = 2;
  } else if ((nowTime.hour >= 8) || (nowTime.hour < 12)) {
      vol = 15;
  } else if ((nowTime.hour >= 12) || (nowTime.hour < 14)) {
      vol = 10;
  } else {
      vol = 20;
  }
}

int chkState() {
  int vv = myDFPlayer.readState();
  Serial.print("Check myDFPlayer state=");Serial.println(vv, DEC);
  // 0:NO_TRACK; 1:PLAYING; 2:PAUSE; 3:STOP; -1:NACK;
  /*  if (vv != 1) {
        f_nowPlaying = false;
    }*/
  return vv;
}
