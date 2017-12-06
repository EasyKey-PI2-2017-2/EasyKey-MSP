#include <msp430g2553.h>
#include <legacymsp430.h>
#include <intrinsics.h>

#define RX            BIT1 // Porta 1
#define TX            BIT2 // Porta 1

#define BAUD_9600     0
#define BAUD_19200    1
#define BAUD_38400    2
#define BAUD_56000    3
#define BAUD_115200   4
#define BAUD_128000   5
#define BAUD_256000   6
#define NUM_BAUDS     7

#define CLK1           BIT6 // Porta 1.6  //----------------------------//
                                         //Fazer curto nessas portas (Mesmo CLK)--//
#define CLK2          BIT0 // Porta 2.0  //--------------------------//

//Driver 1 Nema 17 
#define ORI1          BIT5 // Porta 2.5
#define ENA1          BIT4 // Porta 2.4

//Driver 2 Nema 23
#define ORI2          BIT1 // Porta 2.1
#define ENA2          BIT2 // Porta 2.2

//RELE
#define RELE          BIT3 //Porta 2.3

//Sensores
#define CHAVE1        BIT3 // Porta 1.3 -- Porta da chave 1 ( Eixo do Nema 17)
#define CHAVE2        BIT4 // Porta 1.4 -- Porta da chave 2  (Eixo do Nema 23 - G0)
#define CHAVE3        BIT7 // Porta 1.7 -- Porta da chave 3 ( Eixo do Nema 23 - Final) 
#define SENSOR        BIT5 // Porta 1.5 -- Porta do sensor infravermelho




#define LED BIT0 //Porta 1

// definições de funções
void movea(float xval, float yval);
void mover(float x, float y);
void feedrate(int feed);
float atof(char * ptr);
void Send_Data(unsigned char c);
void Send_Int(int n);
void Send_String(char str[]);
void Atraso(volatile unsigned int x);
void Init_UART(unsigned int baud_rate_choice);

// variáveis globais
typedef enum {
    ESPERA = 0, LEITURA, ENVIO, TERMINO
} Estados;
unsigned char caracter = '0';
char string[128];
int i = 0;
int x_pos = 0;
int y_pos = 0;


int main(void)
{
  static Estados estado = ESPERA;
  int qtd = 0;
  char * ptr;
  float xval, yval;
  
  WDTCTL = WDTPW + WDTHOLD;
  
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;

  //Timer_A em modo up, com 10000 contagens de 1us,
 //gerando uma interrupção a cada 10 ms (10000*(1/1MHz) = 10ms)

 TACCR0 = 50000-1; //PERÍODO DO PWM
 TACCTL1 = OUTMOD_7; //MODO DE SAdÍDA DO TIMER0_A: RESET/SET
 TACCR1 = TACCR0/2; //DUTY CYCLE DO PWM EM 50%
 TACTL = TASSEL_2 + ID_1 + MC_1; //TASSEL_2 -> CLOCK SOURCE: MCLK  MC_1 ->                           //TIMER COM CONTAGEM PROGRESSIVA DE 0 ATÉ TACCR1

  P1IFG = 0x00;
  P1DIR |= LED|CLK1;
  P1OUT |= LED;
  P1SEL |= CLK1|RX|TX;
  P1SEL2|= RX|TX;
  P1DIR &= ~(CHAVE1|CHAVE2|CHAVE3|SENSOR);
  P1REN |= (CHAVE1|CHAVE2|CHAVE3|SENSOR);
  P1IES |= (CHAVE1|CHAVE2|CHAVE3|SENSOR);
  P1IE |= (CHAVE1|CHAVE2|CHAVE3|SENSOR);

  P2DIR |= ENA1|ORI1|ENA2|ORI2|RELE;
  //P2DIR &= CLK2;
  P2OUT |= ENA1|ORI1|ENA2|ORI2|RELE;
  
   
 
  Init_UART(BAUD_9600);
  _BIS_SR(GIE);
  
  while(1) {
    switch (estado) {
      case ESPERA:
        while(caracter != 's'){
          Atraso(10000);
        }
        caracter = '0';
        estado = LEITURA;
        break;

      case LEITURA:
        while (caracter != 'f') {
          Atraso(10000);
        }
        caracter = '0';
        estado = ENVIO;
        break;
        
      case ENVIO:
        if (string[0] == 'M') {
          estado = TERMINO;
        }
        else if (string[0] == 'G') {
          char * ptr = strtok(string," ");
          
          ptr = strtok(NULL," ");
          xval = atof(ptr+1);
          ptr = strtok(NULL," ");
          yval = atof(ptr+1);
          
          if (string[1] == '1') {
            mover(xval, yval);
            
            Send_String("c");
            memset(&string[0], 0, sizeof(string));
            i = 0;
            estado = LEITURA;
          }
          else {
            mover(xval, yval);
            
            Send_String("c");
            memset(&string[0], 0, sizeof(string));
            i = 0;
            estado = LEITURA;
          }
        }
        else {
          Send_String("e");
          memset(&string[0], 0, sizeof(string));
          i = 0;
          estado = LEITURA;
        }
        break;
    
      case TERMINO:
        Send_String("c");
        memset(&string[0], 0, sizeof(string));
        i = 0;
        Atraso(10000);
        estado = ESPERA;
        break;
    }
  }
  return 0;
}


// ------------------------------------------- //
// FUNCOES DO INTERPRETADOR                    //
// ------------------------------------------- //

void movea(float xval,float yval){
  int dx = xval - x_pos;
  int dy = yval - y_pos;
  mover(dx, dy);
}


void mover(float x, float y){
  short dirx = x / abs(x);
  short diry = y / abs(y);
  x = abs(x);
  y = abs(y);
  if(x == y){
    for(int i = 0; i < x; i++){
      //stepperx.step(dirx * (-1));
      x_pos += dirx;
      //steppery.step(diry);
      y_pos += diry;
    }
  }else if(x > y){
    float acc = 0;
    boolean flag = false;
    for(int i = 0; i < x; i++){
      //stepperx.step(dirx * (-1));
      x_pos += dirx;
      if(flag){
        //steppery.step(diry);
        y_pos += diry;
        flag = false;
      }
      acc += y / x;
      if(acc > 0.5){
        flag = true;
        acc--;
      }
    }    
  }else{
    float acc = 0;
    boolean flag = false;
    for(int i = 0; i < y; i++){
      //steppery.step(diry);
      y_pos += diry;
      if(flag){
        //stepperx.step(dirx * (-1));
        x_pos += dirx;
        flag = false;
      }
      acc += x / y;
      if(acc > 0.5){
        flag = true;
        acc--;
      }
    }
  }
}

void feedrate(int feed) {
  
}

float atof(char * ptr){
  float x = 0;
  float i = 10;
  boolean flag = false;
  boolean m = false;
  while(*ptr != '\0'){
    if(flag){
      if(*ptr < 48 || *ptr > 57){
        ptr++;
        continue;
      }
      x += (*ptr - 48) / i;
      i*=10;
    }else{
      if(*ptr == 46)
        flag = true;
      if(*ptr == 45)
        m = true;
      if(*ptr < 48 || *ptr > 57){
        ptr++;
        continue;
      }
      x *= 10;
      x += (*ptr - 48);
    }
    ptr++;
  }
  if(m)
    x *= (-1);
  return x;
}


// ------------------------------------------- //
// COMUNICAO SERIAL                            //
// ------------------------------------------- //

void Send_Data(unsigned char c)
{
  while((IFG2&UCA0TXIFG)==0);
  UCA0TXBUF = c;
}

void Send_String(char str[])
{
  int i;
  for(i=0; str[i]!= '\0'; i++)
    Send_Data(str[i]);
}

void Atraso(volatile unsigned int x)
{
  while(x--);
}

void Init_UART(unsigned int baud_rate_choice)
{
  unsigned char BRs[NUM_BAUDS] = {104, 52, 26, 17, 8, 7, 3};
  unsigned char MCTLs[NUM_BAUDS] = {UCBRF_0+UCBRS_1,
                    UCBRF_0+UCBRS_0,
                    UCBRF_0+UCBRS_0,
                    UCBRF_0+UCBRS_7,
                    UCBRF_0+UCBRS_6,
                    UCBRF_0+UCBRS_7,
                    UCBRF_0+UCBRS_7};
  if(baud_rate_choice<NUM_BAUDS)
  {
    // Habilita os pinos para transmissao serial UART
    //P1SEL2 = P1SEL = RX+TX;
    // Configura a transmissao serial UART com 8 bits de dados,
    // sem paridade, comecando pelo bit menos significativo,
    // e com um bit de STOP
    UCA0CTL0 = 0;
    // Escolhe o SMCLK como clock para a UART
    UCA0CTL1 = UCSSEL_2;
    // Define a baud rate
    UCA0BR0 = BRs[baud_rate_choice];
    UCA0BR1 = 0;
    UCA0MCTL = MCTLs[baud_rate_choice];
    // Habilita a interrupcao por chegada de dados via UART
    IE2 |= UCA0RXIE;
  }
}

interrupt(USCIAB0RX_VECTOR) Receive_Data(void) {
  unsigned char c = UCA0RXBUF;
  
  if (c == 's') {
    caracter = c;
  }
  else if (c == 'f') {
    caracter = c;
  }
  else if (c == 'n') {
    string[0] = '\0';
    caracter = '0';
    i = 0;
  }
  else {
    string[i++] = c;
  }
}


// ------------------------------------------- //
// INTERRUPCOES                                //
// ------------------------------------------- //

interrupt(PORT1_VECTOR) Interrupcao_P1(void) {
  while((P1IN&CHAVE1) == 0){
    P1OUT &= ~LED;
    P2OUT &= ~ENA1;                   //Desabilita o LED, o ENA1 e o ENA2
    P2OUT &= ~ENA2;
    P2OUT &= ~RELE;
 }

    P1OUT |= LED ;
    P2OUT |= ENA1;                    //Habilita o LED, o ENA1 e o ENA2
    P2OUT |= ENA2;
    P2OUT |= RELE;
      
    P1IFG = 0x00;
}
