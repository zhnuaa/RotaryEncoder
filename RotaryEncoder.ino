#define encoderPinA 2 //A connected to Pin2
#define encoderPinB 3 //B connected to Pin3
volatile long encoderPos = 0; //encoder Counter
byte ALast = 0; //last A state
byte BCurrent = 0; //current B state
unsigned long initTimestamp; //initial timestamp
unsigned long time; //relative time, currentTimestamp-initTimestamp
byte dataPackage[11]; //data package,(header)0xff 0x50 (long)time (long)data (byte)checksum  

//print data to Serial
void printData(unsigned long *time,volatile long *data){ 
  //data package: (header)0xff 0x50 (long)time (long)data (byte)checksum  
  //time
  for(int i=3;i>=0;i--){
    dataPackage[5-i]=byte(*time>>(8*i));
  } 
  //data
  for(int i=3;i>=0;i--){
    dataPackage[9-i]=byte(*data>>(8*i));
  } 
  //checksum=sum{dataPackage[0]...dataPackage[9]}
  dataPackage[10]=0;
  for(int i=0;i<10;i++){
    dataPackage[10]+=dataPackage[i];
  }
  //print dataPackage
  Serial.write(dataPackage,11);
}

void setup() {
  pinMode(encoderPinA, INPUT_PULLUP); 
  pinMode(encoderPinB, INPUT_PULLUP);
//  pinMode(encoderPinA, INPUT); 
//  pinMode(encoderPinB, INPUT);
//  digitalWrite(encoderPinA, HIGH); //激活上拉电阻
//  digitalWrite(encoderPinB, HIGH); //激活上拉电阻
  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);
  // set up the Serial Connection 
  Serial.begin (115200);
  //set data package header
  dataPackage[0]=0xff;
  dataPackage[1]=0x50;
  initTimestamp=micros();
}
void loop(){
  //print data
  time=micros()-initTimestamp;
  printData(&time,&encoderPos);
  delay(200);
}
// Interrupt on A changing state
void doEncoderA(){
  BCurrent^ALast ? encoderPos++:encoderPos--;
  ALast=bitRead(PIND,encoderPinA);
}
// Interrupt on B changing state
void doEncoderB(){
  BCurrent=bitRead(PIND,encoderPinB);
  BCurrent^ALast ? encoderPos++:encoderPos--;
}
