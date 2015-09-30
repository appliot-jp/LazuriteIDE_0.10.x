##ML8711用ライブラリ
本プログラムはラピスセミコンダクタ製UVセンサー ML8511用ライブラリです。

## API
*void ml8511.init(int pin);
ML8511が出力する信号をAD変換する端子の初期化を行います。
A0-A5を指定してください。

*void ml8511.get_val(float *data);
UVの値を取得します。

#Sample Program

int ADCpin_AnalogUV = A0;

void setup() {
	Serial.begin(115200);
	ml8511.init(ADCpin_AnalogUV);
  
}

void loop() {
  float uv;
  
  ml8511.get_val(&uv);
  
  Serial.print("ML8511 UV = ");
  Serial.print_double((double)uv,0);
  Serial.println(" [mW/cm2]");
  Serial.println("");
  
  delay(500);
}





