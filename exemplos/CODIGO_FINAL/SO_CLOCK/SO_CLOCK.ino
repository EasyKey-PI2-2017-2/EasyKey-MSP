/*
 ====================================b========================================
 Name        : PI2.c
 Author      : EasyKey
 Version     :
 Copyright   : Your copyright notice
 Description : Codigo EasyKey
 ============================================================================
 */

#include <msp430.h> 
#include <legacymsp430.h>
#include <intrinsics.h>



//Driver 1 Nema 17
#define CLK1          BIT6 // Porta 1
#define ENA1          BIT4 // Porta 2
#define ORI1          BIT5 // Porta 2

//Driver 2 Nema 23
#define CLK2          BIT5 // Porta 1 (Entrada vindo do CLK1) 
#define ENA2          BIT1 // Porta 2
#define ORI2          BIT0 // Porta 2

//RELE
#define RELE          BIT3 //Porta 2

//Sensores
#define SENSOR        BIT2 // Porta 1
#define CHAVE1        BIT3 // Porta 1
#define CHAVE2        BIT4 // Porta 1
#define CHAVE3        BIT7 // Porta 1

#define LED BIT0 //Porta 1

int main(void) {
 
 //##########################################//
 //####  CONFIGUCRAÇÃO DO WATCHDOG TIMER  ####//
 //##########################################//
 
 WDTCTL = WDTPW | WDTHOLD; //WATCHDOG TIMER PARADO
 
 //##########################################//
 //#######  CONFIGURAÇÃO DO CLOCK  ##########//
 //##########################################//
 
 BCSCTL1 = CALBC1_1MHZ;   //CONFIGURA CLOCK EM 1 MHZ
 DCOCTL = CALDCO_1MHZ;   //
 
 //##########################################//
 //#######  CONFIGURAÇÃO DO TIMER0_A  #######//
 //##########################################//
 
 //Timer_A em modo up, com 10000 contagens de 1us,
 //gerando uma interrupção a cada 10 ms (10000*(1/1MHz) = 10ms)
 
 TACCR0 = 50000-1; //PERÍODO DO PWM
 TACCTL1 = OUTMOD_7; //MODO DE SAdÍDA DO TIMER0_A: RESET/SET
 TACCR1 = TACCR0/2; //DUTY CYCLE DO PWM EM 50%
 TACTL = TASSEL_2 + ID_1 + MC_1; //TASSEL_2 -> CLOCK SOURCE: MCLK  MC_1 ->                           //TIMER COM CONTAGEM PROGRESSIVA DE 0 ATÉ TACCR1
 
 //##########################################//
 //##########  DECLARAÇÃO DE I/Os  ##########//
 //##########################################//

 P1DIR |= CLK1|LED;
 P1DIR &= ~(CHAVE1|CHAVE2|CHAVE3|CLK2|SENSOR);
 P1SEL |= CLK1; //CONFIGURANDO P1.6 COMO PERIFÉRICO TA0.1
 P1OUT |= LED;
 P1REN |= CHAVE1|CHAVE2|CHAVE3|SENSOR;
 P1IES |= CHAVE1|CHAVE2|CHAVE3|SENSOR;
 P1IE |= CHAVE1|CHAVE2|CHAVE3|SENSOR;

 P2DIR |= ENA1|ENA2|RELE|ORI1|ORI2;
 P2OUT |= ENA1|ENA2|RELE|ORI1|ORI2; 
 _BIS_SR(GIE);
 
 for(;;)
 {}
 
 return 0; 
}

interrupt(PORT1_VECTOR) Interrupcao_P1(void) {
  while((P1IN&CHAVE1) == 0){
    P1OUT &= ~LED;
    P2OUT &= ~ENA1;                   //Desabilita o LED, o ENA1 e o ENA2
    P2OUT &= ~ENA2;
    P2OUT &= ~RELE;
  }
  while((P1IN&CHAVE2) == 0){
    P1OUT &= ~LED;
    P2OUT &= ~ENA1;                   //Desabilita o LED, o ENA1 e o ENA2
    P2OUT &= ~ENA2;
    P2OUT &= ~RELE;
  }
  while((P1IN&CHAVE3) == 0){
    P1OUT &= ~LED;
    P2OUT &= ~ENA1;                   //Desabilita o LED, o ENA1 e o ENA2
    P2OUT &= ~ENA2;
    P2OUT &= ~RELE;
  }
  while((P1IN&SENSOR) == 0){
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
