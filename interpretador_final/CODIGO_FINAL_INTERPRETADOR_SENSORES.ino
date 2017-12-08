// EasyKey - Interpretador para corte da chave automática - Gcode vem do processamento de imagem da Raspberry
// Equipe de Eletrônica - Rodrigo B. Renato A. Vinicius L.
// Versão 2.0 - 08/12/17


#include <msp430g2553.h> // Biblioteca para os registradores da MSP430
#include <legacymsp430.h> // Biblioteca para os pinos I/O
#include <intrinsics.h> // Biblioteca que inclui bibliotecas padrões de C

//ORI1 |= vai pra chave
//ORI2 &= vai pra chave 
#define RX            BIT1 // Porta 1 para transmissão serial da Raspberry
#define TX            BIT2 // Porta 1 para transmissão serial da Raspberry

#define BAUD_9600     0 //
#define BAUD_19200    1 //
#define BAUD_38400    2 //
#define BAUD_56000    3 //
#define BAUD_115200   4 // Taxas de transmissão
#define BAUD_128000   5 //
#define BAUD_256000   6 //
#define NUM_BAUDS     7 //

//CLOCK
#define CLK1          BIT6 // Porta 1.6  //----------------------------//
                                         //Fazer curto nessas portas (Mesmo CLK)--//
#define CLK2          BIT0 // Porta 2.0  //--------------------------//

//Driver 1 Nema 17 
#define ORI1          BIT5 // Porta 2.5 // Orientação do motor Nema 17
#define ENA1          BIT4 // Porta 2.4 // Liga/desliga do motor Nema 17

//Driver 2 Nema 23
#define ORI2          BIT1 // Porta 2.1 // Orientação do motor Nema 23
#define ENA2          BIT2 // Porta 2.2 // Orientação do motor Nema 23

//RELE
#define RELE          BIT3 //Porta 2.3 // Porta de acionamento do relê

//Sensores
#define CHAVE1        BIT3 // Porta 1.3 -- Porta da chave 1 ( Eixo do Nema 17)
#define CHAVE2        BIT4 // Porta 1.4 -- Porta da chave 2  (Eixo do Nema 23 - G0)
#define SENSOR_AF     BIT5 // Porta 1.5 -- Porta do SENSOR_AF infravermelho
#define SENSOR_17     BIT0 // Porta 1.0 -- Porta do sensor infravermelho do Nema 17
#define SENSOR_23     BIT7 // Porta 1.7 -- Porta do sensor infravermelho do Nema 23


//#define LED BIT0 //Porta 1

/// definições de funções
void movea(double xval, double yval);
void mover(double x, double y);
void feedrate(int feed);
double atof(char * ptr);
void Send_Data(unsigned char c);
void Send_Int(int n);
void Send_String(char str[]);
void Atraso(volatile unsigned int x);
void Init_UART(unsigned int baud_rate_choice);
void G0();
void M2();
void G1();

// variáveis globais
typedef enum {
    ESPERA = 0, LEITURA, ENVIO, TERMINO
} Estados;
unsigned char caracter = '0';
char string[128];
int i = 0;
int x_pos = 0;
int y_pos = 0;
double xval, yval;
double xval_ant;
double yval_ant;
int t = 0;
int n_passos;

int main(void)
{
  static Estados estado = ESPERA;
  int qtd = 0;
  char * ptr;

  
  WDTCTL = WDTPW + WDTHOLD;
  
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;

  //Timer_A em modo up, com 10000 contagens de 1us,
 //gerando uma interrupção a cada 1 ms (1000*(1/1MHz) = 1ms)
 // Fórmula geral -> (TACCR0*(1/1MHz) = f(out)
 // Limite da do NEMA 17 - 705
 
 TACCR0 = 1000-1; //PERÍODO DO PWM
 TACCTL1 = OUTMOD_7; //MODO DE SAÍDA DO TIMER0_A: RESET/SET
 TACCR1 = TACCR0/2; //DUTY CYCLE DO PWM EM 50%
 TACTL = TASSEL_2 + ID_1 + MC_1; //TASSEL_2 -> CLOCK SOURCE: MCLK  MC_1 ->   //TIMER COM CONTAGEM PROGRESSIVA DE 0 ATÉ TACCR1

  P1IFG = 0x00;
  P1DIR |= CLK1;
  //P1OUT |= LED;
  P1SEL |= CLK1|RX|TX;
  P1SEL2|= RX|TX;
  P1DIR &= ~(CHAVE1|CHAVE2|SENSOR_AF|SENSOR_23|SENSOR_17);
  P1REN |= (CHAVE1|CHAVE2|SENSOR_AF|SENSOR_23|SENSOR_17);
  P1IES |= (CHAVE1|CHAVE2|SENSOR_AF|SENSOR_23|SENSOR_17);
  P1IE |= (CHAVE1|CHAVE2|SENSOR_AF|SENSOR_23|SENSOR_17);

  //ORI1 |= vai para o fim de curso
  //ORI2 &= vai para o fim de curso
  P2DIR |= ENA1|ORI1|ENA2|ORI2|RELE;
  //P2DIR &= ~CLK2;
  //P2OUT |= ENA1|ORI1;  
  P2OUT &= ~(ENA2|ENA1|RELE); 
  Init_UART(BAUD_9600);
  _BIS_SR(GIE);
  
  while(1) {
    //M2();
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
           
            G1();
            Send_String("c");
            memset(&string[0], 0, sizeof(string));
            i = 0;
            estado = LEITURA;
          }
          else {            
            G0();
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
      
        memset(&string[0], 0, sizeof(string));
        i = 0;
        M2();
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
double atof(char * ptr){
  double x = 0;
  double i = 10;
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

//ORI1 |= vai para o fim de curso
//ORI2 &= vai para o fim de curso
interrupt(PORT1_VECTOR) Interrupcao_P1(void) {
 if((P1IN&CHAVE1) == 0){          // NEMA 17
    Atraso(10000);
    //P1OUT &= ~LED;
    P2OUT ^= ORI1;
    __delay_cycles(1300000);
    P2OUT &= ~ENA1;
    P2OUT &= ~RELE;
    Send_String("c");
  }
  if((P1IN&CHAVE2) == 0){         // G0 do NEMA 23
    Atraso(10000);
   // P1OUT &= ~LED;
    P2OUT ^= ORI2;
    __delay_cycles(1000000);
    P2OUT &= ~ENA2;
    P2OUT &= ~RELE;
  }
  if((P1IN&SENSOR_AF) == 0){
    //P1OUT &= ~LED;
    P2OUT &= ~ENA1;                   // Desabilita o LED, o ENA1 e o ENA2
    P2OUT &= ~ENA2;
    P2OUT &= ~RELE;
  }

 
  if((P1IN&SENSOR_17) == 0){         // G0 do NEMA 17
    //P1OUT &= ~LED;
    P2OUT |= ORI1;
    P2OUT |= ENA2;
    __delay_cycles(1250000);
    P2OUT &= ~(ENA2|ORI1);
    P2OUT |= ENA1;
    __delay_cycles(3300000);
    P2OUT &= ~ENA1;
    P2OUT |= RELE;
    __delay_cycles(32000);
    
    Send_String("c");
  }
  if((P1IN&SENSOR_23) == 0){         // G0 do NEMA 23
    //P1OUT &= ~LED;
    P2OUT &= ~ORI2;
    P2OUT |= ENA1;
    P2OUT &= ~(ORI1|ENA2);
  } 
    P1IFG = 0x00;    
}

void G0(){
  P2OUT |= ENA2|ORI2;
  P2OUT &= ~ENA1;
  xval_ant = xval; 
}

void M2(){
  P2OUT |= (ENA1|ENA2|ORI1);
  P2OUT &= ~(ORI2|RELE);

}

void G1(){

  P2OUT |= ENA2;
  P2OUT &= ~ORI2;
if(xval<xval_ant){
  P2OUT |= ENA1;
  P2OUT &= ~ORI1;
}
if(xval==xval_ant)
  P2OUT &= ~ENA1;
if(xval>xval_ant){
  P2OUT |= ENA1;
  P2OUT |= ORI1;
  }
  __delay_cycles(32000);
   P2OUT &= ~(ENA1|ENA2); 
  xval_ant=xval;
}

