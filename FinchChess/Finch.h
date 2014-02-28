#ifndef FINCH_H
#define FINCH_H

/** Sets the API for use with Mac or Linux, comment out if using Windows */
//#define _LINUX_

/** If using Linux/Mac, redefines sleep to match that OS call */
#ifdef _LINUX_
#define Sleep(mm)  usleep(mm*1000)
#endif

/**
 *  Fin_init(void).
 *  Initializes the interface to the finch robot and
 *  launches a background thread to prevent the finch from timing out.
 *  MUST be called prior to all other finch functions
 *
 *  @return -1 if failure
 */
int Fin_Init(void);

/**
 *  Fin_Exit(void).
 *  Sends Finch back to idle mode and
 *  closes the connection.
 *
 *  @return -1 if failure
 */
int Fin_Exit(void);

/**
 *  Fin_Motor(tenth, left, right).
 *  Set the speed (and duration) of the wheels.
 *  Use 0,0,0 to stop. To set the wheels to spin continuously,
 *  set tenth to -1. This function does not block further program
 *  execution. Use Fin_Move for that.
 *
 *  @param tenth motor on time (in tenths of a second)
 *  @param left speed of left wheel (-255 to 255)
 *  @param right speed of right wheel (-255 to 255)
 *
 *  @return -1 if failure
 */
int Fin_Motor(int tenth, int left, int right);

/** Fin_Move(tenth, left, right).
 *  Set the speed (and duration) of the wheels, and block the program
 *  from further execution until time is up. Useful for dancing programs.
 *
 *  @param tenth motor on time (in tenths of a second)
 *  @param left speed of left wheel (-255 to 255)
 *  @param right speed of right wheel (-255 to 255)
 *
 *  @return -1 if failure
 */
int Fin_Move( int tenth, int left, int right );

/**
 *  Fin_Speed(*left, *right).
 *  Get the current speed of the wheels.
 *
 *  @param *left pointer to return speed of left wheel
 *  @param *right pointer to return speed of right wheel
 *  @return -1 if failure
 */
int Fin_Speed(int *left, int *right);

/**
 *  Fin_LED(red, green, blue).
 *  Set the color and intensity of the beak LED
 *
 *  @param red intensity of the red portion of the beak LED (0-255, 0 is off)
 *  @param green intensity of the green portion of the beak LED (0-255, 0 is off)
 *  @param blue intensity of the blue portion of the beak LED (0-255, 0 is off)
 *
 *  @return -1 if failure
 */
int Fin_LED(int red, int green, int blue);

/**
 *  Fin_Buzzer(msec, freq).
 *  Turn on the buzzer.
 *  Use 0,0 to turn the buzzer off
 *
 *  @param msec duration in msecs
 *  @param freq frequency in hz
 *
 *  @return -1 if failure
 */
int Fin_Buzzer(int msec,int freq);

/**
 *  Fin_Lights(*left, *right).
 *  Get light sensor data. Returned values range from 255 to 0 (0=dark)
 *
 *  @param *left pointer for left light sensor data
 *  @param *right pointer for right light sensor data
 *
 *  @return -1 if failure
 */
int Fin_Lights(int *left, int *right);

/**
 *  Fin_Obstacle(*left, *right).
 *  Get obstacle sensor data. Returned value is 1 or 0 (0=no obstacle).
 *
 *  @param *left pointer for left obstacle sensor data
 *  @param *right pointer for right obstacle sensor data
 *
 *  @return -1 if failure
 */
int Fin_Obstacle(int *left, int *right);

/**
 *  Fin_Temp(*temp).
 *  Get temperature sensor data
 *
 *  @param *temp pointer to return the temperature; returned value is in celsius (in 1/1000 units)
 *
 *  @return -1 if failure
 */
int Fin_Temp(float *temp);

/**
 *  Fin_Accel(*x, *y, *z, *tap, *shake).
 *  Get acceleration values and tap/shaken flags. Returned acceleration values are in 'g' (in 1/1000 units) and range from +1.5 to -1.5g.
 *
 *  @param *x pointer for the x-axis acceleration
 *  @param *z pointer for the y-axis acceleration
 *  @param *y pointer for the z-axis acceleration
 *  @param *tap pointer to return the tap flag (0 = not tap)
 *  @param *shaken pointer to return the shaken flag (0 = not shaken)
 *
 *  @return -1 if failure
 */
int Fin_Accel(float *x, float *y, float *z,int *tap, int *shake);


#ifdef _LINUX_
int kbhit(void);
#endif

#endif  /* FINCH_H */
