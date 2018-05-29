/******************************************************
Arduino program 
******************************************************/

#include <Servo.h>

#define ROTATE 11
#define POINTER 9
#define ARM 10

#define SLEEP_TIME 250
/* Global variables */
typedef struct pt {
  float _r;
  float _p;
  float _a;
} pt_struct;

/* Servos */
Servo servo_rotate;
Servo servo_pointer;
Servo servo_arm;

/* actual point */
pt_struct *actual_position;

int s_size = (int) sizeof(struct pt);

float r_pos = 90;
float p_pos = 45;
float a_pos = 75;

/**********************
 * void setup() - Initialisations
 ***********************/
void setup() {
  
  //  Setup
  Serial.begin(9600);
  
  //  Attach servos
  
  servo_rotate.attach( ROTATE );
  servo_arm.attach( ARM );
  servo_pointer.attach( POINTER );
  delay(1500);
  
  //  Set & move to initial default position
  servo_rotate.write(r_pos);
  
  
  servo_pointer.write(p_pos);
  servo_arm.write(a_pos);
  delay(SLEEP_TIME);
  
}

/******************************************************
Serial read and movement
******************************************************/

void loop(){

  if( Serial.available() )  {
    //received data in buffer
    char data_buff[s_size];
    // Serial read
    Serial.readBytes(data_buff, s_size);
    // convert to a point struct
    actual_position = (pt_struct*)(&data_buff);
    // move the finger
    r_pos = actual_position->_r;
    p_pos = actual_position->_p;
    a_pos = actual_position->_a;

    if(servo_arm.read()<a_pos){
    servo_rotate.write(r_pos);
    delay(SLEEP_TIME);
    servo_pointer.write(p_pos);
    delay(SLEEP_TIME);
    //moveArm(a_pos, a_pos);
    servo_arm.write(a_pos);
    }
    else{
    //moveArm(a_pos, a_pos);
    servo_arm.write(a_pos);
      servo_rotate.write(r_pos);
    delay(SLEEP_TIME);
    servo_pointer.write(p_pos);
    delay(SLEEP_TIME);
    }
    
  }
}

/* Function that controls robotic finger */
void moveArm(float initPos,float endPos) {
  bool flag = 0;
  float time;
    initPos = servo_arm.read();
    if((initPos-endPos)==0){
      time = 1500;
    }
    else{
    time = 1000/abs(initPos-endPos);
    }
  
  for(float i = initPos;i < endPos; i++){
    servo_arm.write(i);
    delay(time);
  }  
  for(float i = initPos;i > endPos; i--){
    servo_arm.write(i);
    delay(time);
  }
}

