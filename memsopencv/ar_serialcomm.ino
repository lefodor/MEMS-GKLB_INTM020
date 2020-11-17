// combine servo and digital input

#include <Servo.h>
Servo myservo;

int servoPin = 3;
int buttonApin = 7;
int buttonBpin = 9;
int v=90;

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;
String receivedString ;

void setup() 
{
  Serial.begin(9600);
  //Serial.println("hello world");
  pinMode(servoPin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);//INPUT_PULLUP);  
  pinMode(buttonBpin, INPUT_PULLUP);//INPUT_PULLUP);  

  myservo.attach(3);
  v=90;
  myservo.write(v);// no move
}

void loop() 
{   
  if (digitalRead(buttonApin) == HIGH && digitalRead(buttonBpin) == HIGH)
  {
    v=90 ;
    
    // get angle from COM3    
    recvWithEndMarker2() ;

    // move servo
    myservo.write(v);

    // clean buffer
    while(Serial.available()){Serial.read();}
    
    delay(500) ;
  }
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
   
    while (Serial.available() > 0 /*&& newData == false*/) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            //newData = true;
            v=atoi(receivedChars);
        }
    }
}
void recvWithEndMarker2() {
  while (Serial.available() > 0)
  {
    unsigned int outp ;
    //receivedString = Serial.readStringUntil('\n');
    receivedString = Serial.readString();
    char* c_arr=new char[receivedString.length()];
    receivedString.toCharArray(c_arr,255);
    //c_arr[receivedString.length()+1] = '\n' ;
    outp=atoi(c_arr);
    delete[] c_arr ;
    v=outp ;
  }
}

void showNewData() {
    if (newData == true) {
        //Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}
