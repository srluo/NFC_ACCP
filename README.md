# NFC Acoustic Companion CC Project
1-1 前言

「沒有音樂，生活將是一種錯誤」，這句哲學家尼采的名言，隨著歲月的增長體會更為深刻。一生中，總有那麼一天，會發現自己開始『耳不聰、目不明』，卻絕對不願意放棄有音樂陪伴的生活；更簡單、更直覺的方式，可以隨時讓空氣中流動著，那些熟悉且令自己動容的音符，繼續撫慰與陪伴可以精彩的人生歲月。
《NFC聲活良伴》是自己邁入半百人生後，新的熱情創作。將長久以來專注的RFID/NFC技術，與古早家中電唱機的記憶，以創客(Maker)常用的自造工具，開發出這一款適合中年生活的MP3音樂隨身聽（當然，也可以用作兒童的故事機；說書盒子；留聲禮盒；語音導覽等等……）。現在，將完整開放所有的設計與原始程式碼，讓有志一同的人，也可以自己DIY分享與製造，屬於自己或為父母家人專屬的『NFC聲活良伴』。

1-2 架構概述與使用情境

採用最多創客使用的Arduino IDE環境開發，以Microchip ATSAMD21-G18微控制器為核心的Seeeduino XIAO開發主板[圖1-C]，與其專用含OLED顯示器的擴充板[圖1-B] (XIAO Expansion Board)為主體；結合SICRE31 NFC Re-Writer[圖1-H]與DFPlayer MP3 Mini [圖1-D]，架構出具備可充電便攜式的MP3隨身聽；並內置2吋的全音域喇叭[圖1-I]，可以久聽不膩。
DFPlayer具有Micro SD卡槽，可以播放存放在SD卡內的MP3音樂。以FAT/FAT32檔案目錄架構的16G卡，可以存放超過上千首，最高音質的6分鐘MP3音樂；而其檔案目錄形式係以：/01/~/99/的格式來存每張專輯或播放列表；每個目錄下可自編號001~999的順序來寫入個別的MP3檔案。
在取得有版權的數位音樂或自行從CD唱片轉錄的MP3音樂，依其檔案格式寫入SD卡內即可播放。例如：目錄/09/001往事只能回味.mp3，表示在第09專輯裡，第1首的曲子，檔名必須是三位數字帶頭，後面可以加上中英文曲名。

為求播放音樂操作上的便利與直覺性，只要將『NFC小唱片』(XiaoDisc, 簡稱XD)[圖1-Ｇ] 插入感應槽內，XIAO主機便會透過NFC讀寫器，來感應唱片內置的NFC標籤，以讀取其內特定編碼的內容，來決定要播放的曲目與播放的方式[單首/全部/區間循環播放/語音導覽/播放列表/問答遊戲…等]。例如：[09 01 0A 00]，表示依序播放目錄/09/內的所有10首曲目；又如：[20 05 01 01]，表示播放目錄/32/內重複播放第5首曲目……等。

XIAO主機上具有USB Type C的接口，可以用來連接電腦使用Arduino IDE來編修程式，或者對內建的鋰電池充電。
