
#ifndef  _BME280_H_
#define _BME280_H_

typedef struct {
	void (*init)(unsigned char csb);
	void (*read)(double *temp, double *humi, double *press);
}t_BME280;

extern const t_BME280 bme280;


#endif // _BME280_H_

