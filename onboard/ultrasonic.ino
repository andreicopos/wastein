void us(int n){
  long duration;
  float distance;

  if(n==1){//ultrasonic 1 t5E6L0
      //trigger the sensor
      digitalWrite(5, LOW);
      delayMicroseconds(2);
      digitalWrite(5, HIGH);
      delayMicroseconds(10);
      digitalWrite(5, LOW);
      //compute the distance from the pulse duration of echo pin
      duration = pulseIn(6, HIGH, 30000); // timeout 30ms
      distance = duration * 0.0343 / 2;
      if (distance<= 20) {
          Serial.println("Full of waste.\n");
          digitalWrite(0, HIGH);
      } else {
          Serial.print("Empty!\n");
          digitalWrite(0, LOW);
      }
      duration = 0;
      distance = 0;
  }   
  if(n==2){//ultrasonic 2 T7E8L1
      //trigger the sensor
      digitalWrite(7, LOW);
      delayMicroseconds(2);
      digitalWrite(7, HIGH);
      delayMicroseconds(10);
      digitalWrite(7, LOW);
      //compute the distance from the pulse duration of echo pin
      duration = pulseIn(8, HIGH, 30000); // timeout 30ms
      distance = duration * 0.0343 / 2;
      if (distance<= 20) {
          Serial.println("Full of waste.\n");
          digitalWrite(1, HIGH);
      } else {
          Serial.print("Empty!\n");
          digitalWrite(1, LOW);
      }
      duration = 0;
      distance = 0;
  } 
  if(n==3){//ultrasonic 3 T9E10L2
      //trigger the sensor
      digitalWrite(9, LOW);
      delayMicroseconds(2);
      digitalWrite(9, HIGH);
      delayMicroseconds(10);
      digitalWrite(9, LOW);
      //compute the distance from the pulse duration of echo pin
      duration = pulseIn(10, HIGH, 30000); // timeout 30ms
      distance = duration * 0.0343 / 2;
      if (distance<= 20) {
          Serial.println("Full of waste.\n");
          digitalWrite(2, HIGH);
      } else {
          Serial.print("Empty!\n");
          digitalWrite(2, LOW);
      }
      duration = 0;
      distance = 0;
  }   
  if(n==4){//ultrasonic 4 T20E21L3
      //trigger the sensor
      digitalWrite(20, LOW);
      delayMicroseconds(2);
      digitalWrite(20, HIGH);
      delayMicroseconds(10);
      digitalWrite(20, LOW);
      //compute the distance from the pulse duration of echo pin
      duration = pulseIn(21, HIGH, 30000); // timeout 30ms
      distance = duration * 0.0343 / 2;
      if (distance<= 20) {
          Serial.println("Full of waste.\n");
          digitalWrite(3, HIGH);
      } else {
          Serial.print("Empty!\n");
          digitalWrite(3, LOW);
      }
      duration = 0;
      distance = 0;
  } 
}

void setup() {
  Serial.begin(115200);
  //pinMode Ultrasonics
  pinMode(5, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, OUTPUT);
  pinMode(8, INPUT);
  pinMode(9, OUTPUT);
  pinMode(10, INPUT);
  pinMode(20, OUTPUT);
  pinMode(21, INPUT);
  //pinMode LEDs
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  Serial.println("Ultrasonic Test Started...");
}

void loop() {
  // Send a short 10us trigger pulse
  for(int i=1;i<=4;i++){
    us(i);
  }
  delay(1000);
}
