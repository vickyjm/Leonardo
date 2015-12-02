#include <AcceleroMMA7361.h>
#include <math.h>
const int ok_led = 2;
const float MATH_PI = 3.14159265359;

int motori=11; /*motor derecha hacia atras*/
int motord=5;/*motor izquierda hacia atras*/
int motoria=3;/*motor derecha hacia delante*/
int motorda=2;/*motor izquierda hacia delante*/
int eD=6; /* pin enable del puente H para el motor derecho*/
int eI=10; /* pin enable para el motor izquierdo */

AcceleroMMA7361 accelero;
double pitch, roll;
int angulo;
int minVel = 60;
int minVelX = 100;

double last_proportionalX = 0;
double integralX = 0;
double derivativeX = 0;
double proportionalX = 0;
double last_proportionalY = 0;
double integralY = 0;
double derivativeY = 0;
double proportionalY = 0;

void adelante(int velI, int velD){

digitalWrite(eD,LOW);
digitalWrite(eI,LOW);
analogWrite(eD,velD); /* Menos de 20 no se mueve y suena raro */
analogWrite(eI,velI);
digitalWrite(motord,LOW);
digitalWrite(motori,LOW);
delay(5);
digitalWrite(motorda,HIGH);
digitalWrite(motoria,HIGH);

}
void atras(int velI, int velD){
  
digitalWrite(eD,LOW);
digitalWrite(eI,LOW);
delay(5);
analogWrite(eD,velD); /* Menos de 20 no se mueve y suena raro */
analogWrite(eI,velI);
digitalWrite(motorda,LOW);
digitalWrite(motoria,LOW);
delay(5);
digitalWrite(motord,HIGH);
digitalWrite(motori,HIGH);


}

void derecha(int velI, int velD){
  
digitalWrite(eD,LOW);
digitalWrite(eI,LOW);
delay(5);
analogWrite(eD,velD); /* Menos de 20 no se mueve y suena raro */
analogWrite(eI,velI);
digitalWrite(motorda,LOW);
digitalWrite(motori,LOW);
delay(5);
digitalWrite(motord,HIGH);
digitalWrite(motoria,HIGH);
}

void izquierda(int velI, int velD){
  
digitalWrite(eD,LOW);
digitalWrite(eI,LOW);
delay(5);
analogWrite(eD,velD); /* Menos de 20 no se mueve y suena raro */
analogWrite(eI,velI);
digitalWrite(motord,LOW);
digitalWrite(motoria,LOW);
delay(5);
digitalWrite(motorda,HIGH);
digitalWrite(motori,HIGH);
}

void parar(){
  
analogWrite(eD,0);
analogWrite(eI,0);

}


void getAngulo2(){
    int x, y ,z;
    
    x = accelero.getXAccel();
    y = accelero.getYAccel();
    z = accelero.getZAccel();
    
    pitch = atan2(x,sqrt(pow(y,2))+pow(z,2));
    roll = atan2(y,sqrt(pow(x,2))+pow(z,2));

    pitch = pitch*180.0/PI;
    roll = roll*180.0/PI;
    /*Serial.println("Pitch");
    Serial.println(pitch);
    Serial.println("Roll");
    Serial.println(roll);*/
}

void setup() {
    pinMode(motord,OUTPUT);
    pinMode(motorda,OUTPUT);
    pinMode(motori,OUTPUT);
    pinMode(motoria,OUTPUT);

    pinMode(eI,OUTPUT);
    pinMode(eD,OUTPUT);
    
    Serial.begin(9600);
    Serial.println("Initiating...");
    // "OK" Led:
    //pinMode(ok_led, OUTPUT);

    //sleepPin = 13, selfTestPin = 12, zeroGPin = 9, gSelectPin = 8
    //xPin = A2, yPin = A1, zPin = A0
    accelero.begin(13, 12, 9, 8, A2, A1, A0);
    accelero.calibrate();
    Serial.println("Initiated");
    

}

/*
 * power_difference > 0 => Atras
 */
void loop()
{
    getAngulo2();  

    proportionalX = pitch + 0.35;
    derivativeX = proportionalX - last_proportionalX;
    integralX = integralX - proportionalX;
    last_proportionalX = proportionalX;
    
    double kpX = 7.42;
    double kiX = 0.742;
    double kdX = 2.42;
    double power_difference;
    int nuevaVel;
    power_difference = proportionalX*kpX + integralX*kiX + derivativeX*kdX;
    //power_difference = proportionalX*kpX + derivativeX*kdX;
    nuevaVel = abs(int(power_difference))+minVelX;
    
    if (power_difference >  255){
      power_difference = 255;
      nuevaVel = 255;
    } else if (power_difference < -255){
      power_difference = -255;
      nuevaVel = 255;
    }
    if (nuevaVel > 255){
      nuevaVel = 255;
    }
    
    if (pitch > 0){
      derecha(nuevaVel,nuevaVel);
      //Serial.println("Derecha");
      delay(10);
    } else if (pitch < -0.70){
      izquierda(nuevaVel,nuevaVel);
      //Serial.println("Izquierda");
      delay(10);
    } else {
      parar();
      //Serial.println("Parar");
    }
    
    proportionalY = roll + 0.35; 
    derivativeY = proportionalY - last_proportionalY;
    //integralY = integralY - proportionalY;
    last_proportionalY = proportionalY;
    
    double kpY = 5.8; //1/10
    double kiY = 0.070; //1/10000
    double kdY = 9.977; //3/2
    power_difference = proportionalY*kpY + integralY*kiY + derivativeY*kdY;
    //double power_difference = proportionalY*kp + derivativeY*kd;
    

    nuevaVel = abs(int(power_difference))+minVel;
    if (power_difference >  255){
      power_difference = 255;
      nuevaVel = 255;
    } else if (power_difference < -255){
      power_difference = -255;
      nuevaVel = 255;
    }

    if (nuevaVel > 255){
      nuevaVel = 255;
    }

    if(roll>-0.15){
       adelante(nuevaVel,nuevaVel);
      //Serial.println("Adelante");
    }else if (roll < -0.5){
      atras(nuevaVel,nuevaVel);
      //Serial.println("Atras");
      //Serial.println(nuevaVel);  
    } else{
      parar();  
      //Serial.println("Detenido");
    }
    //Serial.println(roll);
    delay(50);
    
}

