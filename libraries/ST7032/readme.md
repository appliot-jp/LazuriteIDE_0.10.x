I2C制御のLCDコントローラ ST7032iを制御するためのドライバです。

以下のコードで上段に「Lazurite」の表示を
下段に「00-99」までの数字を約0.5秒でカウントアップします。

LazuriteIDE\Librariesのフォルダに、ST7032フォルダごと保存してください。
以下のプログラムで動作確認をしているだけで、ライブラリの全機能評価は行っていません。

void setup(){
	lcd.init();
    lcd.begin(8, 2,LCD_5x8DOTS);
    lcd.setContrast(30);
    lcd.print("Lazurite");
}

void loop(){
	static int i=0;
	uint8_t msg[16];
	Print.init(msg,sizeof(msg));
	if(i<10)
	{
		Print.p("0");
	}
	Print.l(i,DEC);
    lcd.setCursor(0, 1);
    lcd.print(msg);
	delay(500);
	i++;
	if(i>=100) i=0;
}

