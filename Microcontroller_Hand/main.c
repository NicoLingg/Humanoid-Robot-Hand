/*!
*\mainpage
*\author Nico Lingg
*\brief MFM, PLM-Roboterhand -Ansteuerung Motor - SoSe 2015
*\date 30.06.2015
*
*\version 2.0
* FINAL

*
* Dieses Programm wird zur Anstuerung der Roborterhand verwendet - Handseite bzw Motoransteuerung. Es werden durch die UART Schnittstelle (via Bluetooth mit dem Baustein BTM222)
* die Soll-Positionen der einzelnen (5) Motoren uebertragen und durch die Funktion Receive_Automat() ausgelesen. Anschliessend erfolgt die Ansteuerung der Servos fuer die aktuelle
* Position mittels PWM PWM_MatchUpdate().
*
*<b>Fuer die Mainfunktion bitte hier klicken: main()</b>
*
*/


/*********************************************************************
 *																	 *
 *								Includes							 *
 *																	 *
 * *******************************************************************/
#include "lc798x.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_pwm.h"
#include "lpc_types.h"
#include "lpc17xx_libcfg_default.h"
#include <stdio.h>


/*********************************************************************
 *																	 *
 *							DEFINITIONEN							 *
 *																	 *
 * *******************************************************************/
/*------------------------ UART-DEFINITIONEN ------------------------*/

/** UART0 receive data buf */
uint8_t 	rxBuf[16];
uint8_t 	rxZahler=0;

/** UART0 receive zaehler im IRQHandler */
uint32_t    rxCnt	  = 0;

/** UART0 receive flag */
volatile Bool       rxFlag    = FALSE;


/*---------------------- Position-DEFINITIONEN ----------------------*/
/** Position der einzelnen Finger Daumen - Zeigefinger - Mittelfinger - Ringfinger - kleiner Finger*/
uint16_t PositionArray[5]={0,0,0,0,0};

/** Start Position aller Servos zu beginn */
uint32_t Position = 1500;


/*------------------------- ADC-DEFINITIONEN ------------------------*/
uint8_t 	adcBuf[10];


/*------------------- Receive_Automat Zustaende ---------------------*/
enum Receive_States {BB, FF,AA, Daten, Warten};
enum Receive_States az=Warten, fz;



/*------------------------- TypDef-DEFINITIONEN ----------------------*/

/** PWM Variable Definition */
PWM_TIMERCFG_Type PWMCfgDat;

/** PWM Variable Definition */
PWM_MATCHCFG_Type PWMMatchCfgDat;
PINSEL_CFG_Type PinCfg;



/*********************************************************************
 *																	 *
 *						   	 PROTOTYPEN		    					 *
 *																	 *
 * *******************************************************************/
void LCD_Init(void);
void Init_System(void);
void PWM_DualEdge(void);
void Receive_Automat(uint8_t ReceiveValue);
void UART_Inital(void);
void Timer0_DelayMs(uint32_t delayInMs);
void UARTPutDec16(uint16_t decnum);
void UARTPutChar(uint8_t ch);



/*********************************************************************
 *																	 *
 *						    MAIN-FUNKTION					 		 *
 *																	 *
 * *******************************************************************/
/*!
*
*	\brief  <b>Main-Programm<b>:
*
*			Zu Beginn werden alle Initalisierungen durchgefuehrt.
*
*			Hier werden zunaechst alle noetigen Initialisierungen vorgenommen:
*
*			Init_System() - LED auf Board wird auf Ausgang gestellt
*
*			LCD_Init() - LCD-Display wird initialisiert
*
*			PWM_DualEdge() - PWM wird initialisiert
*
*
*			Nach der Initialisierungen wird in der While-Schleife alle 40ms die Position der Servos aktualisiert. Zusaetzlich werden die Postionen an das LCD-Display ausgegeben.
*/
int main(void)
{

	//* System initialisieren
	LCD_Init();

	//* UART Schnittstelle initialisieren
	UART_Inital();

	//* PWM initialisieren
	PWM_DualEdge();


	/* WHILE-SCHLEIFE */
	while(1)
	    {

		//*PWM Update aller Servos - Positionen in PositionArray[] wird in der Funktion Receive_Automat() aktualisiert
		PWM_MatchUpdate(LPC_PWM1, 1, PositionArray[0], PWM_MATCH_UPDATE_NOW);
		PWM_MatchUpdate(LPC_PWM1, 2, PositionArray[1], PWM_MATCH_UPDATE_NOW);
		PWM_MatchUpdate(LPC_PWM1, 3, PositionArray[2], PWM_MATCH_UPDATE_NOW);
		PWM_MatchUpdate(LPC_PWM1, 4, PositionArray[3], PWM_MATCH_UPDATE_NOW);
		PWM_MatchUpdate(LPC_PWM1, 5, PositionArray[4], PWM_MATCH_UPDATE_NOW);


		//*LCD-Display Ausgabe der einzelnen Positionen der Servos
		lcd_set_cursor(0,0);
		UARTPutDec16(PositionArray[0]);
		lcd_set_cursor(6,0);
		UARTPutDec16(PositionArray[1]);
		lcd_set_cursor(12,0);
		UARTPutDec16(PositionArray[2]);
		lcd_set_cursor(18,0);
		UARTPutDec16(PositionArray[3]);
		lcd_set_cursor(24,0);
		UARTPutDec16(PositionArray[4]);

		//*Delay
		Timer0_DelayMs(60);
	    }

}



/*!
*
*	\brief	<b>System-Initialisierung<b>:
*
*			Initialisierung des Boards (Bis jetzt nur LED)
*
*  \param[in]	none
*  \return 		none
*/
void Init_System(void)
{
	// Direction of Pin10 at Port0 (LED)
	LPC_GPIO0->FIODIR |= (1<<10);

}

/*!
*
*	\brief	<b>LCD-Initialisierung<b>:
*
*			Initialisierung des LCD-Displays
*
*   \param[in]	none
*   \return 	none
*/
void LCD_Init(void)
{
	//LCD Schnittstelle vorbereiten
	lcd_init_interface();

	//Den Controller initailisieren, hier fuer Textausgabe mit dem internen Zeichensatz
	lcd_set_system(LCD_FONT_INTERNAL,6,8);

	//Den Cursor einstellen (Falls benoetigt)
	lcd_set_cursor_mode(LCD_CURSOR_OFF, 0);
	lcd_clear_screen();
	lcd_control(LCD_ON);

}


/*!
*
*	\brief	<b>PWM-Initialisierung<b>:
*
*			Initialisierung des PWM-Signals
*
*   \param[in]	none
*   \return 	none
*/
void PWM_DualEdge(void)
{
	uint8_t temp;

	/* PWM block section -------------------------------------------- */

	/* Initialize PWM peripheral, timer mode
	 * PWM prescale value = 1 (absolute value - tick value)
     */
	PWMCfgDat.PrescaleOption = PWM_TIMER_PRESCALE_USVAL;
	PWMCfgDat.PrescaleValue = 1;
	PWM_Init(LPC_PWM1, PWM_MODE_TIMER, (void *) &PWMCfgDat);

	/*
	 * Initialize PWM pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	for (temp = 0; temp < 5; temp++)
	{
		PinCfg.Pinnum = temp;
		PINSEL_ConfigPin(&PinCfg);
	}
	/* Set match value for PWM match channel 0 = 100, update immediately */
	PWM_MatchUpdate(LPC_PWM1, 0, 20000, PWM_MATCH_UPDATE_NOW);
	/* PWM Timer/Counter will be reset when channel 0 matching
	 * no interrupt when match
	 * no stop when match
	 */
	PWMMatchCfgDat.IntOnMatch = DISABLE;
	PWMMatchCfgDat.MatchChannel = 0;
	PWMMatchCfgDat.ResetOnMatch = ENABLE;
	PWMMatchCfgDat.StopOnMatch = DISABLE;
	PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);
	/* Configure each PWM channel: --------------------------------------------- */
	/* - Channel 2: Double Edge
	 * - Channel 4: Double Edge
	 * - Channel 5: Single Edge
	 * The Match register values are as follows:
	 * - MR0 = 2000 (PWM rate)
	 * - MR1 = 0, MR2 = 78 (PWM2 output)
	 * - MR3 = 53, MR4 = 27 (PWM4 output)
	 * - MR5 = 65 (PWM5 output)
	 * PWM Duty on each PWM channel:
	 * - Channel 2: Set by match 1, Reset by match 2.
	 * - Channel 4: Set by match 3, Reset by match 4.
	 * - Channel 5: Set by match 0, Reset by match 5.
	 */

	/* Edge setting ------------------------------------ */

	//PWM_ChannelConfig(LPC_PWM1, 6, PWM_CHANNEL_SINGLE_EDGE);
	PWM_ChannelConfig(LPC_PWM1, 5, PWM_CHANNEL_SINGLE_EDGE);
	PWM_ChannelConfig(LPC_PWM1, 4, PWM_CHANNEL_SINGLE_EDGE);
	PWM_ChannelConfig(LPC_PWM1, 3, PWM_CHANNEL_SINGLE_EDGE);
	PWM_ChannelConfig(LPC_PWM1, 2, PWM_CHANNEL_SINGLE_EDGE);


	/* Match value setting ------------------------------------ */


	PWM_MatchUpdate(LPC_PWM1, 5, 1500, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 4, 1500, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 3, 1500, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 2, 1500, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 1, 1500, PWM_MATCH_UPDATE_NOW);

	/* Match option setting ------------------------------------ */
	PWMMatchCfgDat.IntOnMatch = DISABLE;
	PWMMatchCfgDat.MatchChannel = temp;
	PWMMatchCfgDat.ResetOnMatch = DISABLE;
	PWMMatchCfgDat.StopOnMatch = DISABLE;
    for(temp = 0; temp < 6; temp++)
    {
    	PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);
    }
	/* Enable PWM Channel Output ------------------------------------ */

	PWM_ChannelCmd(LPC_PWM1, 5, ENABLE);
	/* Channel 5 */
	PWM_ChannelCmd(LPC_PWM1, 4, ENABLE);
	/* Channel 4 */
	PWM_ChannelCmd(LPC_PWM1, 3, ENABLE);
	/* Channel 5 */
	PWM_ChannelCmd(LPC_PWM1, 2, ENABLE);
	/* Channel 4 */
	PWM_ChannelCmd(LPC_PWM1, 1, ENABLE);
	/* Channel 4 */


	/* Reset and Start counter */
	PWM_ResetCounter(LPC_PWM1);
	PWM_CounterCmd(LPC_PWM1, ENABLE);

	/* Start PWM now */
	PWM_Cmd(LPC_PWM1, ENABLE);

}


/*!
*
*	\brief	<b>UART-Initialisierung<b>:
*
*			Initailisiert UART0 mit 57600 Baut, 8 Bit, keine Parity, 1 Stopbit
*
*   \param[in]	none
*   \return 	none
*/
void UART_Inital(void){


	  PINSEL_CFG_Type pin_cfg={ /*pinsel config*/
	      .Funcnum      = 2,//1
	      .Portnum      = 0,
	      .Pinmode      = PINSEL_PINMODE_PULLUP,
	      .OpenDrain    = PINSEL_PINMODE_NORMAL,
	  };

	  UART_CFG_Type uart_cfg={ /*UART config*/
	      .Baud_rate    = 19200,
	      .Databits     = UART_DATABIT_8,
	      .Parity       = UART_PARITY_NONE,
	      .Stopbits     = UART_STOPBIT_1,
	  };

	  /* UART FIFO configuration Struct variable */
	  	UART_FIFO_CFG_Type UARTFIFOConfigStruct;

	  /*setup tx*/
	  pin_cfg.Pinnum  = 0;
	  PINSEL_ConfigPin(&pin_cfg);

	  /*setup rx*/
	  pin_cfg.Pinnum  = 1;
	  PINSEL_ConfigPin(&pin_cfg);


	  /*Initialize uart*/
	  UART_Init(LPC_UART3, &uart_cfg);


	  UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	  UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV2;	/* 8 Bytes */
	  	/* Initialize FIFO for UART0 peripheral */
	  UART_FIFOConfig(LPC_UART3, &UARTFIFOConfigStruct);


	  	/** config and enable interrupter */
	  UART_IntConfig(LPC_UART3, UART_INTCFG_RBR, ENABLE);

	  	/** NVIC enable interrupter , it's request */
	  NVIC_EnableIRQ(UART3_IRQn);


	  UART_TxCmd(LPC_UART3, ENABLE);

}




/*!
*
*	\brief		<b>Receive Automat(<b>:
*
*				In dieser Funktion wird beim Empfang eines Datenpakets ein Automaten durchlaufen und nach der Serie 0xBB, 0xFF, 0xAA die Daten in das PositionArray[] geschrieben.
*
*	\param[in]	ReceiveValue
*	\return 	none
*/
void Receive_Automat(uint8_t ReceiveValue){




		fz=az; 				// Zustand merken (az=aktueller Zustand)
		switch (az) 		// Verzweigung in den passenden Codeteil
		{
			case Warten:			if (ReceiveValue == 0xBB) fz=BB;
									break;


			case BB:
									if (ReceiveValue == 0xFF) fz=FF;
									break;

			case FF:
									if (ReceiveValue == 0xAA) fz=Daten;rxZahler=0;
									break;

			case AA:
									fz=Daten;
									rxZahler=0;
									break;

			case Daten:
									if(rxZahler>10) {

										PositionArray[0]=((uint16_t)adcBuf[0]<< 8 ) | adcBuf[1];
										PositionArray[1]=((uint16_t)adcBuf[2]<< 8 ) | adcBuf[3];
										PositionArray[2]=((uint16_t)adcBuf[4]<< 8 ) | adcBuf[5];
										PositionArray[3]=((uint16_t)adcBuf[6]<< 8 ) | adcBuf[7];
										PositionArray[4]=((uint16_t)adcBuf[8]<< 8 ) | adcBuf[9];


										fz=Warten;
										break;
									}


									adcBuf[rxZahler++]=ReceiveValue;


									break;

			default: 				// Undefinierter Zustand (Fehler!)
									break;
		}
		// Zustandswechsel
		az=fz;
}





/*!
*
*	\brief		<b>16 Bit Variable Ausgabe <b>:
*
*				Gibt eine 16 Bit Variable an die UART Schnittstelle aus
*
*	\param[in]	16Bit Variable
*	\return 	none
*/
void UARTPutDec16(uint16_t decnum)
{
	uint8_t c1=decnum%10;
	uint8_t c2=(decnum/10)%10;
	uint8_t c3=(decnum/100)%10;
	uint8_t c4=(decnum/1000)%10;
	uint8_t c5=(decnum/10000)%10;


    UARTPutChar(c5 + '0');
    UARTPutChar(c4 + '0');
    UARTPutChar(c3 + '0');
    UARTPutChar(c2 + '0');
    UARTPutChar(c1 + '0');
    //UART_Send((LPC_UART_TypeDef *)LPC_UART0, '\r', 1, BLOCKING);

    //* zusaetzliche LCD-Ausgabe

    printf("%i", c5);
    printf("%i", c4);
    printf("%i", c3);
    printf("%i", c2);
    printf("%i", c1);
    printf("\n\r");
}

/*!
*
*	\brief		<b>Char zu UART<b>:
*
*				Ausgabe eines Chars an die UART Schnittstelle aus
*
*	\param[in]	Char Variable
*	\return 	none
*/
void UARTPutChar(uint8_t ch)
{
	 UART_Send((LPC_UART_TypeDef *)LPC_UART0, &ch, 1, BLOCKING);
}




/*!
*
*	\brief		<b>Warte-Funktion mit Timer0<b>:
*
*				Wartefunktion mit Timer0
*
*	\param[in]	Wartezeit in ms
*	\return 	none
*/
void Timer0_DelayMs(uint32_t delayInMs)
{

	LPC_TIM0->TCR = 0x02;		/* reset timer */
	LPC_TIM0->PR  = 0x00;		/* set prescaler to zero */

	LPC_TIM0->MR0 = delayInMs * (25000000 / 1000-1);

	//LPC_TIM0->MR0 = (100000000 / 4) / (1000/delayInMs);  /* enter delay time */
	LPC_TIM0->IR  = 0xff;		/* reset all interrrupts */
	LPC_TIM0->MCR = 0x04;		/* stop timer on match */
	LPC_TIM0->TCR = 0x01;		/* start timer */

	/* wait until delay time has elapsed */
	while (LPC_TIM0->TCR & 0x01);

 }










/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/

/*!
*
*	\brief		<b>UART3 interrupter<b>:
*
*				Interrupt bei empfangenden Zeichen an der UART3 Schnittstelle. Es wird die Funktion Receive_Automat(ReceiveValue) aufgerufen und dort die Positionen der Servos aktualisiert .
*
*	\param[in]	none
*	\return 	none
*/
void UART3_IRQHandler(void)
{
	uint8_t ReceiveValue;
	uint8_t i;
	uint32_t IIRValue;

	rxCnt = 0;
	IIRValue = UART_GetIntId(LPC_UART3);

	while ((IIRValue & 0x01) == 0){ /* if have another interrupter hand on */
		switch (IIRValue & 0x0E) {	/* flag */
		case UART_IIR_INTID_RDA :
			for (i = 0; i < 8; i++) {
				rxFlag = TRUE;
				ReceiveValue = UART_ReceiveByte(LPC_UART3);
				rxBuf[rxCnt++] = ReceiveValue;
				Receive_Automat(ReceiveValue);
			}
			break;
		case UART_IIR_INTID_CTI:
			rxFlag = TRUE;
			ReceiveValue = UART_ReceiveByte(LPC_UART3);
			rxBuf[rxCnt++] = ReceiveValue;
			Receive_Automat(ReceiveValue);
			break;
		default:
			break;
		}
		IIRValue = UART_GetIntId(LPC_UART3);
	}


}
