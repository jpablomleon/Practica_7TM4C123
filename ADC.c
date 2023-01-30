/*
 * ADC.c
 *
 *  Created on: 04/11/2019
 *      Author: ravenelco
 */
#include "lib/include.h"

extern void Configura_Reg_ADC0(void)
{
   /*Habilitar el modulo 0 del ADC con dos canales analogicos 
    en el puerto E a una velocidad de conversion de 250ksps
    dandole la mayor prioridad al secuenciador 2 con evento
    de procesador 
    */
     //Pag 396 para inicializar el modulo de reloj del adc RCGCADC
    SYSCTL->RCGCADC = (1<<0); //Se inicializa el modulo 0 del ADC 
    //Pag 382 (RGCGPIO) Se habilitan los puertos que corresponden - Dependiendo de los pines que te tocaron ( me toco 1,3,4,5,11,9 - PUERTOS E, D Y B)
    //                     F     E      D       C      B     A
    SYSCTL->RCGCGPIO |= (0<<5)|(1<<4)|(0<<3)|(0<<2)|(0<<1)|(0<<0);
    //Pag 760 (GPIODIR) Habilta los pines como I/O un cero para entrada y un uno para salida (los ADC SON ENTRADAS - SE PONEN LOS PINES QUE NOS TOCARON)
    GPIOE_AHB->DIR = (0<<0) | (0<<2) | (0<<4); //PIN 0, 2,4  - PUERTO E COMO ENTRADAS
    
    
    //(GPIOAFSEL) pag.770 Habilitar funciones alternativas para que el modulo analógico tenga control de esos pines (PUERTO E, D, B) - SE PONEN EN 1
    GPIOE_AHB->AFSEL =  0x0e;
    
    //(GPIODEN) pag.781 desabilita el modo digital para los puertos E, D y B 
    GPIOE_AHB->DEN &= ~0x0e;
    

 ///faltan otros puertos


    //Pag 787 GPIOPCTL registro combinado con el GPIOAFSEL y la tabla pag 1808
    //GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFF00FFFF); ???????????????????????????

    //(GPIOAMSEL) pag.786 Se habilita función analogica para cada uno de los puertos
    GPIOE_AHB->AMSEL |= 0x0e; //PIN 0, 2,4  - PUERTO E COMO ENTRADAS
    
    //Pag 1159 El registro (ADCPC) establece la velocidad de conversión de un millon por segundo para cada ADC y para cada puerto 
    ADC0->PC = (0<<3)|(1<<2)|(0<<1)|(1<<0);//1 Msps
    
    //Pag 1099 Este registro (ADCSSPRI) configura la prioridad de los secuenciadores
    ADC0->SSPRI = 0x0000;   //Se pone por desault así que la prioridad sera 0, 1,2,3
    
    //Pag 1077 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS  &= ~(0x0f); //SE DESACTIVAN PARA INICIAR LA CONFIGURACIÓN 
    
    //Pag 1091 Este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger)
    ADC0->EMUX  |= 0x0000;  //Se eligio trigger
    
    //Pag 1129 Este registro (ADCSSMUX2) define las entradas analógicas con el canal y secuenciador seleccionado
    ADC0->SSMUX0 |= 0x00000210; //       que canales se van a leer de cada selector -primer muestreo es el ultimo numero (9)

    //pag 868 Este registro (ADCSSCTL2), configura el bit de control de muestreo y la interrupción
    ADC0->SSCTL0 |= 0x00000644;    //AQUI SE LEE EL CANAL Y SE DICE Q

    /* Enable ADC Interrupt */
    ADC0->IM |= (0xf<<16) | (0xf<<0); /* Unmask ADC0 sequence 2 interrupt pag 1082*/

    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;
    //Pag 1077 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS |= (1<<0);


    ADC0->PSSI |= (1<<2); //Se inicializa el muestreo en el secuenciador que se va a utilizar
    
}

extern void ADC_leer_canal(uint16_t data[])
{
 // selector 0
    ADC0 -> PSSI |= (1<<0);
    delay_ms(1);

    while (ADC0 -> RIS & 0x01 == 0);
    delay_ms(1);
    while(ADC0 -> SSOP0 & (1<<0) == (1<<0));
    data[0] = ADC0 -> SSFIFO0 & 0xfff;
    delay_ms(1);
    while(ADC0->SSOP0 & (1<<4) == (1<<4));
    data[1] = ADC0->SSFIFO0 & 0xfff;
    delay_ms(1);
    while(ADC0->SSOP0 & (1<<8) == (1<<8));
    data[2] = ADC0->SSFIFO0 & 0xfff;
    delay_ms(1);

    ADC0->ISC |= (1<<0);
    delay_ms(1);
}



