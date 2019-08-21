#include "LSM9DS1_kalman_serial_ide.h"		// Additional Header

#define BLUE_LED                 (26)
#define LSM9DS1_INT1_AG_DRV_PIN  (2) //pin#5
#define TXDATA_SIZE              (100)

bool event = false;
uint8_t txdata[TXDATA_SIZE];
float acc_f[3];
float gyro_f[3];
float gyro_offset[3];
static KALMAN_PARAM kalman_param;

void int1_ag_isr(void)
{
  event = true;
}

void acc_gyro_init(void)
{
  Wire.begin();
  LSM9DS1.init(LSM9DS1_OPMODE_XLG, LSM9DS1_I2C_ADDR_XLG_6A, NULL);
  LSM9DS1.setAccRange(LSM9DS1_XL_FS_RANGE_2G);
  LSM9DS1.setGyroRange(LSM9DS1_G_FS_RANGE_500DPS);
  LSM9DS1.begin(LSM9DS1_ODR_XLG_14_9Hz, NULL);
}


void setup() {
  // put your setup code here, to run once:
  int i;
  static float acc[3],gyro[3],degPitch;
  Serial.begin(115200);

  acc_gyro_init();

  pinMode(LSM9DS1_INT1_AG_DRV_PIN, INPUT);
  attachInterrupt(LSM9DS1_INT1_AG_DRV_PIN - 2, int1_ag_isr, RISING); //外部割込み
  Serial.println("hello");

  kalman.init(&kalman_param);
  acc[0]=0,acc[1]=0,acc[2]=0;
  gyro[0]=0,gyro[1]=0,gyro[2]=0;
  for (i = 0; i < 10; i++) {
    wait_event(&event);
    if (LSM9DS1.availableAcc() && LSM9DS1.availableGyro()) {
      LSM9DS1.getAcc(acc_f);
      LSM9DS1.getGyro(gyro_f);
      acc[0]+=acc_f[0];
      acc[1]+=acc_f[1];
      acc[2]+=acc_f[2];
    }
  }
  degPitch = atan2(acc[2]/10, sqrt(sq(acc[0]/10) + sq(acc[1]/10)))*RAD_TO_DEG;
  kalman.set(&kalman_param,degPitch);

}

void loop() {
  // put your main code here, to run repeatedly:
  float degPitch,kPitch;
  wait_event(&event);

  if (LSM9DS1.availableAcc() && LSM9DS1.availableGyro()) {
    LSM9DS1.getAcc(acc_f);
    LSM9DS1.getGyro(gyro_f);

	degPitch = atan2(acc_f[2], sqrt(sq(acc_f[0]) + sq(acc_f[1])))*RAD_TO_DEG;

 	kPitch=kalman.cal(&kalman_param,degPitch,gyro_f[1]);

    Print.init(txdata, TXDATA_SIZE);

    Print.p("STX,");
    Print.f(gyro_f[0], 3);
    Print.p(",");
    Print.f(gyro_f[1], 3);
    Print.p(",");
    Print.f(gyro_f[2], 3);
    Print.p(",");
    Print.f(acc_f[0], 3);
    Print.p(",");
    Print.f(acc_f[1], 3);
    Print.p(",");
    Print.f(acc_f[2], 3);
    Print.p(",");
    Print.f(degPitch, 3);
     Print.p(",");
    Print.f(kPitch, 3);
    Print.p(",ETX");
    Print.ln();

    Serial.print(txdata);
  }
}
