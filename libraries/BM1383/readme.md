##BME1383用ライブラリ
本プログラムはローム製温度／気圧センサー BME1383のライブラリです。

## API
*void bme1383.init(int slave_address);
BME1383の初期化を行います。slave_addressを0に設定するとBME1383のスレーブアドレス=0x5Dが初期設定されます。

*void bme1383.get_temppresval(float* temp, float* press);
温度と気圧の値を取得します。
tempは温度の値を格納するfloat型のポインタを、pressは気圧を格納するfloat型のポインタを指定してください。


