#include <msp430g2553.h>
#include <legacymsp430.h>

#define BTN BIT3
#define RX BIT1
#define TX BIT2
#define LEDS (BIT0|BIT6)

#define BAUD_9600   0
#define BAUD_19200  1
#define BAUD_38400  2
#define BAUD_56000  3
#define BAUD_115200 4
#define BAUD_128000 5
#define BAUD_256000 6
#define NUM_BAUDS   7


// definições de funções
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


int main(void)
{
  static Estados estado = ESPERA;
  int qtd = 0;
  volatile int i = 0;
  char c;
  WDTCTL = WDTPW + WDTHOLD;
  
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  
  P1OUT = BTN;
  P1REN |= BTN;
  P1DIR &= ~BTN;
  P1OUT &= ~LEDS;
  P1DIR |= LEDS;

  Init_UART(BAUD_9600);
  _BIS_SR(GIE);
  
  while(1) {
    switch (estado) {
      case ESPERA:
        while(caracter != 's'){
          Atraso(10000);
        }
        caracter = '0';
        Send_String("esperei\n");
        estado = LEITURA;
        break;

      case LEITURA:
        while (caracter != 'f') {
          Atraso(10000);
        }
        Send_String("codigo lido\n");
        Send_String(string);
        Send_Data('\n');
        estado = ENVIO;
        break;
        
      case ENVIO:
        while(1);
        estado = LEITURA;
        break;
    }
//        
//      case TERMINO:
//        Serial.println("termino");
//        delay(500);
//        estado = ESPERA;
//        break;
//    }
//---------------------------
  }
  return 0;
}

void Send_Data(unsigned char c)
{
  while((IFG2&UCA0TXIFG)==0);
  UCA0TXBUF = c;
}

void Send_Int(int n)
{
  int casa, dig;
  if(n==0)
  {
    Send_Data('0');
    return;
  }
  if(n<0)
  {
    Send_Data('-');
    n = -n;
  }
  for(casa = 1; casa<=n; casa *= 10);
  casa /= 10;
  while(casa>0)
  {
    dig = (n/casa);
    Send_Data(dig+'0');
    n -= dig*casa;
    casa /= 10;
  }
}

char Read_Char(){
  char c = 's';
  return c;
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
    P1SEL2 = P1SEL = RX+TX;
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

interrupt(USCIAB0RX_VECTOR) Receive_Data(void)
{
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
