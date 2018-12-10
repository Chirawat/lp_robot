/* 	Processor :  Arduino Nano
	Compatible with POP-XT
	Last modified: 2018-11-**
	Edit by P'Tam
	**************************************************
*/

#ifndef lp_robot_h
#define lp_robot_h

#include <Servo.h>
#include <EEPROM.h>


extern int     SPEED_t=55;
extern int     turnPower =0;
extern int     KEEP_DISTANCE=5;
extern int     FOUND_DISTANCE  =12;
float          L_Speed, R_Speed;
extern int     SPEED_u=100;

extern int     UP    = 90;
extern int     DOWN  = 0;
extern int     KEEP_t  = 93;
extern int     RELEASE_t = 50;

/* REFERENCES */
int     L2_REF=300;
int     L_REF=574;
int     R_REF=580;
int     R2_REF=300;
int     BL_t_REF=300;
int     BR_t_REF=300;


Servo     udServo, gripperServo; 
bool      serviceMode;
int       ADIR = 7;
int       PWMA = 6;
int       BDIR = 3;
int       PWMB = 5;
const int trigPin = 11;
const int echoPin = 12;
int       buzzer = 8;
long      duration_t;
int       L, R, C, L2, R2, BL_t, BR_t;
int       L_min, R_min, C_min, L2_min, R2_min, BL_t_min, BR_t_min;
int       L_max, R_max, C_max, L2_max, R2_max, BL_t_max, BR_t_max;
int       L_avg, R_avg, C_avg, L2_avg, R2_avg, BL_t_avg, BR_t_avg ;
int       SW=4;

int       error_back;

// PID
float Kp = 35;
float Kd = 0;
float Ki = 0;
float error=0, pre_error = 0, sum_error = 0;
float PID_value;


void read_sensor(){
  int L2_temp = 0; 
  int L_temp = 0; 
  int R_temp = 0;
  int R2_temp = 0;
  int BL_t_temp = 0;
  int BR_t_temp = 0;
  
  for(int i=0; i<5; i++){
    L_temp   += analogRead(1);
    R_temp   += analogRead(3);
    L2_temp  += analogRead(0);
    R2_temp  += analogRead(2);

    BL_t_temp  += analogRead(5);
    BR_t_temp  += analogRead(4);
    delay(1);
  }

  L2  = L2_temp / 5;
  L   = L_temp / 5;
  R   = R_temp / 5;
  R2  = R2_temp / 5;
  BL_t  = BL_t_temp / 5;
  BR_t  = BR_t_temp / 5;

                  // front
 
  if((L>L_REF)&&(R<R_REF))
    error=0.25;
  else if((L>L_REF)&&(R>R_REF))
    error=0;
  else if((L<L_REF)&&(R>R_REF))
    error=-0.25;
  
  // plot
  /*Serial.print(error);
  Serial.print(",");
  Serial.print(PID_value);
  Serial.print(",");
  Serial.print(L_Speed);
  Serial.print(",");
  Serial.println(R_Speed);
  */

                  // back
  if(BL_t > BL_t_REF && BR_t < BR_t_REF ){
    error_back  = -(BR_t_REF - BR_t);
  }  
  else if(BL_t < BL_t_REF && BR_t > BR_t_REF ){
    error_back = (BL_t_REF - BL_t);
  }
  else{
    error_back = 0;
  } 
}

void clearAndHome(){
  Serial.write(27);
  Serial.print("[2J"); // clear screen
  Serial.write(27);
  Serial.print("[H"); // cursor to home
}

void beep(){
  tone(buzzer, 1500);
  delay(50);
  noTone(buzzer);
}

/* Motor Function */
void ao(){
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

void fd(int L_Speed, int R_Speed){
  //direction
  digitalWrite(ADIR, LOW);
  digitalWrite(BDIR, LOW);
  // speed
  analogWrite(PWMA, map(R_Speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(L_Speed, 0, 100, 0, 255));
  
  // Serial.print(error);
  // Serial.print(",");
  // Serial.print(L_Speed);
  // Serial.print(",");
  // Serial.println(R_Speed);

}

void sl(int L_Speed, int R_Speed){
  //direction
  digitalWrite(ADIR, HIGH);
  digitalWrite(BDIR, LOW);
  // speed
  analogWrite(PWMA, map(L_Speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(R_Speed, 0, 100, 0, 255));
}

void sr(int L_Speed, int R_Speed){
  //direction
  digitalWrite(ADIR, LOW);
  digitalWrite(BDIR, HIGH);
  // speed
  analogWrite(PWMA, map(L_Speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(R_Speed, 0, 100, 0, 255));
}

void bk(int L_Speed, int R_Speed){
  //direction
  digitalWrite(ADIR, HIGH);
  digitalWrite(BDIR, HIGH);
  
  // speed
  analogWrite(PWMA, map(L_Speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(R_Speed, 0, 100, 0, 255));
}

void caliBR_tateSensor(){
  clearAndHome();
  read_sensor();
  
  Serial.print("\tL2\tL\tR\tR2\tBL_t\tBR_t\r\n");
  Serial.print("================================================\r\n");
  Serial.print("Actual:\t");
  Serial.print(L2); Serial.print("\t");
  Serial.print(L); Serial.print("\t");
  Serial.print(R); Serial.print("\t");
  Serial.print(R2); Serial.print("\t");
  Serial.print(BL_t); Serial.print("\t");
  Serial.print(BR_t); Serial.print("\r\n");

  if(L2 < L2_min)   L2_min = L2;
  if(L < L_min)     L_min = L;
  if(R < R_min)     R_min = R;
  if(R2 < R2_min)   R2_min = R2;
  if(BL_t < BL_t_min)   BL_t_min = BL_t;
  if(BR_t < BR_t_min)   BR_t_min = BR_t;
  Serial.print("MIN:\t");
  Serial.print(L2_min); Serial.print("\t");
  Serial.print(L_min); Serial.print("\t");
  Serial.print(R_min); Serial.print("\t");
  Serial.print(R2_min); Serial.print("\t");
  Serial.print(BL_t_min); Serial.print("\t");
  Serial.print(BR_t_min); Serial.print("\r\n");

  if(L2 > L2_max)   L2_max = L2;
  if(L > L_max)     L_max = L;
  if(R > R_max)     R_max = R;
  if(R2 > R2_max)   R2_max = R2;
  if(BR_t > BR_t_max)   BR_t_max = BR_t;
  if(BL_t > R2_max)   BL_t_max = BL_t;
  Serial.print("MAX:\t");
  Serial.print(L2_max); Serial.print("\t");
  Serial.print(L_max); Serial.print("\t");
  Serial.print(R_max); Serial.print("\t");
  Serial.print(R2_max); Serial.print("\t");
  Serial.print(BL_t_max); Serial.print("\t");
  Serial.print(BR_t_max); Serial.print("\r\n");

  L2_avg  = (L2_min + L2_max) / 2;
  L_avg   = (L_min + L_max) / 2;
  R_avg   = (R_min + R_max) / 2;
  R2_avg  = (R2_min + R2_max) / 2;
  BL_t_avg  = (BL_t_min + BL_t_max) / 2;
  BR_t_avg  = (BR_t_min + BR_t_max) / 2;
  Serial.print("AVG:\t");
  Serial.print(L2_avg); Serial.print("\t");
  Serial.print(L_avg); Serial.print("\t");
  Serial.print(R_avg); Serial.print("\t");
  Serial.print(R2_avg); Serial.print("\t");
  Serial.print(BL_t_avg); Serial.print("\t");
  Serial.print(BR_t_avg); Serial.print("\r\n");
  
  delay(250);
}

void follow_line(){
  read_sensor();
  if(error > 0){
    fd(SPEED_t*0.875, SPEED_t);
  }
  else if(error < 0){
    fd(SPEED_t, SPEED_t*0.875);
  }
  else{
    fd(SPEED_t, SPEED_t);
  }
}

void follow_line_t(float factor){
	read_sensor();
	if(error > 0){
		fd((SPEED_t*0.875)*factor, SPEED_t);
	}
	else if(error < 0){
		fd(SPEED_t, (SPEED_t*0.875)*factor);
	}
	else{
		fd(SPEED_t*factor, SPEED_t*factor);
	}
}

void follow_line_bk(){
  read_sensor();
  if(error_back > 0){
    bk(SPEED_t-turnPower, SPEED_t);
  }
  else if(error_back < 0){
    bk(SPEED_t, SPEED_t-turnPower);
  }
  else{
    bk(SPEED_t, SPEED_t);
  }
}

void FF_t(){
	int k= 5;
	for(k = 5; k < 50 && L2 > L2_REF && R2 > R2_REF; k++){
		follow_line_t((float)k/50.0);
	}
	while(L2 > L2_REF && R2 > R2_REF && k >= 50) {
		follow_line();
	}
}

void FF_tt(){
	while(L2 > L2_REF && R2 > R2_REF) {
		follow_line();
	}
}
  
void FF(){
  FF_tt(); 
  while(L2 < L2_REF || R2 < R2_REF){
    read_sensor();
  }
}

void FFS(){
  FF_t(); 
    
  while(L2 < L2_REF || R2 < R2_REF) {
		follow_line();
	}
  delay(10);
}

void FDD(){
  fd(SPEED_t* 0.7, SPEED_t* 0.7);
  
  delay(400); ao();

  beep();
}

void FRS(){
  FF_t(); 
  while(L2 < L2_REF || R2 < R2_REF){
    read_sensor();
  }
  ao(); delay(100); 
  sr(SPEED_u*0.45, SPEED_u*0.45);  
  read_sensor();
  while(R2 > R2_REF){
    read_sensor();
  }
  read_sensor();
  while(R > R_REF){
    read_sensor();
  }
  ao(); 
  beep();
  delay(200);
}

void FR(){
  FFS();
  ao(); delay(100);
  bk(0.5, 0.5); ao(); delay(100); 
  sr(SPEED_u*0.45, SPEED_u*0.45);  
  read_sensor();
  while(R2 > R2_REF){
    read_sensor();
  }
  read_sensor();
  while(R > R_REF){
    read_sensor();
  }
  ao(); 
  beep();
  delay(100);
}

void FL(){
  FFS();
  ao(); delay(100);
  bk(0.5, 0.5); ao(); delay(100); 
  sl(SPEED_u*0.45, SPEED_u*0.45);  
  read_sensor();
  while(L2 > L2_REF){
    read_sensor();
  }
  read_sensor();
  while(L > L_REF){
    read_sensor();
  }
  ao(); 
  beep(); 
  delay(100);
}

void FLS(){
  FF_t(); 
  while(L2 < L2_REF || R2 < R2_REF){
    read_sensor();
  }
  ao(); delay(100); 
  sl(SPEED_u*0.45, SPEED_u*0.45);  
  read_sensor();
  while(L2 > L2_REF){
    read_sensor();
  }
  read_sensor();
  while(L > L_REF){
    read_sensor();
  }
  ao(); 
  beep(); 
  delay(200);
}

void BF(){
  read_sensor();
  bk(SPEED_t*0.4, SPEED_t*0.4);
  while(BL_t > BL_t_REF && BR_t > BR_t_REF){  // detect BL_tack line
    read_sensor();
  }
  while(BL_t < BL_t_REF || BR_t < BR_t_REF){  // detect BL_tack line
    read_sensor();
  }
  ao();
  beep();
}

void BR(){
  BF();
  sr(SPEED_u*0.45, SPEED_u*0.45);  
  read_sensor();
  while(R2 > R2_REF){
    read_sensor();
  }
  read_sensor();
  while(R > R_REF){
    read_sensor();
  }
  ao(); 
  beep();
  delay(200);
}

void BL(){
  BF();
  sl(SPEED_u*0.55, SPEED_u*0.55);  
  read_sensor();
  while(L2 > L2_REF){
    read_sensor();
  }
  read_sensor();
  while(L > L_REF){
    read_sensor();
  }
  ao(); 
  beep(); 
  delay(200);
}

void BB(){
  sl(SPEED_u*0.45,SPEED_u*0.45);
  
  delay(150); 
   
  read_sensor();
  while(L2 > L2_REF){
    read_sensor();
  }
  read_sensor();
  while(L > L_REF){
    read_sensor();
  }
  read_sensor();
  while(L < L_REF){
    read_sensor();
  }
  ao(); beep();
  
}

int read_distance(){
int distance;
  
  // Clears the trigPin
  digitalWrite (trigPin, LOW);
  delayMicroseconds (2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite (trigPin, HIGH);
  delayMicroseconds (10);
  digitalWrite (trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration_t = pulseIn (echoPin, HIGH);
  
  // Calculating the distance
  distance = duration_t * 0.034 / 2;
  
  // Serial.println(distance);
  
  return distance;
}

void sw_ok_press(){
  while(digitalRead(SW)){
    delay(100);
  }
}

bool sw_ok(){
  if(digitalRead(SW)) 
    return false;
  else
    return true;
}

void RL(){
  udServo.write(DOWN);
  delay(300);
  
  gripperServo.write(RELEASE_t); 
  delay(300);
  
  udServo.write(UP);
  delay(300);
}

void KEEP(){
  int distance = read_distance();
  
  // increasing speed
  int k= 0;
  for(k = 5; k < 30 && L2 > L2_REF && R2 > R2_REF; k++){
    follow_line_t((float)k/60.0);
  }
  while(distance > KEEP_DISTANCE || distance == 0){
    distance = read_distance();
    read_sensor();
    follow_line_t((float)k/60.0);
  }
  ao();
  beep();
  delay(200);
  
  // Keep procedure
  udServo.write(DOWN);
  delay(200);
  
  gripperServo.write(KEEP_t); 
  delay(200);
  
  udServo.write(UP);
  delay(500);
}

void walkUntilFoundObject(){
  int distance = read_distance();
  while(distance > KEEP_DISTANCE || distance == 0){
  read_sensor();
    distance = read_distance();
  if(distance !=0)
    follow_line();
  else
    follow_line();
  }
  ao();
  beep();
  delay(1000);
}

void walkUntilFoundObject2(){
  int distance = read_distance();
  while(distance > FOUND_DISTANCE || distance == 0){
    read_sensor();
    distance = read_distance();
    //follow_line();
    follow_line_t(0.65);
  }
  ao();
  beep();
  delay(100);
}

void LEEKL(){
  walkUntilFoundObject2();  
  sl(SPEED_t*0.5,SPEED_t*0.5);
  delay(450); ao(); delay(200);
  fd(0.45 * SPEED_t, SPEED_t);
  delay(500); 
  // ao(); delay(200);
  read_sensor();
  while(L2 > L2_REF){
    read_sensor();
  }
  while(L2 < L2_REF){
    read_sensor();
  }
  ao();
  beep(); //delay(1000);

  sl(SPEED_t*0.5, SPEED_t*0.5);  
  read_sensor();
  while(L2 > L2_REF){
  read_sensor();
  }
  while(R > R_REF){
  read_sensor();
  }
  ao(); 
  beep();
}

void LEEKR(){
  walkUntilFoundObject2();  
  sr(SPEED_t*0.5,SPEED_t*0.5);
  delay(450); ao(); delay(200);
  fd(SPEED_t, 0.35 * SPEED_t);
  delay(500); 
  // ao(); delay(200);
  read_sensor();
  while(R2 > R2_REF){
    read_sensor();
  }
  while(R2 < R2_REF){
    read_sensor();
  }
  ao();
  beep(); //delay(1000);

  sr(SPEED_t*0.5, SPEED_t*0.5);  
  read_sensor();
  while(R2 > R2_REF){
  read_sensor();
  }
  while(L > L_REF){
  read_sensor();
  }
  ao(); 
  beep();
}

void FFS_t(){
  int k= 5;
	for(k = 5; k < 50 && L2 > L2_REF && R2 > R2_REF; k++){
		follow_line_t((float)k/100.0);
	}
	while(L2 > L2_REF && R2 > R2_REF && k >= 50) {
		follow_line_t(0.45);
	}
  fd(SPEED_t*0.6, SPEED_t*0.6);
  read_sensor();
  while(L2 < L2_REF || R2 < R2_REF) {
    read_sensor();
	}
  delay(50);
  
}

void RLB(){
  FFS(); ao(); delay(100); 
  BF(); ao(); delay(100); 
  RL(); FFS_t(); ao();
}

void lp_init()
{
  /* drive motor */
 pinMode(PWMA,OUTPUT); //PWM Pin 1
 pinMode(PWMB,OUTPUT); //PWM Pin 2
 pinMode(ADIR,OUTPUT); //Left Motor Pin 1
 pinMode(BDIR,OUTPUT);  //Right Motor Pin 2
 analogWrite(PWMA, 128);
 digitalWrite(ADIR, LOW);
 analogWrite(PWMB, 128);
 digitalWrite(BDIR, LOW);
 
 /* servo */
  udServo.attach(9);
  gripperServo.attach(10);

  /* ultrasonic */
  pinMode (trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode (echoPin, INPUT); // Sets the echoPin as an Input

  /* SW */
  pinMode(SW, INPUT);
  
  /* Communication */
  Serial.begin(9600);

  /* set to initial val */
  ao(); // all motors off
  

  /* Initialize ref min, max */
  read_sensor();
  L2_min  = L2;
  L2_max  = L2;
  L_min   = L;
  L_max   = L;
  R_min   = R;
  R_max   = R;
  R2_min  = R2;
  R2_max  = R2;
  BL_t_min  = BL_t;
  BL_t_max  = BL_t;
  BR_t_min  = BR_t;
  BR_t_max  = BR_t;
  
  udServo.write(UP);
  gripperServo.write(RELEASE_t);

  if(sw_ok()){
    serviceMode = true;
    beep();
    beep();
    Serial.println("Service Mode activated");
    while(sw_ok()) 
      delay(100);
  }
  while(serviceMode){
    caliBR_tateSensor();
    if(sw_ok()){
      beep();
      serviceMode = false;

      /* Store reference */
      EEPROM.write(0, L2_avg);
      EEPROM.write(1, L2_avg >> 8);
      EEPROM.write(2, L_avg);
      EEPROM.write(3, L_avg >> 8);
      EEPROM.write(4, R_avg);
      EEPROM.write(5, R_avg >> 8);
      EEPROM.write(6, R2_avg);
      EEPROM.write(7, R2_avg >> 8);
      EEPROM.write(8, BL_t_avg);
      EEPROM.write(9, BL_t_avg >> 8);
      EEPROM.write(10, BR_t_avg);
      EEPROM.write(11, BR_t_avg >> 8);
      
      while(sw_ok()) 
        delay(100);
    }
  }
  

  /* Read reference values from EEPROM */
  int L2_REFL = EEPROM.read(0);
  int L2_REFH = EEPROM.read(1);
  L2_REF = L2_REFH << 8 | L2_REFL;
  
  int L_REFL = EEPROM.read(2);
  int L_REFH = EEPROM.read(3);
  L_REF = L_REFH << 8 | L_REFL;

  int R_REFL = EEPROM.read(4);
  int R_REFH = EEPROM.read(5);
  R_REF = R_REFH << 8 | R_REFL;

  int R2_REFL = EEPROM.read(6);
  int R2_REFH = EEPROM.read(7);
  R2_REF = R2_REFH << 8 | R2_REFL;

  int BL_t_REFL = EEPROM.read(8);
  int BL_t_REFH = EEPROM.read(9);
  BL_t_REF = BL_t_REFH << 8 | BL_t_REFL;
  
  int BR_t_REFL = EEPROM.read(10);
  int BR_t_REFH = EEPROM.read(11);
  BR_t_REF = BR_t_REFH << 8 | BR_t_REFL;

  
  Serial.println("Reference values are...");
  Serial.print("L2=");  Serial.println(L2_REF);
  Serial.print("L=");   Serial.println(L_REF);
  Serial.print("R=");   Serial.println(R_REF);
  Serial.print("R2=");  Serial.println(R2_REF);
  Serial.print("BL=");  Serial.println(BL_t_REF);
  Serial.print("BR_t=");  Serial.println(BR_t_REF);
  

  
  Serial.println("Press key to start...");
  beep();
  sw_ok_press();
  beep();
  delay(1000);
   
  Serial.println("Started");
}



#endif