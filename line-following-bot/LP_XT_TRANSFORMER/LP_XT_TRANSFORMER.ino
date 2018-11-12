#include <Servo.h>
#include <EEPROM.h>

int     SPEED_t=65;
int     turnPower = 0;

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
int       error;
int       SW=4;
int       error_front, error_back;

void clearAndHome()
{
  Serial.write(27);
  Serial.print("[2J"); // clear screen
  Serial.write(27);
  Serial.print("[H"); // cursor to home
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
  
  for(int i=0; i<5; i++){
    L_temp   += analogRead(0);
    R_temp   += analogRead(2);
    L2_temp  += analogRead(4);
    R2_temp  += analogRead(3);

    BL_t_temp  += analogRead(1);
    BR_t_temp  += analogRead(5);
    delay(1);
  }
  L2  = L2_temp / 5;
  L   = L_temp / 5;
  R   = R_temp / 5;
  R2  = R2_temp / 5;
  BL_t  = BL_t_temp / 5;
  BR_t  = BR_t_temp / 5;

  // front
  if(L > L_REF && R < R_REF ){
    error_front = -(R_REF - R);
  }  
  else if(L < L_REF && R > R_REF ){
    error_front = (L_REF - L);
  }
  else{
    error_front = 0;
  }

  // back
  if(BL_t > BL_t_REF && BR_t < BR_t_REF ){
    error_back = -(BR_t_REF - BR_t);
  }  
  else if(BL_t < BL_t_REF && BR_t > BR_t_REF ){
    error_back = (BL_t_REF - BL_t);
  }
  else{
    error_back = 0;
  }
  
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
  if(error_front > 0){
    fd(SPEED_t, SPEED_t-turnPower);
  }
  else if(error_front < 0){
    fd(SPEED_t-turnPower, SPEED_t);
  }
  else{
    fd(SPEED_t, SPEED_t);
  }
}

void follow_line_bk(){
  read_sensor();
  if(error_back > 0){
    sr(SPEED_t-turnPower, SPEED_t-turnPower);
  }
  else if(error_back < 0){
    sl(SPEED_t-turnPower, SPEED_t-turnPower);
  }
  else{
    bk(SPEED_t, SPEED_t);
  }
}

void FF(){
  read_sensor();
  while(L2 > L2_REF && R2 > R2_REF){  // detect BL_tack line
    read_sensor();
    follow_line();
  }
  //Serial.println("Detect intersection");
  read_sensor();
  while(L2 < L2_REF || R2 < R2_REF){  // during BL_tack line
    read_sensor();
    fd(SPEED_t-turnPower, SPEED_t-turnPower);
  }  
  //Serial.println("Out of intersection");
  ao();
  beep();
}

void FR(){
  FF();
  sr(SPEED_t-turnPower, SPEED_t-turnPower);  
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

void FL(){
  FF();
  sl(SPEED_t-turnPower, SPEED_t-turnPower);  
  read_sensor();
  while(L2 > L2_REF){
    read_sensor();
  }
  while(L > L_REF){
    read_sensor();
  }
  ao(); beep();
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
    bk(SPEED_t-turnPower, SPEED_t-turnPower);
  }  
  ao();
  beep();
}

void BR(){
  BF();
  sr(SPEED_t-turnPower, SPEED_t-turnPower);  
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
  sl(SPEED_t-turnPower, SPEED_t-turnPower);  
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

  return distance;
}

void beep(){
  tone(buzzer, 1500);
  delay(50);
  noTone(buzzer);
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



void setup() {
  /* drive motor */
  pinMode(ADIR, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BDIR, OUTPUT);
  pinMode(PWMB, OUTPUT);
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
  udServo.write(0);
  gripperServo.write(0);

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
  

  SPEED_t = map(analogRead(6), 0, 1023, 0, 100);
  turnPower = map(analogRead(7), 0, 1023, 0, SPEED_t);

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
  sw_ok_press();
  delay(1000);
  
  Serial.println("Started");
  udServo.write(80);
  gripperServo.write(30); 
  
  //BF();
}

void loop() {
  FF(); FR(); FF(); FR(); FF(); BL();
  FR(); FF(); FR(); FR(); FL(); FL(); FR(); FR();
}
