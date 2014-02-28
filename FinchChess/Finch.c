#if 0

Command/Response Format:

All commands to the Finch are 9 bytes long:

     Byte 0:  is always 0x00.
     Byte 1:  is an Ascii character
              determines the command for the Finch to execute
  Bytes 2-7:  parametric data in binary
     Byte 8:  sequence number

All responses are 8 bytes long:

  Bytes 0-6:  sensor data in binary
     Byte 7:  sequence number (from byte 8 of the command)


Ascii Command Codes:

 - 'O' - control full-color LEDs (Orbs)
         Byte 2 = Red Intensity (0-255)
         Byte 3 = Green Intensity (0-255)
         Byte 4 = Blue Intensity (0-255)

 - 'M' - control the velocity of the motors
         Byte 2 = Left Wheel Direction (0:forward, 1:reverse)
         Byte 3 = Left Wheel Speed (0-255)
         Byte 4 = Right Wheel Direction (0:forward, 1:reverse)
         Byte 5 = Right Wheel Speed (0-255)
         *All zeroes will immediately turn the motors off

 - 'B' - sets the buzzer to chirp for a period of time
         Byte 2 = Duration in msec (MSB)
         Byte 3 = Duration in msec (LSB)
         Byte 4 = Frequency in Hz (MSB)
         Byte 5 = Frequency in Hz (LSB)
         *All zeroes will immediately turn the buzzer off

 - 'T' - gets the temperature
         returns
         Byte 0 = Temperature Value (0-255)
                   Celsius = (value-127) / 2.4 + 25.0

 - 'L' - gets the values from the two light sensors
         returns
         Byte 0 = Left Light Sensor (0-255)
         Byte 1 = Right Light Sensor (0-255)

 - 'A' - gets the accelerometer values of the X, Y, Z axis, and the tap/shake byte.
         returns
         Byte 0 = 153 (decimal)
         Byte 1 = X-axis (0-63)
         Byte 2 = Y-axis (0-63)
         Byte 3 = Z-axis (0-63)
                  if value is 0x00 to 0x1f (positive)
                     g-force = value * 1.5/32.0
                  if value is 0x20 to 0x3f (negative)
                     g-force = (value-64) * 1.5/32.0
         Byte 4 = Tap/Shaken flag (0-255)
                  If bit 7 (0x80) is a 1, then the Finch has been shaken since the last read
                  If bit 5 (0x20) is a 0, then the Finch has been tapped since the last read

 - 'I' - gets the values of the two obstacle sensors
         returns
         Byte 0 = Left Sensor (0:none or 1:obstacle present)
         Byte 1 =  Right Sensor (0:none or 1:obstacle present)

 - 'X' - set all motors and LEDs to off

 - 'R' - turns off the motor and
         has the Finch go back into color-cycling mode

 - 'z' - send a counter value showing the number of times command-z has been sent
         used for "keep-alive" or communication tests only
         returns
         Byte 0 = Counter (0-255)

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Finch.h"

#ifdef _LINUX_
#include <pthread.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "hidapi.h"

/* to be used with Fin_Cmnd */
#define SEND       0                // command does not have a response
#define SEND_RECV  1                // response is expected

/* Global Variables */
static hid_device *finch_handle;    // The handle to communicate with the Finch
static int cmnd_count = 0;          // number of commands that have been sent
static int left_speed = 0;
static int right_speed = 0;
static int time_to_stop = 0;

/* local prototypes */
#ifdef _LINUX_
void *Fin_Thread(void *arg);
void *KeyThread(void *arg);
#else
void Fin_Thread(void *arg);
#endif
int Fin_Cmnd(int flag, char cmnd, unsigned char *buffer);

/**  Fin_init(void).
 *  initializes the interface to the finch robot
 *  launches a background thread to prevent the finch from timing out
 *  *Must be called prior to all other finch functions
 *
 *  input:
 *     none
 *  returns:
 *     -1 if failure
 */
int Fin_Init(void)
{
#ifdef _LINUX_
    pthread_t tid;
#else
    HANDLE thread_res;
    DWORD tid;
#endif
    int res;

    // open a connection to the finch
    // the Finch communicates using the USB HID protocol
    // with a VID of 2354 (Hex) and a PID of 1111 (Hex)
    finch_handle = hid_open(0x2354, 0x1111, NULL);
    if (finch_handle == 0)
    {
        // failure...
        printf("Unable to connect to the Finch\n");
        res = -1;
    }
    else
    {
        // success... turn off the beak led
        Fin_LED(0,0,0);

        // create a keep-alive thread
#ifdef _LINUX_
        /* create independent thread to monitor the console */
        pthread_create( &tid, NULL, KeyThread, (void *)0 );

        pthread_create( &tid, NULL, Fin_Thread, (void *)0 );
#else
        thread_res = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Fin_Thread,(LPVOID)0,0,(LPDWORD)&tid);
#endif
        res = 0;
    }

    return(res);
}


/**  Fin_Exit(void).
 *  go back to idle mode
 *  close the connection
 *
 *  input:
 *     none
 *  returns:
 *     -1 if failure
 */
int Fin_Exit(void)
{
    unsigned char IoBuffer[9];
    int res;

    // reset the Finch to idle mode
    res = Fin_Cmnd(SEND,'R',IoBuffer);
    hid_close(finch_handle);
    return(res);
}


/*
 * background (keep-alive) thread
 */
#ifdef _LINUX_
void *Fin_Thread(void *arg)
#else
void Fin_Thread(void *arg)
#endif
{
    unsigned char IoBuffer[9];
    int res;
    int save;
    int count = 0;

    // The Finch has a time-out where it will go back to passive color cycling mode
    // if no commands are sent within a five second time frame.
    while(1)
    {
        // cmnd_count gets incremented each time something is sent to the finch
        save = cmnd_count;

        // pause for 1/10 second
        Sleep(100);
        if (finch_handle == 0)
            break;

        if (time_to_stop > 0)
        {
            // motors running, see if we should stop
            if (--time_to_stop <= 0)
            {
                Fin_Motor(0,0,0);
             	count = 0;
                continue;
            }
        }

        // see if any commands went out over the last second
        if (save != cmnd_count)
        {
            count = 0;
            continue;
        }

        // wait for 2 seconds of no commands before sending keep-alive
        if (++count < 20)
            continue;

        // request the command count (for keep-alive)
        res = Fin_Cmnd(SEND_RECV,'z',IoBuffer);
        count = 0;
    }
}


/*
 * send/recv messages to the finch
 */
int Fin_Cmnd(int flag, char cmnd, unsigned char *buffer)
{
    static unsigned char seq_num = 0;
    int res = 0;

    // the background thread uses this flag
    cmnd_count++;

    // all finch commands have a leading 0
    // followed by an ascii command character
    // and for commands with a response, insert a sequence number
    buffer[0] = 0x00;
    buffer[1] = cmnd;
    if (flag == SEND_RECV)
        buffer[8] = ++seq_num;

	while(res == 0)
	{
		res = hid_write(finch_handle, buffer, 9);
	}

    while (res > 0 && flag == SEND_RECV)
    {
        // read back from the finch
        res = hid_read(finch_handle, buffer, 9);
        // make sure the sequence number matches what was sent
        if (cmnd == 'z' || buffer[7] == seq_num)
           break;
    }

    return(res);
}


/**  Fin_Motor(tenth, left, right).
 *  set the speed (and duration) of the wheels
 *
 *  input:
 *     int tenth = motor on time (in tenths of a second)
 *     int left/right = speed of each wheel
 *     must be between +255 and -255
 *     positive values are forward, negative is reverse
 *     use 0,0 to stop
 *     use -1 for time if you want to sets the wheels to a certain speed without turning them off at
 *     a predetermined time (example: Fin_Motor(-1, 200, -200))
 *  returns
 *     -1 if failure
 */
int Fin_Motor(int tenth, int left, int right)
{
    unsigned char IoBuffer[9];
    char leftDir = 0;
    char rightDir = 0;
    int res;

    // save the motor speed in global variables
    time_to_stop = 0;
    left_speed = left;
    right_speed = right;

    // check for motor stop
    if (left == 0 && right == 0)
        tenth = 0;


    // If the numbers are negative, set the direction bit to 1,
    // and make the negative speed positive
    if (left < 0)
    {
        left = -left;
        leftDir = 1;
    }
    if (right < 0)
    {
        right = -right;
        rightDir = 1;
    }

    // set the direction and speed for each motor
    IoBuffer[2] = leftDir;
    IoBuffer[3] = (char)left;
    IoBuffer[4] = rightDir;
    IoBuffer[5] = (char)right;

    res = Fin_Cmnd(SEND,'M',IoBuffer);
    if (res > 0 && tenth > 0)
    {
        // have the background thread stop the motors
        time_to_stop = tenth;
    }

    return(res);
}


/**  Fin_Move(tenth, left, right).
 *  set the speed (and duration) of the wheels, and block the program
 *  from further execution until time is up
 *
 *  input:
 *     int tenth = motor on time (in tenths of a second), and block time
 *     int left/right = speed of each wheel
 *     must be between +255 and -255
 *     positive values are forward, negative is reverse
 *  returns
 *     -1 if failure
 */
int Fin_Move( int tenth, int left, int right )
{
   int toReturn = Fin_Motor( tenth, left, right );
   while (1 && toReturn != -1)
   {
      Fin_Speed( &left, &right );
      if (left == 0 && right == 0)
         break;
   }
   return toReturn;
}

/**  Fin_Speed(*left, *right).
 *  get the current speed of the wheels
 *
 *  input:
 *     int *left/*right = pointer where to return the speed of each wheel
 *  returns
 *     -1 if failure
 */
int Fin_Speed(int *left, int *right)
{
    *left = left_speed;
    *right = right_speed;
    return(1);
}


/**  Fin_LED(red, green, blue).
 *  set the color and intensity of the beak LED
 *
 *  input:
 *     int red/green/blue = intensity of each color
 *     must be between +255 and 0 (0=off)
 *  returns
 *     -1 if failure
 */
int Fin_LED(int red, int green, int blue)
{
    unsigned char IoBuffer[9];
    int res;

    // set the intensity of each led
    // use 0,0,0 for led off
    IoBuffer[2] = (char)red;
    IoBuffer[3] = (char)green;
    IoBuffer[4] = (char)blue;

    res = Fin_Cmnd(SEND,'O',IoBuffer);
    return(res);
}


/**  Fin_Buzzer(msec, freq).
 *  turn on the buzzer
 *
 *  input:
 *     int msec = duration in msecs
 *     int freq = frequency in hz
 *     use 0,0 to turn the buzzer off
 *  returns
 *     -1 if failure
 */
int Fin_Buzzer(int msec,int freq)
{
    unsigned char IoBuffer[9];
    int res;

    // set the duration and frequencies
    // use 0,0 for buzzer off
    IoBuffer[2] = (char)(msec >> 8);
    IoBuffer[3] = (char)(msec);
    IoBuffer[4] = (char)(freq >> 8);
    IoBuffer[5] = (char)(freq);

    res = Fin_Cmnd(SEND,'B',IoBuffer);
    return(res);
}


/**  Fin_Lights(*left, *right).
 *  get light sensor data
 *
 *  input:
 *     int *left/*right = pointer where to return the light sensor data
 *     returned values range 255 to 0 (0=dark)
 *  returns
 *     -1 if failure
 */
int Fin_Lights(int *left, int *right)
{
    unsigned char IoBuffer[9];
    int res;

    *left = 0;
    *right = 0;

    // request left/right sensors
    res = Fin_Cmnd(SEND_RECV,'L',IoBuffer);

    if (res > 0)
    {
        *left = (int)IoBuffer[0];
        *right = (int)IoBuffer[1];
    }
    return(res);
}


/**  Fin_Obstacle(*left, *right).
 *  get obstacle sensor data
 *
 *  input:
 *     int *left/*right = pointer where to return the obstacle flags
 *     returned value is 1 or 0 (0=no obstacle)
 *  returns
 *     -1 if failure
 */
int Fin_Obstacle(int *left, int *right)
{
    unsigned char IoBuffer[9];
    int res;

    *left = 0;
    *right = 0;

    // get left/right obstacle sensors
    res = Fin_Cmnd(SEND_RECV,'I',IoBuffer);

    if (res > 0)
    {
        *left = (int)IoBuffer[0];
        *right = (int)IoBuffer[1];
    }
    return(res);
}


/**  Fin_Temp(*temp).
 *  get temperature sensor data
 *
 *  input:
 *     float *temp = pointer where to return the temperature
 *     returned value is in celsius (in 1/1000 units)
 *  returns
 *     -1 if failure
 */
int Fin_Temp(float *temp)
{
    unsigned char IoBuffer[9];
    int res;

    *temp = 0.0;

    // request temperature data
    res = Fin_Cmnd(SEND_RECV,'T',IoBuffer);

    if (res > 0)
    {
        // convert the data to celsius
        *temp = (float)(IoBuffer[0] - 127) / 2.4 + 25;
    }
    return(res);
}


/**  Fin_Accel(*x, *y, *z, *tap, *shake).
 *  get acceleration values and tap/shaken flags
 *
 *  input:
 *     float *x/*y/*z = pointer where to return the acceleration for each axis
 *     returned value is in 'g' (in 1/1000 units) can be positive or negative
 *     int *tap/*shake = pointer where to return the tap/shaken flags
 *     returned value is 1 or 0 (0=not tap, not shaken)
 *  returns
 *     -1 if failure
 */
int Fin_Accel(float *x, float *y, float *z, int *tap, int *shake)
{
    float table[3] = {0.0, 0.0, 0.0};
    unsigned char IoBuffer[9];
    int res,ofst,data;

    *tap = 0;
    *shake = 0;

    // request sensor information
    res = Fin_Cmnd(SEND_RECV,'A',IoBuffer);

    if (res > 0)
    {
        // Convert the raw accelerometer data to G-forces
        for (ofst=0; ofst<3; ofst++)
        {
            data = (int)IoBuffer[ofst+1];
            if (data > 31)
                data -= 64;

            table[ofst] = (float)data * 1.5 / 32.0;
        }
        *x = table[0];
        *y = table[1];
        *z = table[2];

        // check the tap/shake sensors
        *tap = IoBuffer[4] & 0x20 ? 1 : 0 ;
        *shake = IoBuffer[4] & 0x80 ? 1 : 0 ;
    }

    return(res);
}

static char _inpbuf[80];

#ifdef _LINUX_
static int _key = 0;

/* secondary thread to block waiting for a keypress */
void *KeyThread(void *arg)
{
   while (1)
   {
     usleep(1000);
     gets(_inpbuf);
     _key = 1;
   }
}

int kbhit(void)
{
   fflush(stdout);
   return(_key);
}
#endif

/* CheckForKey(void)
 *    check for a keypress: combination of kbhit and getch
 *
 * input:
 *    none
 * return:
 *    0 = no keyboard entry
 *    else, Ascii character from the keyboard
 */
char *CheckForInput(void)
{
   /* see if a key has been pressed */
   if (kbhit() == 0)
      return(0);

#ifdef _LINUX_
   _key = 0;
#else
   gets(_inpbuf);
#endif
   return(_inpbuf);
}


