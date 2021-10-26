void genRanList(int a, int b, int ct) {

Serial.print("ranList=");
  for (int i = a; i <= ct; i++) {
          ran[i] = random(a,b+1); //亂數產生，亂數產生的範圍是1~10
          for (int j = 0; j < i; j++) 
          {
              while (ran[j] == ran[i]) //檢查是否與前面產生的數值發生重複，如果有就重新產生
              {
                  j = 0; //如果重複，將更改數j設為0，再次檢查（因為還是有重複的可能）
                  ran[i] = random(a,b+1); //重產生，存回，亂數產生的範圍是1~9
              }
          }
          Serial.print(ran[i], DEC); Serial.print(", "); 
  }
  Serial.println("end"); 

}
