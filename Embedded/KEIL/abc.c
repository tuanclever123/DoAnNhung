#include <stdio.h>
#include "NUC131.h"
#include <stdlib.h>
#include "string.h"

#define PLL_CLOCK   50000000
#define RXBUFSIZE   1024

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile int32_t g_bWait = TRUE;
int i=0;
int len=0;
uint32_t count = 0;
char *send_string;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void UART_TEST3_HANDLE(void);
void UART_TEST_HANDLE(void);
void Delay(uint32_t time);

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

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

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD(PB.0) and TXD(PB.1) */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
		SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA2_Msk | SYS_GPA_MFP_PA3_Msk);
    SYS->GPA_MFP |= SYS_GPA_MFP_PA3_UART3_RXD | SYS_GPA_MFP_PA2_UART3_TXD;
		SYS->ALT_MFP4|=(SYS_ALT_MFP4_PA3_UART3_RXD| SYS_ALT_MFP4_PA2_UART3_TXD);

}

void UART0_Init()
{
    /* Reset UART0 module */
    SYS_ResetModule(UART0_RST);
    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

void UART3_Init()
{
    /* Reset UART3 module */
    SYS_ResetModule(UART3_RST);
    /* Configure UART3 and set UART3 Baudrate */
    UART_Open(UART3, 115200);
}



/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

void UART02_IRQHandler(void)
{
    UART_TEST_HANDLE();
}
void UART3_IRQHandler(void)
{
    UART_TEST3_HANDLE();
}

void UART_TEST3_HANDLE(void)
{
    char u8InChar = 0xFF;
    uint32_t u32IntSts = UART3->ISR;
		
    if(u32IntSts & UART_ISR_RDA_INT_Msk)
    {
        /* Get all the input characters */
        while(UART_IS_RX_READY(UART3))
        {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART3);
						printf("%c",u8InChar);						     
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


void UART_TEST_HANDLE(void)
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



void TransmitString(char *string)
{
	i = 0;
	send_string =	string;
	len = strlen(string);
	g_bWait = TRUE;
	UART_EnableInt(UART3, UART_IER_THRE_IEN_Msk);
	while (g_bWait);	
}


void Delay(uint32_t time){
	uint32_t count = 0;
	while(count<time)
		{
			TIMER_Delay(TIMER0, 1000000);
			count++;
		}
}


	unsigned char KEYS_PAD[4][4] ={'D', '#' , '0' , '*',
								   'C' ,'9' , '8', '7',
								   'B' ,'6' ,'5', '4',
								   'A' , '1', '2' , '3' };
	void quetphim_Init(volatile uint32_t ** output,volatile uint32_t ** input)
	{
			GPIO_SetMode(PA, BIT10, GPIO_PMD_INPUT);
			GPIO_SetMode(PA, BIT11, GPIO_PMD_INPUT);
			GPIO_SetMode(PA, BIT15, GPIO_PMD_INPUT);
			GPIO_SetMode(PE, BIT5, GPIO_PMD_INPUT);
		
			GPIO_SetMode(PB, BIT11, GPIO_PMD_OUTPUT);
			GPIO_SetMode(PA, BIT14, GPIO_PMD_OUTPUT);
			GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT);
			GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);
			
			GPIO_SetMode(PA, BIT4, GPIO_PMD_OUTPUT);
			PA4=0;
		
			PB11 = 0;
			PA14 = 0;
			PA13 = 0;
			PA12 = 0;
		
			input[3] = &PA10;
			input[2] = &PA11;
			input[1]=	&PA15;
			input[0]=	&PE5;
		
			output[0] = &PB11;
			output[1] = &PA14;
			output[2]=	&PA13;
			output[3]=	&PA12;
	}
	unsigned char quetphim(volatile uint32_t ** output,volatile uint32_t ** input)
	{
			PA10 = 1;
			PA11 = 1;
			PA15 = 1;
			PE5 = 1;
		
			PB11 = 0;
			PA14 = 0;
			PA13 = 0;
			PA12 = 0;		
			TIMER_Delay(TIMER0, 2);
			if(*input[0] == 0||*input[1] == 0||*input[2] == 0||*input[3] == 0)
			{
				  TIMER_Delay(TIMER0, 1000);
					if(*input[0] == 0||*input[1] == 0||*input[2] == 0||*input[3] == 0)
				{
					for(int i = 0;i<4;i++)
					{
						for(int c = 0; c<4;c++)
						{
							*output[c] = 1;
							if(i==c)
								*output[c] = 0;
						}
						TIMER_Delay(TIMER0, 1000);
						for(int j = 0;j<4;j++)
						{
							if(*input[j]==0)
							{
								TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
								while(*input[j]==0);
								int u32DelayTime = TIMER_GetCounter(TIMER1) / 1000;
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


int main(void)
{
		char key;
		
	
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
		
    /* Init UART0 for printf and testing */	
    UART0_Init();
		UART3_Init();
    printf("\n\nESP8266 & NuMaker UNO\n");
		UART_ENABLE_INT(UART3, (UART_IER_RDA_IEN_Msk | UART_IER_TOUT_IEN_Msk));
		NVIC_EnableIRQ(UART3_IRQn);
		UART3->FCR &=~0x00000100;	
		
		volatile uint32_t **output=(volatile uint32_t **) malloc(4*sizeof(uint32_t));
			volatile uint32_t **input= (volatile uint32_t **) malloc(4*sizeof(uint32_t));
		quetphim_Init(output,input);


		TransmitString("AT+RST\r\n");
		Delay(5);
		TransmitString("AT+CWMODE=1\r\n");
		Delay(5);
		TransmitString("AT+CIPMUX=0\r\n");
		Delay(5);		
		TransmitString("AT+CWJAP=\"Tuan11\",\"tuan111phan\"\r\n");
		Delay(10);
		TransmitString("AT+CIPSTART=\"TCP\",\"192.168.0.101\",8000\r\n");
		Delay(10);

    while(1)
			{
				key = quetphim(output,input);
				if(key == '2'||key == '4'||key == '6'||key == '8'||key=='5')
				{
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
					TransmitString("AT+CIPSEND=");
					TransmitString("1\r\n");
					TransmitString(&key);
					Delay(5);

				}
			}
}

