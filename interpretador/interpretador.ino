//MOTOR EIXO X (NEMA 17)
#define ENA_1 P2_4
#define ORI_1 P2_5
#define CLK_1 P1_6

//MOTOR EIXO Y (NEMA 23)
#define ENA_2 P2_1
#define ORI_2 P2_0
#define CLK_2 P1_5

// Máquina de estados
typedef enum {
    ESPERA = 0, LEITURA, ENVIO, TERMINO
} Estados;
static Estados estado = ESPERA;
String string;
int qtd = 0;

void setup() {
  Serial.begin(9600);
  //analogFrequency(2000);

  pinMode(ORI_1, OUTPUT);
  pinMode(ENA_1, OUTPUT);
  pinMode(CLK_1, OUTPUT);

  pinMode(ORI_2, OUTPUT);
  pinMode(ENA_2, OUTPUT);
  pinMode(CLK_2, INPUT);

  digitalWrite(ORI_1, HIGH);
  digitalWrite(ORI_2, HIGH);
  digitalWrite(ENA_1, HIGH);
  digitalWrite(ENA_2, HIGH);

  analogWrite(CLK_1, 130);
}

void loop() {
  char caracter;
  
  switch (estado)
  {
    case ESPERA:
      while(Serial.read() != 's'){
        // LOW 
        digitalWrite(ORI_1, LOW);
        digitalWrite(ORI_2, LOW);
        delay(2000);
        digitalWrite(ORI_1, HIGH);
        digitalWrite(ORI_2, HIGH);
        delay(1500);
        Serial.println("espera");
        delay(500);
      }
      estado = LEITURA;
      break;
      
    case LEITURA:
      caracter = Serial.read();
      if (caracter > 0){
        string += caracter;
        qtd++;
      }
      else if (qtd > 0) {
        string += '\0';
        qtd = 0;
        estado = ENVIO;
      }
      else {
        Serial.println("leitura");
        delay(500);
      }
      break;
      
    case ENVIO:
      if(string.startsWith("G0")) {
        Serial.println("envio de um G00");
        estado = LEITURA;
      }
      else if(string.startsWith("G1")) {
        Serial.println("envio de um G01");
        estado = LEITURA;
      }
      else if (string.startsWith("M2")) {
        delay(500);
        estado = TERMINO;
      }
      else {
        estado = LEITURA;
      }
      string = "";
      break;
      
    case TERMINO:
      Serial.println("termino");
      delay(500);
      estado = ESPERA;
      break;
  }
}
