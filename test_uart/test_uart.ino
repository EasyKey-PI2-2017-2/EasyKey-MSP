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

//Driver 1 Nema 17
#define ENA1          BIT5 // Porta 2
#define ORI1          BIT4 // Porta 2

//Driver 2 Nema 23
#define ENA2          BIT0 // Porta 2
#define ORI2          BIT1 // Porta 2

//RELE
#define RELE          BIT3 //Porta 2

//Sensores
#define CHAVE1        BIT3 // Porta 1
#define CHAVE2        BIT6 // Porta 1
#define CHAVE3        BIT4 // Porta 1
#define CHAVE4        BIT7 // Porta 1
#define SENSOR        BIT5 // Porta 1

#define LED BIT0 //Porta 1

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
  
  WDTCTL = WDTPW + WDTHOLD;
  
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  
  P1OUT |= LED;
  P1DIR |= LED;
  P1DIR &= ~(CHAVE1|CHAVE2|CHAVE3|CHAVE4|SENSOR);
  P1REN |= CHAVE1|CHAVE2|CHAVE3|CHAVE4|SENSOR;
  P1IES |= CHAVE1|CHAVE2|CHAVE3|CHAVE4|SENSOR;
  P1IE |= CHAVE1|CHAVE2|CHAVE3|CHAVE4;
  
  P2OUT |= ENA1|ORI1|ENA2|ORI2|RELE;
  P2DIR |= ENA1|ORI1|ENA2|ORI2|RELE; 
 
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
          if (string[1] == '1') {
            Send_String("c");
            memset(&string[0], 0, sizeof(string));
            i = 0;
            estado = LEITURA;
          }
          else {
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
//---------------------------
  }
  return 0;
}

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
