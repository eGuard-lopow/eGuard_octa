//Include driver and driver parameters
#include "sensor_lsm303agr.h"

//initialize and enable device using the parameters in the header
int init_lsm303agr(LSM303AGR_t* dev, int cm)
{   
    int res = LSM303AGR_init(dev, &LSM303AGR_params[0]);
    res += LSM303AGR_enable(dev);
    res += LSM303AGR_enable_interrupt(dev, cm);
    if (res == 0){
        puts("LSM303AGR: Initialization successful\n"); //should initialize using 2 options: unrespectfull: 10Hz, 0x02 duration, 40cm; 10Hz, 0x03 duration, 90cm
    } else {
        puts("LSM303AGR: Initialization failed\n");
    }
    return 0;
}

//Read the current state of the accelerometer
//check tests folder for use of the drivers! 
int read_lsm303agr(LSM303AGR_t* dev)
{
    LSM303AGR_3d_data_t acc_value;
    LSM303AGR_read_acc(dev, &acc_value);
    if (LSM303AGR_read_acc(dev, &acc_value) == 0) {
        printf("Accelerometer x: %i y: %i z: %i\n", 
        acc_value.x_axis,
        acc_value.y_axis,
        acc_value.z_axis);
    }
    else {
        puts("\nFailed reading accelerometer values\n");
    }

    LSM303AGR_3d_data_t mag_value;
    if (LSM303AGR_read_mag(dev, &mag_value) == 0) {
        printf("Magnetometer x: %i y: %i z: %i\n", mag_value.x_axis,
        mag_value.y_axis,
        mag_value.z_axis);
    }
    else {
        puts("\nFailed reading magnetometer values\n");
    }

    return 0;
}



