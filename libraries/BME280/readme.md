#BME280
LazuriteでBOSH製 温湿度／気圧センサー BME280からデータを取得するためのライブラリです。

#使い方
bme280のフォルダをLazuriteIDE\Librariesのフォルダにコピーしてください。
bme280.init(CSB端子の番号) でBME280を初期化します。
bme280.read(double *temp, double *humi, double *press)でデータを取得します。
temp, humi, pressは、それぞれ温度／湿度／気圧のデータを格納するdouble型のポインタを指定してください。
ポインタの引数にNULLを指定した場合、対象の値は取得しません。

#サンプルコード
サンプルコードでは、取得した温度／湿度／気圧のデータをシリアルで出力します。

------ここから------

#define BME280_CSB 10

void setup()
{
	
	Serial.begin(115200);
	bme280.init(BME280_CSB);
}


void loop()
{
    double temp_act = 0.0, press_act = 0.0,hum_act=0.0;
    
	bme280.read(&temp_act, &hum_act, &press_act);
	
    Serial.print("TEMP : ");
    Serial.print_double(temp_act,2);
    Serial.print(" DegC  PRESS : ");
    Serial.print_double(press_act,2);
    Serial.print(" hPa  HUM : ");
    Serial.print_double(hum_act,2);
    Serial.println(" %");
	
	sleep(1000);
}





