#include <stdio.h>
#include "NUC131.h"
#include <string.h>
#include <stdlib.h>

#define PLL_CLOCK   50000000

char* send_string;
int len = 0;
int i = 0;
int ff = 0;

volatile int32_t g_OK = TRUE;
volatile int32_t g_bWait = TRUE;
char* ok ="OK\r\n";


void UART3_HANDLE()
{
		char u8InChar = 0xff;
		uint32_t u32IntSts = UART3->ISR;
	if(u32IntSts & UART_ISR_RDA_INT_Msk)
    {
				// Get all the input characters 
        while(UART_IS_RX_READY(UART3))
        {
            //Get the character from UART Buffer
            u8InChar = UART_READ(UART3);
						printf("%c",u8InChar);
						
					
						//check if READ char " OK "
						if(g_OK)
							{
								if(u8InChar == ok[ff])
								{
									ff++;
									if(ff == 4)
									{
										g_OK = FALSE;
									}
								}
								else	ff=0;
							}					
        }
		}
	if(u32IntSts & UART_ISR_THRE_INT_Msk)
    {       
            if(i>=len-1)
						{
							g_bWait = FALSE;
							UART_DISABLE_INT(UART3, 0x00000002); 
						}
            while(UART_IS_TX_FULL(UART3));  // Wait Tx is not full to transmit data                           
						UART3->THR = send_string[i++];         
     }
}





void UART0_HANDLE(void)
{
	char u8InChar = 0xFF;
    uint32_t u32IntSts = UART0->ISR;
    if(u32IntSts & UART_ISR_RDA_INT_Msk)
    {
        /* Get all the input characters */
        while(UART_IS_RX_READY(UART0))
        {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART0);
						printf("%c",u8InChar);            
        }        
    }
}


//transmit string	
void TransmitString(char* string)
{
		i = 0;
		send_string = string;
		len = strlen(string);
		g_bWait = TRUE;
		UART_EnableInt(UART3, UART_IER_THRE_IEN_Msk);
		while (g_bWait);
		
}



//setup UART0 and UART3	
void UART3_IRQHandler(void)
{
			UART3_HANDLE();
}
void UART02_IRQHandler(void)
{
		UART0_HANDLE();
}


//setup clock
void SYS_Init(void)
{
    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);
    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);
    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));
    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);
    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);
    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
	
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
		CLK_EnableModuleClock(UART3_MODULE);
	
		CLK_EnableModuleClock(TMR0_MODULE);
		CLK_EnableModuleClock(TMR1_MODULE);
		CLK_EnableModuleClock(TMR2_MODULE);
		
		
    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
		CLK_SetModuleClock(UART3_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
		
		CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HCLK, 0);
		CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HCLK, 0);
		CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2_S_HCLK, 0);

    /* Set GPB multi-function pins for UART0 RXD(PB.0) and TXD(PB.1) */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
		SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA2_Msk | SYS_GPA_MFP_PA3_Msk);
    SYS->GPA_MFP |= SYS_GPA_MFP_PA3_UART3_RXD | SYS_GPA_MFP_PA2_UART3_TXD;
		SYS->ALT_MFP4|=(SYS_ALT_MFP4_PA3_UART3_RXD| SYS_ALT_MFP4_PA2_UART3_TXD);
}

//keypad 4x4
unsigned char KEYS_PAD[4][4] ={ 'D' ,'#' ,'0' ,'*' ,
																'C' ,'9' ,'8' ,'7' ,
																'B' ,'6' ,'5' ,'4' ,
																'A' ,'1' ,'2' ,'3'};	
//setup scankey
void Init_Scankey(void)
{		
		GPIO_SetMode(PE, BIT5, GPIO_PMD_INPUT); 	//R1
		GPIO_SetMode(PA, BIT15, GPIO_PMD_INPUT);	//R2
		GPIO_SetMode(PA, BIT11, GPIO_PMD_INPUT);	//R3
		GPIO_SetMode(PA, BIT10, GPIO_PMD_INPUT);	//R4
	
		GPIO_SetMode(PB, BIT11, GPIO_PMD_OUTPUT); //C1
		GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT);	//C2
		GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT);	//C3
		GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);	//C4
	
		GPIO_SetMode(PF, BIT4, GPIO_PMD_OUTPUT); //output vibrate
		PF4 = 1; 
		
		PB11 = 0;	//C1
		PA14 = 0;	//C2
		PA13 = 0;	//C3
		PA12 = 0;	//C4
}

//Init Colums of keypad
void pinC(int pin,int pinx)
{
	switch(pin)
	{
		case 0:
		{
			PB11 = pinx;
			break;
		}
		case 1:
		{
			PA14 = pinx;
			break;
		}
		case 2: 
		{
			PA13 = pinx;
			break;
		}
		case 3:
		{
			PA12 = pinx;
			break;
		}
	}
}	

//Init rows of keypad
int pinR(int pin)
{
	switch(pin)
	{
		case 0:
		{
			if(PE5 == 0)
				return 0;
			else return 1;
			break;
		}
		case 1:
		{
			if(PA15 == 0)
				return 0;
			else return 1;
			break;
		}
		case 2: 
		{
			if(PA11 == 0)
				return 0;
			else return 1;
			break;
		}
		case 3:
		{
			if(PA10 == 0)
				return 0;
			else return 1;
			break;
		}
	}
	return 1;
}	

//Scankey function
unsigned char Scankey()
{			
			unsigned int i =0, c =0,j=0;
			unsigned int u32DelayTime = 0;
			PE5 = 1;		//R1
			PA15 = 1;		//R2	
			PA11 = 1;		//R3
			PA10 = 1;		//R4
					
			PB11 = 0;	//C1
			PA14 = 0;	//C2
			PA13 = 0;	//C3
			PA12 = 0;	//C4
			TIMER_Delay(TIMER0, 3);
			if(PE5 == 0||PA15 == 0||PA11 == 0||PA10 == 0)
			{
				  TIMER_Delay(TIMER0, 1000);
					if(PE5 == 0||PA15 == 0||PA11 == 0||PA10 == 0)
				{
					for(i = 0;i<4;i++)
					{
						for(c = 0; c<4;c++)
						{
							pinC(c,1);
							if(i==c)
								pinC(c,0);
						}
						TIMER_Delay(TIMER0, 1000);
						for(j = 0;j<4;j++)
						{
							if(pinR(j)==0)
							{
								TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
								while(pinR(j)==0);
								u32DelayTime = TIMER_GetCounter(TIMER1) / 1000;
								if(u32DelayTime < 100)
								return 'a';
							return KEYS_PAD[i][j];
						}
					}
				}
			}
		}
	return 'a';
}



void Delay(uint32_t time){
	uint32_t count = 0;
	while(count<time)
		{
			TIMER_Delay(TIMER0, 1000000);
			count++;
		}
}



void UART0_Init()
{		
	SYS_ResetModule(UART0_RST);
	UART_Open(UART0, 9600);
}
void UART3_Init()
{
	SYS_ResetModule(UART3_RST);
	UART_Open(UART3, 9600);
}
	
int32_t main(void)
{
			/* Unlock protected registers */
			SYS_UnlockReg();
			/* Init System, peripheral clock and multi-function I/O */
			SYS_Init();
			/* Lock protected registers */
			SYS_LockReg();
			/* Init UART0 for printf */
			UART0_Init();
			UART3_Init();
	
		
			Init_Scankey();
			char key;
		
			TIMER1->TCSR = TIMER_PERIODIC_MODE | (12 - 1);
			TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
			TIMER_Start(TIMER1);
			TIMER2->TCSR = TIMER_PERIODIC_MODE | (12 - 1);
			TIMER_SET_CMP_VALUE(TIMER2, 0xFFFFFF);
			TIMER_Start(TIMER2);		
			g_OK = FALSE;
			
			UART_ENABLE_INT(UART3, (UART_IER_RDA_IEN_Msk | UART_IER_TOUT_IEN_Msk));
			NVIC_EnableIRQ(UART3_IRQn);
			UART3->FCR &=~0x00000100;
			
			TransmitString("AT+RST\r\n");			
			Delay(5);
			TransmitString("AT+CWMODE=1\r\n");			
			while(g_OK);
			g_OK = TRUE;
			//Delay(2);
			TransmitString("AT+CIPMUX=0\r\n");
			//Delay(2);
			while(g_OK);
			g_OK = TRUE;
			
			TransmitString("AT+CWJAP=\"Tuan11\",\"tuan111phan\"\r\n");
			while(g_OK);
			g_OK = TRUE;
			//Delay(2);			
			TransmitString("AT+CIPSTART=\"TCP\",\"13.250.109.162\",8081\r\n");
			while(g_OK);
			g_OK = TRUE;
			//Delay(2);
			TIMER_SET_CMP_VALUE(TIMER2, 0xFFFFFF);
			
			
			
			while(1)
			{
				key = Scankey();
				if(key == '2'||key == '4'||key == '6'||key == '8'||key=='5')
				{
					TIMER_SET_CMP_VALUE(TIMER2, 0xFFFFFF);
					if(key == '2')
						key ='2';
					if(key == '4')
						key ='4';
					if(key == '6')
						key ='6';
					if(key == '8')
						key ='8';
					if(key == '5')
						key ='5';
					TransmitString("AT+CIPSEND=1\r\n");
					while(g_OK);
					g_OK = TRUE;
					TransmitString(&key);
					while(g_OK);
					g_OK = TRUE;
				}
			}
}

