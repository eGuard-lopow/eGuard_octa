#include "sensor_sht3x.h"

int res;

//initialize and enable device using the parameters in the header
int init_sht3x(sht3x_dev_t* dev)
{   
    if ((res = sht3x_init(dev, &sht3x_params[0])) != SHT3X_OK) {
        puts("SHT3X: Initialization failed\n");
        printf("res = %d\n",res);
        return 1;
    }
    else {
        puts("SHT3X: Initialization successful\n");
    }
    return 0;
}

static int16_t calculateData(int16_t humidity, float temperature)
{
    //Data from hum and temp should be converted in corresponding 16 bit values:
    //hum = x/100*((2^16)-1): x*655.35    x is percentage of humidity
    //temp = (x + 45)*((2^16)-1)/175  : (x+45)*374.5  x is temperature in celcius
    float temp = temperature + 45.0;
    temp = temp*374.48571;
    int16_t temp16 = (int16_t) temp;
    int16_t hum = humidity*655.35;
    hum = hum & 0xFE00;
    hum = hum + (temp16 >> 7);
    return hum;
}
static uint8_t gencrc(uint8_t *data, size_t len)
{
    uint8_t crc = 0xff;
    size_t i, j;
    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    return crc;
}

int read_sht3x(sht3x_dev_t* dev, int16_t* temp, int16_t* hum)
{
    if ((res = sht3x_read(dev, temp, hum)) == SHT3X_OK) {
        printf("Temperature [°C]: %d.%d\n"
        "Relative Humidity [%%]: %d.%d\n"
        "+-------------------------------------+\n",
        *temp / 100, *temp % 100,
        *hum / 100, *hum % 100);
    }
    else {
        printf("Could not read data from sensor, error %d\n", res);
    }
    return 0;
}

int read_alert_sht3x(sht3x_dev_t* dev, int limit){
    uint8_t data[2];
    if(sht3x_alertmode_read(dev, data,limit) == SHT3X_OK){
        uint8_t hum = (data[0]>>1);
        hum = hum*100 / 0b1111110;
        int16_t temp = data[0] & 0x01;
        temp = temp << 8;
        temp = temp + data[1];
        temp = -45+175*temp/0b111111110;
        printf("High alert set limit humidity: %d, temp: %d\n", hum, temp);
    }
    else {
        printf("Could not read data from sensor, error %d\n", res);
    }

    return 0;
}

int set_alert_sht3x(sht3x_dev_t* dev, int limit, int humidity, int temperature){
    uint16_t data = calculateData(humidity, temperature);
    uint8_t bytes[2] = {data >> 8, data & 0xff};
    uint8_t crc = gencrc(bytes, 2);
    if(sht3x_alertmode_write(dev, limit, data, crc) == SHT3X_OK){
        printf("Limit ");
        switch(limit) {
		case 1	: 
			printf("High set ");
			break;
		case 2 : 
			printf("High clear ");
			break;
		case 3 : 
			printf("Low clear ");
			break;
		case 4 : 
			printf("Low set ");
			break;
		default:
			printf("Error, no number between 1 or 4 given");
	    }	
        printf("set to hum %d and temp %d \n",humidity, temperature);
    }
    else {
        printf("Could not write data to sensor, error %d\n", res);
    }

    return 0;
}

void cb_sht3x(void *arg)
{
    if (arg != NULL) {
        //do something?
    }
    printf("Interrupt from temp sensor\n");
}

void configure_PB15(void) {
    gpio_init_int(GPIO_PIN(PORT_B, 15),GPIO_IN,GPIO_RISING, cb_sht3x, (void*) 0);
    gpio_irq_enable(GPIO_PIN(PORT_B, 15));
}


