int CLK = 14;
int EN1 = 2;
int EN2 = 3;
int MOV1 = 4;
int MOV2 = 5;

void setup()
{
// //##########################################//
// //####  CONFIGURAÇÃO DO WATCHDOG TIMER  ####//
// //##########################################//
// 
// WDTCTL = WDTPW | WDTHOLD; //WATCHDOG TIMER PARADO
// 
// //##########################################//
// //#######  CONFIGURAÇÃO DO CLOCK  ##########//
// //##########################################//
// 
// DCOCTL = 0;             //
// BCSCTL1 = CALBC1_1MHZ;   //CONFIGURA CLOCK EM 1 MHZ
// DCOCTL = CALDCO_1MHZ;   //
// 
// //##########################################//
// //##########  DECLARAÇÃO DE I/Os  ##########//
// //##########################################//
// 
// P1DIR |= 0x40; //P1.6 IMPLEMENTADO COMO SAÍDA
// P1SEL |= 0x40; //CONFIGURANDO P1.6 COMO PERIFÉRICO TA0.1
// 
// //##########################################//
// //#######  CONFIGURAÇÃO DO TIMER0_A  #######//
// //##########################################//
// 
// TACCR0 = 8000; //PERÍODO DO PWM
// TACCTL1 = OUTMOD_7; //MODO DE SAÍDA DO TIMER0_A: RESET/SET
// TACCR1 = TACCR0/2; //DUTY CYCLE DO PWM EM 50%
// TACTL = TASSEL_2 + MC_1; //TASSEL_2 -> CLOCK SOURCE: MCLK  MC_1 ->                           //TIMER COM CONTAGEM PROGRESSIVA DE 0 ATÉ TACCR1
// 
// _BIS_SR(CPUOFF); //DESLIGA A CPU PARA ECONOMIZAR CONSUMO (LPM0)
 
  pinMode(CLK, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(MOV1, OUTPUT);
  pinMode(MOV2, OUTPUT);
}

void loop()
{
  analogWrite(CLK, 120);
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(MOV1, HIGH);
  digitalWrite(MOV2, HIGH);
//  delay(5000);
//  digitalWrite(MOV1, LOW);
//  digitalWrite(MOV2, LOW);
//  delay(5000);
//  digitalWrite(EN1, LOW);
//  digitalWrite(EN2, LOW);
//  delay(1000);
  
}
