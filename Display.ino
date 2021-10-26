void drawSTATESymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)
{
  // fonts used:
  // u8g2_font_open_iconic_check_6x_t
  // u8g2_font_open_iconic_mime_6x_t
  // encoding values, see: https://github.com/olikraus/u8g2/wiki/fntgrpiconic
  switch(symbol)
  {
    case LST:
      u8g2.setFont(u8g2_font_open_iconic_mime_2x_t);
      u8g2.drawGlyph(x, y, 70);
      break;
    case TRK:
      u8g2.setFont(u8g2_font_open_iconic_play_2x_t);
      u8g2.drawGlyph(x, y, 77);
      break;
    case TIME:
      u8g2.setFont(u8g2_font_open_iconic_app_4x_t);
      u8g2.drawGlyph(x, y, 69);
      break;
    case TIMER:
      u8g2.setFont(u8g2_font_open_iconic_app_4x_t);
      u8g2.drawGlyph(x, y, 72);
      break;
    case GAMER:
      u8g2.setFont(u8g2_font_open_iconic_play_4x_t);
      u8g2.drawGlyph(x, y, 64);
      break;
    case LLB:
      u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
      u8g2.drawGlyph(x, y, 64);
      break;
    case SCAN:
      u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
      u8g2.drawGlyph(x, y, 80);
      break;
    case NG_ERR:
      u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
      u8g2.drawGlyph(x, y, 71);
      break;
    case SLEEP:
      u8g2.setFont(u8g2_font_logisoso32_tr);
      u8g2.drawGlyph(x, y, 32);
      break;
    case OFF:
      u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
      u8g2.drawGlyph(x, y, 78);
      break;                
  }
}

void drawSTATE(uint8_t symbol, byte kind)
{
  switch (kind) {
    case 0x01: //for Audio Guide Mode only
      drawSTATESymbol(0, 32, symbol);
      u8g2.setCursor(0, 48);
      u8g2.setFont(u8g2_font_ncenB08_tr);
      switch(langType) {
        case EN:
          u8g2.print("EN"); break;
        case JP:
          u8g2.print("JP"); break;
        default:
          u8g2.print(""); break;
      }
      if (f_eqOn) {
          u8g2.print("EQ");
      } else {
          u8g2.print("");
      }
      u8g2.setCursor(16+3, 48);
      u8g2.setFont(u8g2_font_logisoso32_tr);
      sr = "";
      sr = String(numFolder) + "/" + String(numTrack);
      u8g2.print(sr);
      break;
      
    case 0x02: //for Playlist-Mode only
      drawSTATESymbol(0, 32, symbol);
      u8g2.setCursor(0, 48);
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.print(String(offsetList));
      u8g2.setCursor(16+3, 48);
      u8g2.setFont(u8g2_font_logisoso32_tr);
      sr = "";
      sr = String(listFolder) + "/" + String(listTrack);
      u8g2.print(sr);
      break;
      
    case 0x03: //for error message
      drawSTATESymbol(0, 48, symbol);
      u8g2.setCursor(32+3, 48);
      u8g2.setFont(u8g2_font_logisoso32_tr);
      sr = "";
      sr = String(numFolder) + "/" + String(numTrack);
      u8g2.print(sr);
      break; 
       
    case 0xF0: //for SLEEP state only
      sr = "";
      u8g2.print(sr);
      break;
      
    case 0xF1:
      drawSTATESymbol(0, 48, symbol);
      u8g2.setCursor(32+3, 48);
      u8g2.setFont(u8g2_font_logisoso32_tr);
      u8g2.print(sr);
      u8g2.setCursor(75, 48);
      u8g2.setFont(u8g2_font_logisoso16_tr);
      if (sr != "OFF") {
         u8g2.print("min.");
      }
      break;
        
    case 0xFF:
      drawSTATESymbol(0, 48, symbol);
      u8g2.setCursor(32+3, 48);
      u8g2.setFont(u8g2_font_logisoso32_tr);
      u8g2.print(sr);
      break;
  }
}

void draw(uint8_t symbol, byte kind)
{
    u8g2.firstPage();
    do {
      drawSTATE(symbol, kind);
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.setCursor(0, 12);
      u8g2.print(st);
      u8g2.setCursor(0, 62);
      u8g2.print(ss);
      //drawScrollString(offset, s);
    } while ( u8g2.nextPage() );
    delay(20);
}

void drawTXT(String sl1, String sl2, String sl3)
{
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.setCursor(0, 12);
      u8g2.print(st);
      u8g2.setCursor(0, 24);
      u8g2.print(sl1);
      u8g2.setCursor(0, 36);
      u8g2.print(sl2);
      u8g2.setCursor(0, 48);
      u8g2.print(sl3);
      u8g2.setCursor(0, 62);
      u8g2.print(ss);
      //drawScrollString(offset, s);
    } while ( u8g2.nextPage() );
    //delay(2000);
}
