const int SENSOR = 0;
const int CHAVE_1 = 4;
const int CHAVE_2 = 5;

void setup() 
{
  //pinMode(SENSOR, INPUT);
  //pinMode(CHAVE_1, INPUT_PULLUP);
  //pinMode(CHAVE_2, INPUT_PULLUP);
  pinMode(0, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop()
{
    //digitalWrite(CHAVE_1, HIGH);
    //delay(1000);
    //digitalWrite(CHAVE_2, HIGH);
    digitalWrite(0, HIGH);
    digitalWrite(6, HIGH);
    delay(500);
    digitalWrite(0, LOW);
    digitalWrite(6, LOW);
    delay(500);
     
}
