#define LED RED_LED

char string[128];
int enable = 4;

void setup() {
  Serial.begin(9600);
  pinMode(enable,OUTPUT);
  digitalWrite(enable,LOW);
}

void loop() {
  // Envia vários 's'inais indicando que está pronto pra iniciar.
  // Em python, usar a função serial.read_all() para limpar o buffer
  //   após a comunicação inicial.
  if (Serial.available() <= 0) {
    Serial.print('s');
    delay(500);
  }
  // Fluxo principal depois de estabelecida a comunicação.
  else {
    // Essa leitura será descartada. Faz parte da comunicação inicial.
    int lixo = Serial.read();
    
    int qtd = 0;
    char caracter;

    while (true) {
      // Leitura de caracter por caracter. Quando não há leitura, caracter == -1
      caracter = Serial.read();
      
      // Se houver algum caracter, será adicionado a minha string
      if (caracter > 0){
        string[qtd] = caracter;
        qtd++;
      }
      // Leitura da string finalizada. Ela já possui todos os valores do Gcode.
      //   A chamada do interpretador está aqui.
      else if (qtd > 0) {
        string[qtd+1] = '\0';
        interpretador();
        qtd = 0;
      }
    }
  }
}

// Função do interpretador. Neste momento, a variável global (seria melhor passar ela como parâmetro?)
//   já está preenchida com a linha do Gcode que deve ser executado.
void interpretador() {
//  int i = 0;
//  for (i = 0; string[i] != '\0'; i++) {
//    Serial.write(string[i]);
//  }
  digitalWrite(enable,HIGH);
  delay(5000);
  digitalWrite(enable,LOW);
}
