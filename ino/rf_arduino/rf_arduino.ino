/******************************************************
Arduino program 
******************************************************/

#include <Servo.h>

#define SERVO_X 9
#define SERVO_Y 10
#define SERVO_Z 11

#define SLEEP_TIME 1000
/* Global variables */
typedef struct pt {
  uint8_t _x;
  uint8_t _y;
  uint8_t _z;
} pt_struct;

/* Servos */
Servo servo_x;
Servo servo_y;
Servo servo_z;

/* actual point */
pt_struct *actual_position;

int s_size = (int) sizeof(struct pt);

uint8_t x_pos = 0;
uint8_t y_pos = 0;
uint8_t z_pos = 0;

/**********************
 * void setup() - Initialisations
 ***********************/
void setup() {
  
  //  Setup
  Serial.begin(9600);
  
  //  Attach servos
  servo_x.attach( SERVO_X );
  servo_y.attach( SERVO_Y );
  servo_z.attach( SERVO_Z );
  
  //  Set & move to initial default position
  servo_x.write(x_pos);
  servo_y.write(y_pos);
  servo_z.write(z_pos);
  
}

/******************************************************
Serial read and moving determination
******************************************************/

void loop(){

  if( Serial.available() )  {
    //received data in buffer
    char data_buff[s_size];
    // Serial read
    Serial.readBytes(data_buff, s_size);
    // convert to a point struct
    actual_pos = (pt_struct*)(&data_buff);
    // move the finger
    x_pos = actual_pos->_x;
    y_pos = actual_pos->_y;
    z_pos = actual_pos->_z;
    servo_x.write(x_pos);
    servo_y.write(y_pos);
    servo_z.write(z_pos);
    delay(SLEEP_TIME);
  }
}

