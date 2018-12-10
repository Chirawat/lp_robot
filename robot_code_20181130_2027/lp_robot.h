/* 	Processor :  Arduino Nano
	Compatible with POP-XT
	Last modified: 2018-10-31
	**************************************************
*/

#ifndef lp_robot_h
#define lp_robot_h



#include <Servo.h>
#include <EEPROM.h>
#include <TimerOne.h>
#include <PID_v1.h>


int SPEED_t=80;
int turnPower = 10;
float turnPower_t = 0;
extern int KEEP_DISTANCE=5;
extern int FOUND_DISTANCE	=12;

//Define Variables we'll be connecting to
volatile double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
// 90% PID myPID(&Input, &Output, &Setpoint, 0.017, 0, 0.008, REVERSE);
// 80% PID myPID(&Input, &Output, &Setpoint, 0.1, 0, 0.0125, REVERSE);
// PID myPID(&Input, &Output, &Setpoint, 0.075, 0, 0.001, REVERSE);

int L_Speed, R_Speed;
int UP		=	90;
int DOWN	=	0;
int KEEP_t	=	93;
int RELEASE_t	=	50;

/* REFERENCES */
int     L2_REF=300;
int     L_REF=574;
int     R_REF=580;
int     R2_REF=300;
int     BL_t_REF=300;
int     BR_t_REF=300;

int     L2_MAX=300;
int     L_MAX=574;
int     R_MAX=580;
int     R2_MAX=300;
int     BL_t_MAX=300;
int     BR_t_MAX=300;

int     L2_MIN=300;
int     L_MIN=574;
int     R_MIN=580;
int     R2_MIN=300;
int     BL_t_MIN=300;
int     BR_t_MIN=300;


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
volatile int       L, R, C, L2, R2, BL_t, BR_t;
int       L_min, R_min, C_min, L2_min, R2_min, BL_t_min, BR_t_min;
int       L_max, R_max, C_max, L2_max, R2_max, BL_t_max, BR_t_max;
int       L_avg, R_avg, C_avg, L2_avg, R2_avg, BL_t_avg, BR_t_avg ;
int       error;
int       SW=4;
int       error_front, error_back;

int j = 0;

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
  TCCR2B = 0x00;
  
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

void motor_L(int L_Speed){
	if(L_Speed > 255)
		L_Speed = 255;
	else if(L_Speed < 0)
		L_Speed = 0;
	
	digitalWrite(ADIR, LOW);
	analogWrite(PWMA, L_Speed);
}

void motor_R(int R_Speed){
	if(R_Speed > 255)
		R_Speed = 255;
	else if(R_Speed < 0)
		R_Speed = 0;
	
	digitalWrite(BDIR, LOW);
	analogWrite(PWMB, R_Speed);
}

void fd(int L_Speed, int R_Speed){
  //direction
  digitalWrite(ADIR, LOW);
  digitalWrite(BDIR, LOW);
  // speed
  analogWrite(PWMA, map(R_Speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(L_Speed, 0, 100, 0, 255));
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

void read_sensor(){
  int L2_temp = 0; 
  int L_temp = 0; 
  int R_temp = 0;
  int R2_temp = 0;
  int BL_t_temp = 0;
  int BR_t_temp = 0;
  
  for(int i=0; i<2; i++){
    L_temp   += analogRead(1);
    R_temp   += analogRead(3);
    L2_temp  += analogRead(0);
    R2_temp  += analogRead(2);

    BL_t_temp  += analogRead(5);
    BR_t_temp  += analogRead(4);
    delay(1);
  }
  L2  = L2_temp / 2;
  L   = L_temp / 2;
  R   = R_temp / 2;
  R2  = R2_temp / 2;
  BL_t  = BL_t_temp / 2;
  BR_t  = BR_t_temp / 2;
  
  TCNT2   = 0;           
	TIFR2   = 0x00;  
  TCCR2B  = 0x06;  
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
  if(L < L_REF){
		fd(SPEED_t, SPEED_t*0.875);
	}
	else if(R < R_REF){
		fd(SPEED_t*0.875, SPEED_t);
	}
	else{
		fd(SPEED_t, SPEED_t);
	}
}

void follow_line_t(float factor){
	read_sensor();

	if(error > 0){
		fd(SPEED_t*0.875*factor, SPEED_t);
	}
	else if(error < 0){
		fd(SPEED_t, SPEED_t*0.875*factor);
	}
	else{
		fd(SPEED_t*factor, SPEED_t*factor);
	}
}

void follow_line_bk(){
  read_sensor();
  if(error_back > 0){
    //sr(SPEED_t-turnPower, SPEED_t-turnPower);
	bk(SPEED_t-turnPower, SPEED_t);
	//bk(0, SPEED_t);
  }
  else if(error_back < 0){
    //sl(SPEED_t-turnPower, SPEED_t-turnPower);
	bk(SPEED_t, SPEED_t-turnPower);
	//bk(SPEED_t, 0);
  }
  else{
    bk(SPEED_t, SPEED_t);
  }
}

void FF(){
  int i;
	for(i = 30; i < 100 && L2 > L2_REF && R2 > R2_REF; i++){
		follow_line_t(i/100.0);
	}
	while(L2 > L2_REF && R2 > R2_REF && i >= 100) {
		follow_line();
	}
}

void FDD(){
  fd(SPEED_t* 0.7, SPEED_t* 0.7);
	
  delay(700); ao();

  beep();
}

void FR(){
  FF();
  ao(); 
  beep(); 
  sr(SPEED_t*0.7, SPEED_t*0.7);  
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
}

void FL(){
  FF(); ao(); beep(); 
  //ao(); beep(); delay(1000);
  sl(SPEED_t*0.65, SPEED_t*0.65);  
  while(L2 > L2_REF);
  while(L > L_REF);
  ao(); beep(); delay(1000);
}

void BF(){
  read_sensor();
  while(BL_t > BL_t_REF || BR_t > BR_t_REF){  // detect BL_tack line
    read_sensor();
    follow_line_bk();
  }
  read_sensor();
  while(BL_t < BL_t_REF || BR_t < BR_t_REF){  // during BL_tack line
    read_sensor();
    follow_line_bk();
  }  
  ao();
  beep();
}

void BR(){
  BF();
  sr(SPEED_t, SPEED_t);  
  read_sensor();
  while(R2 > R2_REF){
    read_sensor();
  }
  read_sensor();
  while(R > R_REF){
    read_sensor();
  }
  ao(); beep();
}

void BL(){
  BF();
  sl(SPEED_t, SPEED_t);  
  read_sensor();
  while(L2 > L2_REF){
    read_sensor();
  }
  while(L > L_REF){
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
  
  Serial.println(distance);
  
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

void follow_line2(){
  int sp = (float)(SPEED_t * 0.75);
  int sp_l = (float)(SPEED_t * 0.3);
  read_sensor();
  if(error_front > 0){
    fd(sp, sp_l);
  }
  else if(error_front < 0){
    fd(sp_l, sp);
  }
  else{
    fd(sp, sp);
  }
}

void KEEP(){
  int distance = read_distance();
  while(distance > KEEP_DISTANCE || distance == 0){
	read_sensor();
    distance = read_distance();
	//if(distance !=0)
	follow_line2();
	//else
	
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
  delay(200);
}

void walkUntilFoundObject(){
	int distance = read_distance();
  while(distance > KEEP_DISTANCE || distance == 0){
	read_sensor();
    distance = read_distance();
	if(distance !=0)
		follow_line2();
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
	follow_line();
  }
  ao();
  beep();
  delay(100);
}

void LEEKL(){
	walkUntilFoundObject2();  
	
	sl(SPEED_t,SPEED_t);
	
	delay(200); ao(); delay(200);
	
	fd(0.45 * SPEED_t, SPEED_t);
	
	delay(500); //ao();
	
	//ao(); delay(200);// while(1);
	
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
	
	
	//FF();
}

void LEEKR(){
	walkUntilFoundObject2();  
	
	sr(SPEED_t,SPEED_t);
	
	delay(200); ao(); delay(200);
	
	fd(SPEED_t, 0.45 * SPEED_t);
	
	delay(500); //ao();
	
	read_sensor();
	while(R2 > R2_REF){
		read_sensor();
	}
	sr(SPEED_t * 0.5,SPEED_t * 0.5);
	while(L > L_REF){
		read_sensor();
	}
	ao();
	
	
}

void rw(int L_Speed, int R_Speed){
  //direction
  digitalWrite(ADIR, HIGH);
  digitalWrite(BDIR, LOW);
  // speed
  analogWrite(PWMA, map(R_Speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(L_Speed, 0, 100, 0, 255));
}

void BB(){
  rw(SPEED_t*0.9,SPEED_t*0.9);
	
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

void lp_init() {
	
	/* Communication */
	Serial.begin(2000000);
	
  /* drive motor */
  pinMode(ADIR, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BDIR, OUTPUT);
  pinMode(PWMB, OUTPUT);
  
  analogWrite(PWMA, 128);
  digitalWrite(ADIR, LOW);
  analogWrite(PWMB, 128);
  digitalWrite(BDIR, LOW);

  
  /* ultrasonic */
  pinMode (trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode (echoPin, INPUT); // Sets the echoPin as an Input

  /* SW */
  pinMode(SW, INPUT);
  

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
  
  /* servo */
  udServo.attach(9);
  gripperServo.attach(10);
  
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

      /* Save reference */
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
	  
	  /* Save max value */
      EEPROM.write(20, L2_max);
      EEPROM.write(21, L2_max >> 8);
      EEPROM.write(22, L_max);
      EEPROM.write(23, L_max >> 8);
      EEPROM.write(24, R_max);
      EEPROM.write(25, R_max >> 8);
      EEPROM.write(26, R2_max);
      EEPROM.write(27, R2_max >> 8);
      EEPROM.write(28, BL_t_max);
      EEPROM.write(29, BL_t_max >> 8);
      EEPROM.write(30, BR_t_max);
      EEPROM.write(31, BR_t_max >> 8);
	  
	  /* Save min value */
      EEPROM.write(40, L2_min);
      EEPROM.write(41, L2_min >> 8);
      EEPROM.write(42, L_min);
      EEPROM.write(43, L_min >> 8);
      EEPROM.write(44, R_min);
      EEPROM.write(45, R_min >> 8);
      EEPROM.write(46, R2_min);
      EEPROM.write(47, R2_min >> 8);
      EEPROM.write(48, BL_t_min);
      EEPROM.write(49, BL_t_min >> 8);
      EEPROM.write(50, BR_t_min);
      EEPROM.write(51, BR_t_min >> 8);
      
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
  
  /* Read maximum values from EEPROM */
  int L2_MAXL = EEPROM.read(20);
  int L2_MAXH = EEPROM.read(21);
  L2_MAX = L2_MAXH << 8 | L2_MAXL;
  
  int L_MAXL = EEPROM.read(22);
  int L_MAXH = EEPROM.read(23);
  L_MAX = L_MAXH << 8 | L_MAXL;

  int R_MAXL = EEPROM.read(24);
  int R_MAXH = EEPROM.read(25);
  R_MAX = R_MAXH << 8 | R_MAXL;

  int R2_MAXL = EEPROM.read(26);
  int R2_MAXH = EEPROM.read(27);
  R2_MAX = R2_MAXH << 8 | R2_MAXL;

  int BL_t_MAXL = EEPROM.read(28);
  int BL_t_MAXH = EEPROM.read(29);
  BL_t_MAX = BL_t_MAXH << 8 | BL_t_MAXL;
  
  int BR_t_MAXL = EEPROM.read(30);
  int BR_t_MAXH = EEPROM.read(31);
  BR_t_MAX = BR_t_MAXH << 8 | BR_t_MAXL;

  
  /* Read minimum values from EEPROM */
  int L2_MINL = EEPROM.read(40);
  int L2_MINH = EEPROM.read(41);
  L2_MIN = L2_MINH << 8 | L2_MINL;
  
  int L_MINL = EEPROM.read(42);
  int L_MINH = EEPROM.read(43);
  L_MIN = L_MINH << 8 | L_MINL;

  int R_MINL = EEPROM.read(44);
  int R_MINH = EEPROM.read(45);
  R_MIN = R_MINH << 8 | R_MINL;

  int R2_MINL = EEPROM.read(46);
  int R2_MINH = EEPROM.read(47);
  R2_MIN = R2_MINH << 8 | R2_MINL;

  int BL_t_MINL = EEPROM.read(48);
  int BL_t_MINH = EEPROM.read(49);
  BL_t_MIN = BL_t_MINH << 8 | BL_t_MINL;
  
  int BR_t_MINL = EEPROM.read(50);
  int BR_t_MINH = EEPROM.read(51);
  BR_t_MIN = BR_t_MINH << 8 | BR_t_MINL;
  
  Serial.println("Reference values are...");
  Serial.print("L2=");  Serial.println(L2_REF);
  Serial.print("L=");   Serial.println(L_REF);
  Serial.print("R=");   Serial.println(R_REF);
  Serial.print("R2=");  Serial.println(R2_REF);
  Serial.print("BL=");  Serial.println(BL_t_REF);
  Serial.print("BR_t=");  Serial.println(BR_t_REF);
  Serial.println();
  
  Serial.println("Maximum values are...");
  Serial.print("L2_MAX=");  Serial.println(L2_MAX);
  Serial.print("L_MAX=");   Serial.println(L_MAX);
  Serial.print("R_MAX=");   Serial.println(R_MAX);
  Serial.print("R2_MAX=");  Serial.println(R2_MAX);
  Serial.print("BL_MAX=");  Serial.println(BL_t_MAX);
  Serial.print("BR_MAX=");  Serial.println(BR_t_MAX);
  Serial.println();
  
  Serial.println("Minimum values are...");
  Serial.print("L2_MIN=");  Serial.println(L2_MIN);
  Serial.print("L_MIN=");   Serial.println(L_MIN);
  Serial.print("R_MIN=");   Serial.println(R_MIN);
  Serial.print("R2_MIN=");  Serial.println(R2_MIN);
  Serial.print("BL_MIN=");  Serial.println(BL_t_MIN);
  Serial.print("BR_MIN=");  Serial.println(BR_t_MIN);

  
  // Setpoint = 127;
  // Input = error;

  // myPID.SetOutputLimits(-255, 255);
  //turn the PID on
  // myPID.SetMode(AUTOMATIC);
  
  
	Serial.println("Press key to start...");
	beep();
	sw_ok_press();
	delay(1000);
	Serial.println("Started");

	// Interrupt
	pinMode(13, OUTPUT);    
  
	TCCR2B = 0x00;        //Disbale Timer2 while we set it up
	//TCNT2  = 130;         //Reset Timer Count to 130 out of 255
	TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
	TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
	TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
	TCCR2B = 0x06;        //Timer2 Control Reg B: Timer Prescaler set to 128
}

ISR(TIMER2_OVF_vect) {  // 
	digitalWrite( 13, HIGH );
	read_sensor();  
  // Serial.print(L);
  // Serial.print(",");
  // Serial.println(L2);
	digitalWrite( 13, LOW );
}; 

#endif