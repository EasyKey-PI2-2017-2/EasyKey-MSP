int clk = 3;
int ena = 4;
int ori = 5;


void setup() {
  pinMode(clk,OUTPUT);
  pinMode(ena,OUTPUT);
  pinMode(ori,OUTPUT);
}

void loop() {
  analogWrite(clk,100);
  digitalWrite(ena,HIGH);
  digitalWrite(ori,HIGH);
}

