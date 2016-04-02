/*!
*\mainpage
*\author Nico Lingg
*\brief MFM, PLM-Roboterhand - Ansteuerung ADC - SoSe 2015
*\date 30.06.2015
*
*
*\version 2.0
* FINAL
*\version 1.0
* Erstes funktionierendes Programm.
*\version 0.6
* AD-Wandler implementier mit Schnittstelle UART.
*\version 0.5
* Toten Code entfernt.
*\version 0.4
* Kommentare hinzugefuegt (Doxygen).
*\version 0.3
* Motoransteurung (PWM) implementiert
*\version 0.2
* LCD-Display implementiert
*\version 0.1
* UART implementiert und erste Test mit dem Sensor.
*
* Dieses Programm wird verwendet um die Flex Sensoren ueber den AD-Wandler auszulesen und anschliessend ueber Bluetooth (UART) zu verschicken.
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
#include "lpc_types.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_adc.h"



/*********************************************************************
 *																	 *
 *							DEFINITIONEN							 *
 *																	 *
 * *******************************************************************/



/*------------------------ position-DEFINITIONEN ------------------------*/
/** Aus den ADC-Werten umgerechnet die Positionen der Servomotoren  **/
uint16_t PositionArray[5] ={0,0,0,0,0};
uint16_t PositionArray_Temp[5]={0,0,0,0,0};

/** Trashhold fuer die Aktualisierung der Position - wenn sich der Wert von der letzten Messung nicht mehr als um 2 geaendert hat, dann nicht aktualisieren   **/
uint16_t Position_Trashhold=4;

/** Max. bzw. Min position der einzelnen Servos  **/
uint16_t PositionHighLimit[5] = {1900,2050,2200,2000,1750};
uint16_t PositionLowLimit[5]  = {1050,750,1200,700,700};



/** Temp. Variable  **/
uint16_t var;


/** MaxMin Werte **/
uint16_t SensorArrayMax[5]={2000,2000,2000,1900,1900};
uint16_t SensorArrayMin[5]={1400,1200,1300,1000,1000};

uint16_t MotorArrayMax[5]={1900,2050,2200,2000,1750};
uint16_t MotorArrayMin[5]={1050,750,1200,700,700};

double Delta_SensorMotor[5]={0,0,0,0,0};


/*------------------------ UART-DEFINITIONEN ------------------------*/
/** Tx Buffer für UART Uebertragung **/
uint8_t txBuf[16];


/*------------------------- ADC-DEFINITIONEN ------------------------*/

/** Eingelesene ADC-Werte der Sensoren - Daumen, Zeigefinger, Mittelfinger, Ringfinger, kleiner Finger **/
uint16_t adcValueArray[5] ={0,0,0,0,0};

/** ADC Channel Array  **/
uint8_t adc_channelArray[5]={0,1,2,3,6};



/*********************************************************************
 *																	 *
 *						   	 PROTOTYPEN		    					 *
 *																	 *
 * *******************************************************************/
void LCD_Init(void);
void Init_System(void);
void Timer0_DelayMs(uint32_t delayInMs);
void UART_Inital(void);
void ADC_HardWareInit(void);
void UARTPutDec16(uint16_t decnum);
void UARTPutChar(uint8_t ch);
uint16_t adcValue_BurstRead(uint8_t adc_channel);



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
*			Init_LCD() - LCD-Display wird initialisiert
*
*			ADC_HardWareInit() - AD-Wandler wird initialisiert
*
*			Blablabla....
*/
int main(void)
{
	int i=0;

	//* System initialisieren
	LCD_Init();

	//* UART Schnittstelle initialisieren
	UART_Inital();


	//* AD-Wandler initialisieren
	ADC_HardWareInit();


	for(i=0; i<5;i++){
					Delta_SensorMotor[i]=( (double) (MotorArrayMax[i]-MotorArrayMin[i])/(SensorArrayMax[i]-SensorArrayMin[i]) );
				}



	/* WHILE-SCHLEIFE */
	while(1)
	    {


			for(i=0; i<5;i++){
				PositionArray_Temp[i]=PositionArray[i];

				adcValueArray[i]=adcValue_BurstRead(adc_channelArray[i]);


				PositionArray[i]=MotorArrayMin[i]+(adcValueArray[i]-SensorArrayMin[i])*Delta_SensorMotor[i];


			}



			txBuf[0]=0xBB;
			txBuf[1]=0xFF;
			txBuf[2]=0xAA;


			//* Finger 1 - Daumen
			if( (PositionArray_Temp[0] >= (PositionArray[0]+Position_Trashhold) ) ||  (PositionArray_Temp[0] <= (PositionArray[0]-Position_Trashhold) ) ){

				if(PositionArray[0] < PositionHighLimit[0] && PositionArray[0]> PositionLowLimit[0] ){
					txBuf[3]=(PositionArray[0] >> 8 );
					txBuf[4]= PositionArray[0];
				}

			}

			//* Finger 2 - Zeigefinder
			if( (PositionArray_Temp[1] >= (PositionArray[1]+Position_Trashhold) ) || (PositionArray_Temp[1] <= (PositionArray[1]-Position_Trashhold) ) ){

				if(PositionArray[1] < PositionHighLimit[1] && PositionArray[1]> PositionLowLimit[1] ){
					txBuf[5]=(PositionArray[1] >> 8 );
					txBuf[6]=PositionArray[1];
				}

			}

			//* Finger 3 - Mittelfinger
			if( (PositionArray_Temp[2] >= (PositionArray[2]+Position_Trashhold) ) || (PositionArray_Temp[2] <= (PositionArray[2]-Position_Trashhold) ) ){

				if(PositionArray[2] < PositionHighLimit[2] && PositionArray[2]> PositionLowLimit[2] ){
					txBuf[7]=(PositionArray[2] >> 8 );
					txBuf[8]=PositionArray[2];
				}

			}

			//* Finger 4 - Ringfinger
			if( (PositionArray_Temp[3] >= (PositionArray[3]+Position_Trashhold) ) || (PositionArray_Temp[3] <= (PositionArray[3]-Position_Trashhold) ) ){

				if(PositionArray[3] < PositionHighLimit[3] && PositionArray[3]> PositionLowLimit[3] ){
					txBuf[9]=(PositionArray[3] >> 8 );
					txBuf[10]=PositionArray[3];
				}

			}

			//* Finger 5 - Kleiner Finger
			if( (PositionArray_Temp[4] >= (PositionArray[4]+Position_Trashhold) ) || (PositionArray_Temp[4] <= (PositionArray[4]-Position_Trashhold) ) ){

				if(PositionArray[4] < PositionHighLimit[4] && PositionArray[4]> PositionLowLimit[4] ){
					txBuf[11]=(PositionArray[4] >> 8 );
					txBuf[12]=PositionArray[4];
				}

			}




			UART_Send((LPC_UART_TypeDef *)LPC_UART3, txBuf, 13, BLOCKING);



			Timer0_DelayMs(10);

	    }

}

/*!
*
*	\brief	<b>AD-Wandler Burst Read<b>:
*
*			Auslesen des aktuellen AD Wertes
*
*   \param[in]	Channel (1-5)
*   \return 	none
*/
uint16_t adcValue_BurstRead(uint8_t adc_channel)
{


	  // Wait conversion complete
	  while (!(ADC_ChannelGetStatus(LPC_ADC, adc_channel, ADC_DATA_DONE)));

	  return ADC_ChannelGetData(LPC_ADC, adc_channel);
}



/*!
*
*	\brief	<b>AD-Wandler initialisieren<b>:
*
*			Initialisierung des AD-Wandlers in Burst-Mode
*
*   \param[in]	none
*   \return 	none
*/
void ADC_HardWareInit(void)
{
	PINSEL_CFG_Type PinCfg;

	/*
	 * Init ADC pin connect
	 * AD0.0 - AD0.3
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;

	//* AD0.0 on P0.23
	PinCfg.Pinnum = 23;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	//* AD0.1 on P0.24
	PinCfg.Pinnum = 24;
	PINSEL_ConfigPin(&PinCfg);

	//* AD0.2 on P0.25
	PinCfg.Pinnum = 25;
	PINSEL_ConfigPin(&PinCfg);

	//* AD0.3 on P0.26
	PinCfg.Pinnum = 26;
	PINSEL_ConfigPin(&PinCfg);


	/*
	 * Init ADC pin connect
	 * AD0.6
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	//* AD0.6 on P0.3
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);


	/* Configuration for ADC:
	 *  select: ADC channel 1
	 *  ADC conversion rate = 200KHz
	 */
	ADC_Init(LPC_ADC, 200000);

	 // Start burst mode.
	 ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
	 ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_1, ENABLE);
	 ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, ENABLE);
	 ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_3, ENABLE);
	 ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_6, ENABLE);
	 ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);
	 ADC_BurstCmd(LPC_ADC, ENABLE);

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
	  //UART_IntConfig(LPC_UART3, UART_INTCFG_RBR, ENABLE);



	  	/** NVIC enable interrupter , it's request */
	  //NVIC_EnableIRQ(UART3_IRQn);




	  UART_TxCmd(LPC_UART3, ENABLE);

}




/*!
*
*	\brief	<b>UARTPutDec32<b>:
*
*			Puts a decimal number to UART port
*
*   \param[in]	decnum the decimal value to be displayed
*   \return 	none
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
*	\brief	<b>UUARTPutChar<b>:
*
*			Puts a character to UART port.
*
*   \param[in]	ch the character to be displayed
*   \return 	none
*/
void UARTPutChar(uint8_t ch)
{
	 UART_Send((LPC_UART_TypeDef *)LPC_UART3, &ch, 1, BLOCKING);
}

