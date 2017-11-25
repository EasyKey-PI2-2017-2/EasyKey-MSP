// Máquina de estados
typedef enum {
    ESPERA = 0, LEITURA, ENVIO, TERMINO
} Estados;
static Estados estado = ESPERA;
String string;
int qtd = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  char caracter;
  
  switch (estado)
  {
    case ESPERA:
      while(Serial.read() != 's'){
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
