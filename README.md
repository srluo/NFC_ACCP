# NFC Acoustic Companion CC Project
### 1-1 前言 ###

「*沒有音樂，生活將是一種錯誤*」，這句哲學家尼采的名言，隨著歲月的增長體會更為深刻。一生中，總有那麼一天，會發現自己開始『耳不聰、目不明』，卻絕對不願意放棄有音樂陪伴的生活；更簡單、更直覺的方式，可以隨時讓空氣中流動著，那些熟悉且令自己動容的音符，繼續撫慰與陪伴可以精彩的人生歲月。
《**NFC聲活良伴**》是自己邁入半百人生後，新的熱情創作。將長久以來專注的RFID/NFC技術，與古早家中電唱機的記憶，以創客(Maker)常用的自造工具，開發出這一款適合中年生活的MP3音樂隨身聽（當然，也可以用作兒童的故事機；說書盒子；留聲禮盒；語音導覽等等……）。現在，將完整開放所有的設計與原始程式碼，讓有志一同的人，也可以自己DIY分享與製造，屬於自己或為父母家人專屬的『NFC聲活良伴』。

### 1-2 架構概述與使用情境 ###
![ALL](https://user-images.githubusercontent.com/17289414/140821858-b84be1b9-cfe0-432e-97b1-d366529232d1.png)

採用最多創客使用的Arduino IDE環境開發，以 Microchip ATSAMD21-G18 微控制器為核心的 Seeeduino XIAO 開發主板 `圖.F`，與其專用含OLED顯示器的擴充板 `圖.E` (XIAO Expansion Board)為主體；結合SICRE31 NFC Re-Writer `圖.B` 與DFPlayer MP3 Mini `圖.G`，架構出具備可充電便攜式的MP3隨身聽；並內置2吋的全音域喇叭`圖.A`，可以久聽不膩。

DFPlayer具有Micro SD卡槽 `圖.H`，可以播放存放在SD卡內的MP3音樂。以FAT/FAT32檔案目錄架構的16G卡，可以存放超過上千首，最高音質的6分鐘MP3音樂；而其檔案目錄形式係以：/01/ - /99/的格式來存每張專輯或播放列表；每個目錄下可自編號001至999的順序來寫入個別的MP3檔案。在取得有版權的數位音樂或自行從CD唱片轉錄的MP3音樂，依其檔案格式寫入SD卡內即可播放。例如：目錄/09/001往事只能回味.mp3，表示在第09專輯裡，第1首的曲子，檔名必須是三位數字帶頭，後面可以加上中英文曲名。

為求播放音樂操作上的便利與直覺性，只要將『NFC小唱片』(XiaoDisc, 簡稱XD)`圖.L - 圖.M` 插入感應槽內，XIAO主機便會透過NFC讀寫器，來感應唱片內置的NFC標籤，以讀取其內第`0x27`夜的特定編碼內容(每頁含4個位元組的資料，來決定要播放的曲目與播放的方式。例如：`09 01 0A 00`，表示依序播放目錄/09/內的所有10首曲目；又如：`20 05 01 01`，表示播放目錄/32/內重複播放第5首曲目……等。 

> 電唱機的曲目索引格式：
 
**X-Disc晶片記憶體位址**：`0x27`

`Byte0`: 目錄編號 (容許值: 1~99，HEX:0x01 ~ 0x63);

`Byte1`: 檔案名稱 (前三碼容許值: 001~255, HEX: 0x01 ~ 0xFF);

`Byte2`: 曲目總數 (容許值: 1~99，HEX:0x01 ~ 0x63);

`Byte3`: EQ/播放模式 (`高4位元`表啟用EQ模式：0:Normal/1:Pop/2:Rock/3:Jazz/4:Classical/5:Bass；
                   `低4位元`表播放模式：0:專輯全曲/1:A-B循環或單曲/2:語音導覽/3:背景音樂/4:播放列表/ 5:聲音遊戲); 

XIAO主機上具有USB Type C的接口，可以用來連接電腦使用Arduino IDE來編修程式，或者對內建的鋰電池`圖.I`充電。


### 1-3 功能與規格 ###

音樂播放以感應NFC標籤晶片製成的小唱碟(X-DISC)為主，亦可採用同型各式標籤嵌入於物件之中來觸發，共有:

> 6種播放模式

* `ALBUM PLAY` 專輯播放模式，可接續並循環播放專輯目錄內所有曲目。
* `A-B REPEAT` 區間循環播放，指定播放目錄內從A首到B首間所有曲目。
* `SINGLE PLAY` 單首曲目播放，指定播放目錄內單一首曲目。
* `AUDIO GUIDE` 語音導覽模式，可切換多國語言來播放相對應語音說明。
* `BGM SHUFFLE` 背景音樂隨選播放，在指定專輯目錄內以隨機方式播放。
* `PLAYLIST GO` 播放列表演出模式，依照預先編排的曲目順序播放。
* `QUIZ GAMER` 快問快答遊戲模式，以互動與語音方式，考驗聲音的記憶。


> 規格與外觀尺寸

* `主控核心處理器` ATSAMD21-G18/ ARM® Cortex®-M0+ 32bit 48MHz microcontroller(SAMD21G18) with 256KB Flash,32KB SRAM
* `韌體開發環境` 相容於Arduino IDE
* `NFC裝置` FESID4 with Silicon Craft RE31 Reader chip @ 13.56MHz, ISO14443A/ 支援NFC Type 2 Tag
* `MP3播放器` DFPlayer Mini, MicroSD 16/32GB, 3W功放, 3鍵音量/播放控制
* `其他裝置` 0.96”OLED顯示器 / 內建PCF8563 RTC / 支援Type-C USB 3.7V Li-Po電池（800mAh）充電控制
* `電唱機尺寸` (長x寬x高)：128mm x 78.5mm x 29mm
* `NFC小唱片` XD採復古黑膠唱片縮小版的風格，為一直徑49mm圓片，其中心內置一直徑30mm彩印圓型貼標；外封套為方形54mm x 54mm

NFC ACCP相關程式碼的開發與編譯，請參考 -> [Wiki](https://github.com/srluo/NFC_ACCP/wiki/2-準備篇)

NFC ACCP電路圖，請參考 -> [PNG](https://github.com/srluo/NFC_ACCP/SCH-2021-1015-1.png)
