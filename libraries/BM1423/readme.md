##BM1423用ライブラリ
本プログラムはローム製地磁気センサー BM1423用のライブラリです。

## API
*void bm1423.init(int slave_address);
BM1423の初期化を行います。slave_addressを0x0Fを設定するとBME1423のスレーブアドレスを0x0Fにして動作します。それ以外の時のslave_addressは0x0Eになります。
正常に完了すると0を返します。

*byte bm1423.get_val(float *data);
地磁気センサーの値を取得します。dataには、3つのfloat型配列の先頭ポインタを指定してください。正常に完了すると0を返します。


#Sample Program
void setup() {
  byte rc;

  Serial.begin(115200);
  
  Wire.begin();
  
  rc = bm1423.init(0);
}

void loop() {
  byte rc;
  float mag[3];
  rc = bm1423.get_val(mag);

  if (rc == 0) {
    Serial.print("BM1423 XDATA=");
    Serial.println_double(mag[0], 2);
    Serial.print("BM1423 YDATA=");
    Serial.println_double(mag[1], 2);
    Serial.print("BM1423 ZDATA=");
    Serial.println_double(mag[2], 2);
    Serial.print("\n");    
  }
  
  delay(500);
}


