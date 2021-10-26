void quizGame() {
  int quiz = 0;
  genRanList(1, 10, 10);
  int qq = 1;
  do {
      quiz = ran[qq];
      Serial.print("hpGame=");Serial.print(hpGame, DEC);
      Serial.print(", wpGame=");Serial.print(wpGame, DEC);
      Serial.print(", quiz=");Serial.print(quiz, DEC);
      int f_timeout = true;
      sr = "Q#" + String(qq);
      ss = "WP=" + String(wpGame) + ", HP=" + String(hpGame);;
      draw(GAMER, 0xFF);
      myDFPlayer.playFolder(GFOLDER, quiz);
      delay(2000);
      long quizInterval = millis();
      delay(1000);
      do {
           rwdIdle();
           if (isTagPresent()) {
              t2_ReadPage(addrTkt, t_mem);
              Serial.print(", ans=");Serial.println(t_mem[1] -10, DEC);
              if (t_mem[3] == 5) {
                  sysDelay = 5000;
                  sr = "QUIT";
                  ss = "Sorry, see you again..";
                  draw(GAMER, 0xFF);
                  delay(2000);
                  f_gamePlaying = false;
                  myDFPlayer.playFolder(1, 2); 
                  scanInterval = millis();
                  Serial.println("Quit Game");
                  return;
              } else {
                  int ans = t_mem[1] - 10;
                  if (ans == quiz) {
                      wpGame ++;
                      sr = "[O]";
                      ss = "WP=" + String(wpGame) + ", HP=" + String(hpGame);
                      draw(GAMER, 0xFF);
                      myDFPlayer.playFolder(1, 15);
                      delay(5000);
                  } else {
                      hpGame--;
                      sr = "[X]";
                      ss = "WP=" + String(wpGame) + ", HP=" + String(hpGame);
                      draw(GAMER, 0xFF);
                      myDFPlayer.playFolder(1, 6);
                      delay(5000);
                  }
                  f_timeout = false;
                  break;
              }
           }
      } while ((millis() - quizInterval) < 10000);
      if (f_timeout) {
          hpGame--;
          sr = "[?]";
          ss = "WP=" + String(wpGame) + ", HP=" + String(hpGame);
          draw(GAMER, 0xFF);
          Serial.println(" No answer? --Timeout!");
          myDFPlayer.playFolder(1, 7);
          delay(5000);
      }
      qq++;
  } while ((wpGame < 3) && (hpGame > 0));
  
  f_gamePlaying = false;
  if (hpGame == 0) {
      sr = "[GG]";
      ss = "You fail this game..";
      draw(GAMER, 0xFF);
      myDFPlayer.playFolder(1, 11); //game over
      delay(4000);
  } else if (wpGame ==3) {
      sr = "V=V";
      ss = "You win this game..";
      draw(GAMER, 0xFF);
      myDFPlayer.playFolder(1, 14); //pass game
      delay(6000);
  }
  
}
