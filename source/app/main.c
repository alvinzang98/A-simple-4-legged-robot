#include "NUC131.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
	unsigned int m_User_Action_CMD_00;

//=========================================================
// Bottom - Mobile Platform
//=========================================================
// Pin Mapping
//=========================================================
// BT_Control_UART_TX	PB0/UART0_RXD
// BT_Control_UART_RX	PB1/UART0_TXD
//=========================================================

//=========================================================
// Robot Action Command
//=========================================================
unsigned int 	g_User_Action_CMD_00	= 0;
//=========================================================

//=========================================================
// UART
//=========================================================
unsigned int	g_UART_00_RX_Result_Byte 			= 0;
unsigned int  	g_UART_05_RX_Result_Byte 			= 0;
//=========================================================

//=========================================================
void f_SYS_Init(void);
void f_SYS_Exit(void);
void f_Startup_Init(void);
void Forward(void);
//=========================================================

void UART02_IRQHandler(void)
{
    uint32_t     u32IntSts= UART0->ISR;
    uint8_t      t_UART_00_RX_Result_Buffer[1] = {0};
    unsigned int t_UART_00_RX_Result_Byte      = 0;

    if(u32IntSts & UART_IS_RX_READY(UART0))
    {
       	UART_Read(UART0, t_UART_00_RX_Result_Buffer, sizeof(t_UART_00_RX_Result_Buffer));
       	t_UART_00_RX_Result_Byte = (unsigned int)t_UART_00_RX_Result_Buffer[0];
       	g_UART_00_RX_Result_Byte = t_UART_00_RX_Result_Byte;
       	m_User_Action_CMD_00 = g_UART_00_RX_Result_Byte;
 //      	UART_Write(UART0, t_UART_00_RX_Result_Buffer, sizeof(t_UART_00_RX_Result_Buffer));

    }
}

void f_Motor_Control_Delay(void)
{
	int n = 0;
	int DoSomething = 0;
	for(n=0;n<5000;n++)
	{
		DoSomething = 0;
	}
}

void Short_Delay(void)
{
	int n = 0;
	int DoSomething = 0;
	for(n=0;n<300000;n++)
	{
		DoSomething = 999;
	}
}

void Long_Delay(void)
{
	int n = 0;
	int DoSomething = 0;
	for(n=0;n<1000000;n++)
	{
		DoSomething = 999;
	}
}

void f_SYS_Init(void)
{
	//================================================================================
	// Init Clock
	//================================================================================

    //==================================================
    // Init System Clock
    //==================================================
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);
    CLK_SetCoreClock(50000000);
    //==================================================
    // Init Module Clock - UART0
    //==================================================
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    CLK_EnableModuleClock(UART0_MODULE);
    SYS_ResetModule(UART0_RST);
    //==================================================
    // Init Module Clock - UART5
    //==================================================
    CLK_SetModuleClock(UART5_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    CLK_EnableModuleClock(UART5_MODULE);
    SYS_ResetModule(UART5_RST);

    //================================================================================
    // Init Multi Function Pin
    //================================================================================
    //==================================================
    // Init Multi Function Pin - UART0
    //==================================================
    SYS->GPB_MFP  &= ~(SYS_GPB_MFP_PB0_Msk       | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP  |=   SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
    //==================================================
    // Init Multi Function Pin - UART5
    //==================================================
    SYS->GPA_MFP  &= ~(SYS_GPA_MFP_PA0_Msk       | SYS_GPA_MFP_PA1_Msk);
    SYS->GPA_MFP  |=   SYS_GPA_MFP_PA0_UART5_TXD | SYS_GPA_MFP_PA1_UART5_RXD;
}

void f_SYS_Exit(void)
{
	UART_Close(UART0);
	UART_Close(UART5);
}

void f_Startup_Init(void)
{
	//==================================================
	// I/O Configuration
	//==================================================
	// Init (UART0)
	UART_Open(UART0, 115200);
	UART_EnableInt(UART0, (UART_IER_RDA_IEN_Msk));
	NVIC_EnableIRQ(UART02_IRQn);
	// Init (UART5)
	UART_Open(UART5, 9600);
	UART_EnableInt(UART5, (UART_IER_RDA_IEN_Msk));
	NVIC_EnableIRQ(UART5_IRQn);
}

char Motor_CMD_CheckSum(char *Temp_CMD_01)
{
    int  Temp_Value_01 = 0;
    char Temp_Value_02;
    for (Temp_Value_02 = 2; Temp_Value_02 < Temp_CMD_01[3] + 2; Temp_Value_02++)
    {
        Temp_Value_01 += Temp_CMD_01[Temp_Value_02];
    }
    Temp_Value_01 = ~Temp_Value_01;
    Temp_Value_02 = (char)Temp_Value_01;
    return Temp_Value_02;
}

void Reset(void)
{
	char 	m_Motor_CMD_01[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_02[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_03[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_04[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_05[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_06[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_07[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_08[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	m_User_Action_CMD_00 = '\0';

	//===========================================================
	//	1
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	//===========================================================
	//	2
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	5
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//	6
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	//===========================================================
	//	7
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	//===========================================================
	//	8
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	Short_Delay();
}

void Little_Left(void)
{
	char 	m_Motor_CMD_01[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_02[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_03[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_04[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_05[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_06[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_07[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_08[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	m_User_Action_CMD_00 = '\0';


	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	//===========================================================
	//
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	//===========================================================
	//
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0x84;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x03;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	1
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	2
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	3
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	4
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	5
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	6
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	Long_Delay();
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//===========================================================
	//
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
}
void Little_Right(void)
{
	char 	m_Motor_CMD_01[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_02[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_03[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_04[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_05[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_06[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_07[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_08[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	m_User_Action_CMD_00 = '\0';


	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	//===========================================================
	//
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	//===========================================================
	//
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0x84;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x03;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	1
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	2
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0x84;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x03;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	3
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	4
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	5
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	6
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	Long_Delay();
	//===========================================================
	//
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
}
void Little_Running(void)
{
	char 	m_Motor_CMD_01[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_02[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_03[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_04[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_05[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_06[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_07[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_08[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	m_User_Action_CMD_00 = '\0';


	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	//===========================================================
	//
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	//===========================================================
	//
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0x84;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x03;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	1
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	2
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0x84;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x03;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	3
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	4
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	5
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	6
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	Long_Delay();
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Long_Delay();
	Long_Delay();
	//===========================================================
	//
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
}
void Front_Climbing(void)
{
	char 	m_Motor_CMD_01[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_02[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_03[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_04[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_05[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_06[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_07[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_08[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	m_User_Action_CMD_00  = '\0';
			//===========================================================
			//	0
			m_Motor_CMD_05[0] = 0x55;
			m_Motor_CMD_05[1] = 0x55;
			m_Motor_CMD_05[2] = 0x05;		// ID 5
			m_Motor_CMD_05[3] = 0x07;
			m_Motor_CMD_05[4] = 0x01;
			m_Motor_CMD_05[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_05[7] = 0x00;
			m_Motor_CMD_05[8] = 0x00;
			m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
			UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
			//===========================================================
			//===========================================================
			//	0
			m_Motor_CMD_07[0] = 0x55;
			m_Motor_CMD_07[1] = 0x55;
			m_Motor_CMD_07[2] = 0x07;		// ID 7
			m_Motor_CMD_07[3] = 0x07;
			m_Motor_CMD_07[4] = 0x01;
			m_Motor_CMD_07[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_07[7] = 0x00;
			m_Motor_CMD_07[8] = 0x00;
			m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
			UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
			//===========================================================
			Long_Delay();
			//===========================================================



    		//===========================================================
    		//	1
    		m_Motor_CMD_01[0] = 0x55;
    		m_Motor_CMD_01[1] = 0x55;
    		m_Motor_CMD_01[2] = 0x01;		// ID 1
    		m_Motor_CMD_01[3] = 0x07;
    		m_Motor_CMD_01[4] = 0x01;
    		m_Motor_CMD_01[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
    		m_Motor_CMD_01[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
    		m_Motor_CMD_01[7] = 0x00;
    		m_Motor_CMD_01[8] = 0x00;
    		m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
    		UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
    		//===========================================================
			Long_Delay();
			//===========================================================
			//	2
			m_Motor_CMD_02[0] = 0x55;
			m_Motor_CMD_02[1] = 0x55;
			m_Motor_CMD_02[2] = 0x02;		// ID 2
			m_Motor_CMD_02[3] = 0x07;
			m_Motor_CMD_02[4] = 0x01;
			m_Motor_CMD_02[5] = 0x84;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_02[6] = 0x03;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_02[7] = 0x00;
			m_Motor_CMD_02[8] = 0x00;
			m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
			UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
			//===========================================================
			Long_Delay();
			//===========================================================
			//	3
			m_Motor_CMD_01[0] = 0x55;
			m_Motor_CMD_01[1] = 0x55;
			m_Motor_CMD_01[2] = 0x01;		// ID 1
			m_Motor_CMD_01[3] = 0x07;
			m_Motor_CMD_01[4] = 0x01;
			m_Motor_CMD_01[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_01[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_01[7] = 0x00;
			m_Motor_CMD_01[8] = 0x00;
			m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
			UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
			//===========================================================
			Long_Delay();
			//===========================================================
			//	4
			m_Motor_CMD_03[0] = 0x55;
			m_Motor_CMD_03[1] = 0x55;
			m_Motor_CMD_03[2] = 0x03;		// ID 3
			m_Motor_CMD_03[3] = 0x07;
			m_Motor_CMD_03[4] = 0x01;
			m_Motor_CMD_03[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_03[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_03[7] = 0x00;
			m_Motor_CMD_03[8] = 0x00;
			m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
			UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
			//===========================================================
			Long_Delay();
			//===========================================================
			//	5
			m_Motor_CMD_04[0] = 0x55;
			m_Motor_CMD_04[1] = 0x55;
			m_Motor_CMD_04[2] = 0x04;		// ID 4
			m_Motor_CMD_04[3] = 0x07;
			m_Motor_CMD_04[4] = 0x01;
			m_Motor_CMD_04[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_04[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_04[7] = 0x00;
			m_Motor_CMD_04[8] = 0x00;
			m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
			UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
			//===========================================================
			Long_Delay();
    		//===========================================================
    		//	6
    		m_Motor_CMD_03[0] = 0x55;
    		m_Motor_CMD_03[1] = 0x55;
    		m_Motor_CMD_03[2] = 0x03;		// ID 3
    		m_Motor_CMD_03[3] = 0x07;
    		m_Motor_CMD_03[4] = 0x01;
    		m_Motor_CMD_03[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
    		m_Motor_CMD_03[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
    		m_Motor_CMD_03[7] = 0x00;
    		m_Motor_CMD_03[8] = 0x00;
    		m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
    		UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
    		//===========================================================
			Long_Delay();
			//===========================================================
			//	7
			m_Motor_CMD_05[0] = 0x55;
			m_Motor_CMD_05[1] = 0x55;
			m_Motor_CMD_05[2] = 0x05;		// ID 5
			m_Motor_CMD_05[3] = 0x07;
			m_Motor_CMD_05[4] = 0x01;
			m_Motor_CMD_05[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_05[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_05[7] = 0x00;
			m_Motor_CMD_05[8] = 0x00;
			m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
			UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
			//===========================================================
    		//===========================================================
    		//	10
    		m_Motor_CMD_07[0] = 0x55;
    		m_Motor_CMD_07[1] = 0x55;
    		m_Motor_CMD_07[2] = 0x07;		// ID 7
    		m_Motor_CMD_07[3] = 0x07;
    		m_Motor_CMD_07[4] = 0x01;
    		m_Motor_CMD_07[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
    		m_Motor_CMD_07[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
    		m_Motor_CMD_07[7] = 0x00;
    		m_Motor_CMD_07[8] = 0x00;
    		m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
    		UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
    		//===========================================================
			Long_Delay();
			//*******************************************************************************************************
			//*******************************************************************************************************
			//===========================================================
			//	
			m_Motor_CMD_02[0] = 0x55;
			m_Motor_CMD_02[1] = 0x55;
			m_Motor_CMD_02[2] = 0x02;		// ID 2
			m_Motor_CMD_02[3] = 0x07;
			m_Motor_CMD_02[4] = 0x01;
			m_Motor_CMD_02[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_02[7] = 0x00;
			m_Motor_CMD_02[8] = 0x00;
			m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
			UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
			//===========================================================
			//===========================================================
			//	
			m_Motor_CMD_04[0] = 0x55;
			m_Motor_CMD_04[1] = 0x55;
			m_Motor_CMD_04[2] = 0x04;		// ID 4
			m_Motor_CMD_04[3] = 0x07;
			m_Motor_CMD_04[4] = 0x01;
			m_Motor_CMD_04[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_04[7] = 0x00;
			m_Motor_CMD_04[8] = 0x00;
			m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
			UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
			//===========================================================
			//===========================================================
			//	
			m_Motor_CMD_06[0] = 0x55;
			m_Motor_CMD_06[1] = 0x55;
			m_Motor_CMD_06[2] = 0x06;		// ID 6
			m_Motor_CMD_06[3] = 0x07;
			m_Motor_CMD_06[4] = 0x01;
			m_Motor_CMD_06[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_06[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_06[7] = 0x00;
			m_Motor_CMD_06[8] = 0x00;
			m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
			UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
			//===========================================================
			//===========================================================
			//	
			m_Motor_CMD_08[0] = 0x55;
			m_Motor_CMD_08[1] = 0x55;
			m_Motor_CMD_08[2] = 0x08;		// ID 8
			m_Motor_CMD_08[3] = 0x07;
			m_Motor_CMD_08[4] = 0x01;
			m_Motor_CMD_08[5] = 0x84;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_08[6] = 0x03;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_08[7] = 0x00;
			m_Motor_CMD_08[8] = 0x00;
			m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
			UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
			//===========================================================
			Long_Delay();

}
void Back_Climbing(void)
{
	char 	m_Motor_CMD_01[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_02[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_03[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_04[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_05[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_06[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_07[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_08[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	m_User_Action_CMD_00  = '\0';
	//===========================================================
	//	3
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	4
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	7
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Long_Delay();
	//===========================================================
	//
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	Long_Delay();
	//===========================================================
	//	7
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Long_Delay();
	//===========================================================
	//	10
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	Long_Delay();
	//===========================================================
	//
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	Long_Delay();
	//===========================================================
	//	10
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	Long_Delay();
/*	//===========================================================
	//	1
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	2
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0x84;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x03;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	Short_Delay();*/
	//===========================================================
	//	3
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	//===========================================================
	//	4
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	Long_Delay();
	Long_Delay();
	/*
	//===========================================================
	//	5
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	Short_Delay();
	//===========================================================
	//	6
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x58;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	Long_Delay();
	*/
	/*
			//*******************************************************************************************************
			//*******************************************************************************************************
			//===========================================================
			//
			m_Motor_CMD_02[0] = 0x55;
			m_Motor_CMD_02[1] = 0x55;
			m_Motor_CMD_02[2] = 0x02;		// ID 2
			m_Motor_CMD_02[3] = 0x07;
			m_Motor_CMD_02[4] = 0x01;
			m_Motor_CMD_02[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_02[7] = 0x00;
			m_Motor_CMD_02[8] = 0x00;
			m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
			UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
			//===========================================================
			//===========================================================
			//
			m_Motor_CMD_04[0] = 0x55;
			m_Motor_CMD_04[1] = 0x55;
			m_Motor_CMD_04[2] = 0x04;		// ID 4
			m_Motor_CMD_04[3] = 0x07;
			m_Motor_CMD_04[4] = 0x01;
			m_Motor_CMD_04[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_04[7] = 0x00;
			m_Motor_CMD_04[8] = 0x00;
			m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
			UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
			//===========================================================
			//===========================================================
			//
			m_Motor_CMD_06[0] = 0x55;
			m_Motor_CMD_06[1] = 0x55;
			m_Motor_CMD_06[2] = 0x06;		// ID 6
			m_Motor_CMD_06[3] = 0x07;
			m_Motor_CMD_06[4] = 0x01;
			m_Motor_CMD_06[5] = 0x64;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_06[6] = 0x00;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_06[7] = 0x00;
			m_Motor_CMD_06[8] = 0x00;
			m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
			UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
			//===========================================================
			//===========================================================
			//
			m_Motor_CMD_08[0] = 0x55;
			m_Motor_CMD_08[1] = 0x55;
			m_Motor_CMD_08[2] = 0x08;		// ID 8
			m_Motor_CMD_08[3] = 0x07;
			m_Motor_CMD_08[4] = 0x01;
			m_Motor_CMD_08[5] = 0x84;		//(byte)((Temp_Motor_Position_01));
			m_Motor_CMD_08[6] = 0x03;		//(byte)((Temp_Motor_Position_01) >> 8);
			m_Motor_CMD_08[7] = 0x00;
			m_Motor_CMD_08[8] = 0x00;
			m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
			UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
			//===========================================================
			Long_Delay();
*/
}
void Running(void)
{
	char 	m_Motor_CMD_01[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_02[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_03[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_04[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_05[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_06[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_07[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_08[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	m_User_Action_CMD_00 = '\0';

	//===========================================================
	//	1
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	//===========================================================
	//	2
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	5
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0x26;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//	6
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0x26;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	//===========================================================
	//	7
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0xC2;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	//===========================================================
	//	8
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0xC2;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	9
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	//===========================================================
	//	10
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	//===========================================================
	//	11
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	12
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	13
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0xC2;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//	14
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0xC2;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	//===========================================================
	//	15
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0x26;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	//===========================================================
	//	16
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0x26;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	Short_Delay();
}

void Left(void)
{
	char 	m_Motor_CMD_01[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_02[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_03[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_04[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_05[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_06[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_07[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_08[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	m_User_Action_CMD_00 = '\0';

	//===========================================================
	//	1
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	//===========================================================
	//	2
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	5
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//	6
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0x26;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	//===========================================================
	//	7
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0xC2;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	//===========================================================
	//	8
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	9
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	//===========================================================
	//	10
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	//===========================================================
	//	11
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	12
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	13
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//	14
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0xC2;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	//===========================================================
	//	15
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0x26;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	//===========================================================
	//	16
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	Short_Delay();
}

void Right(void)
{
	char 	m_Motor_CMD_01[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_02[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_03[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_04[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_05[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_06[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_07[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	char 	m_Motor_CMD_08[10] = { '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };
	m_User_Action_CMD_00 = '\0';

	//===========================================================
	//	1
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	//===========================================================
	//	2
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	//===========================================================
	//	3
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	4
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	5
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0x26;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//	6
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	//===========================================================
	//	7
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	//===========================================================
	//	8
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0xC2;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	9
	m_Motor_CMD_01[0] = 0x55;
	m_Motor_CMD_01[1] = 0x55;
	m_Motor_CMD_01[2] = 0x01;		// ID 1
	m_Motor_CMD_01[3] = 0x07;
	m_Motor_CMD_01[4] = 0x01;
	m_Motor_CMD_01[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_01[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_01[7] = 0x00;
	m_Motor_CMD_01[8] = 0x00;
	m_Motor_CMD_01[9] = Motor_CMD_CheckSum(m_Motor_CMD_01);		//Checksum
	UART_Write(UART0, m_Motor_CMD_01, sizeof(m_Motor_CMD_01));
	//===========================================================
	//===========================================================
	//	10
	m_Motor_CMD_03[0] = 0x55;
	m_Motor_CMD_03[1] = 0x55;
	m_Motor_CMD_03[2] = 0x03;		// ID 3
	m_Motor_CMD_03[3] = 0x07;
	m_Motor_CMD_03[4] = 0x01;
	m_Motor_CMD_03[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_03[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_03[7] = 0x00;
	m_Motor_CMD_03[8] = 0x00;
	m_Motor_CMD_03[9] = Motor_CMD_CheckSum(m_Motor_CMD_03);		//Checksum
	UART_Write(UART0, m_Motor_CMD_03, sizeof(m_Motor_CMD_03));
	//===========================================================
	//===========================================================
	//	11
	m_Motor_CMD_07[0] = 0x55;
	m_Motor_CMD_07[1] = 0x55;
	m_Motor_CMD_07[2] = 0x07;		// ID 7
	m_Motor_CMD_07[3] = 0x07;
	m_Motor_CMD_07[4] = 0x01;
	m_Motor_CMD_07[5] = 0x90;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_07[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_07[7] = 0x00;
	m_Motor_CMD_07[8] = 0x00;
	m_Motor_CMD_07[9] = Motor_CMD_CheckSum(m_Motor_CMD_07);
	UART_Write(UART0, m_Motor_CMD_07, sizeof(m_Motor_CMD_07));
	//===========================================================
	//===========================================================
	//	12
	m_Motor_CMD_05[0] = 0x55;
	m_Motor_CMD_05[1] = 0x55;
	m_Motor_CMD_05[2] = 0x05;		// ID 5
	m_Motor_CMD_05[3] = 0x07;
	m_Motor_CMD_05[4] = 0x01;
	m_Motor_CMD_05[5] = 0x2C;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_05[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_05[7] = 0x00;
	m_Motor_CMD_05[8] = 0x00;
	m_Motor_CMD_05[9] = Motor_CMD_CheckSum(m_Motor_CMD_05);
	UART_Write(UART0, m_Motor_CMD_05, sizeof(m_Motor_CMD_05));
	//===========================================================
	Short_Delay();
	//*******************************************************************************************************
	//*******************************************************************************************************
	//===========================================================
	//	13
	m_Motor_CMD_02[0] = 0x55;
	m_Motor_CMD_02[1] = 0x55;
	m_Motor_CMD_02[2] = 0x02;		// ID 2
	m_Motor_CMD_02[3] = 0x07;
	m_Motor_CMD_02[4] = 0x01;
	m_Motor_CMD_02[5] = 0xC2;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_02[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_02[7] = 0x00;
	m_Motor_CMD_02[8] = 0x00;
	m_Motor_CMD_02[9] = Motor_CMD_CheckSum(m_Motor_CMD_02);		//Checksum
	UART_Write(UART0, m_Motor_CMD_02, sizeof(m_Motor_CMD_02));
	//===========================================================
	//===========================================================
	//	14
	m_Motor_CMD_04[0] = 0x55;
	m_Motor_CMD_04[1] = 0x55;
	m_Motor_CMD_04[2] = 0x04;		// ID 4
	m_Motor_CMD_04[3] = 0x07;
	m_Motor_CMD_04[4] = 0x01;
	m_Motor_CMD_04[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_04[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_04[7] = 0x00;
	m_Motor_CMD_04[8] = 0x00;
	m_Motor_CMD_04[9] = Motor_CMD_CheckSum(m_Motor_CMD_04);		//Checksum
	UART_Write(UART0, m_Motor_CMD_04, sizeof(m_Motor_CMD_04));
	//===========================================================
	//===========================================================
	//	15
	m_Motor_CMD_08[0] = 0x55;
	m_Motor_CMD_08[1] = 0x55;
	m_Motor_CMD_08[2] = 0x08;		// ID 8
	m_Motor_CMD_08[3] = 0x07;
	m_Motor_CMD_08[4] = 0x01;
	m_Motor_CMD_08[5] = 0xF4;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_08[6] = 0x01;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_08[7] = 0x00;
	m_Motor_CMD_08[8] = 0x00;
	m_Motor_CMD_08[9] = Motor_CMD_CheckSum(m_Motor_CMD_08);
	UART_Write(UART0, m_Motor_CMD_08, sizeof(m_Motor_CMD_08));
	//===========================================================
	//===========================================================
	//	16
	m_Motor_CMD_06[0] = 0x55;
	m_Motor_CMD_06[1] = 0x55;
	m_Motor_CMD_06[2] = 0x06;		// ID 6
	m_Motor_CMD_06[3] = 0x07;
	m_Motor_CMD_06[4] = 0x01;
	m_Motor_CMD_06[5] = 0x26;		//(byte)((Temp_Motor_Position_01));
	m_Motor_CMD_06[6] = 0x02;		//(byte)((Temp_Motor_Position_01) >> 8);
	m_Motor_CMD_06[7] = 0x00;
	m_Motor_CMD_06[8] = 0x00;
	m_Motor_CMD_06[9] = Motor_CMD_CheckSum(m_Motor_CMD_06);
	UART_Write(UART0, m_Motor_CMD_06, sizeof(m_Motor_CMD_06));
	//===========================================================
	Short_Delay();
}

int main(void)
{
	unsigned int t_Count_01 = 0;
	unsigned int DoSomething = 0;



	SYS_UnlockReg();
	f_SYS_Init();
	SYS_LockReg();

	f_Startup_Init();

	m_User_Action_CMD_00 	= '\0';

    while(1)
    {
    //	Long_Delay();
    //	m_User_Action_CMD_00 = g_UART_00_RX_Result_Byte;
    	if(m_User_Action_CMD_00 == 'r') // Bottom Stop
    	{
    		Reset();
    	}
		if (m_User_Action_CMD_00 == 'f') // Bottom Stop
		{
			Front_Climbing();
		}
		if (m_User_Action_CMD_00 == 'b') // Bottom Stop
		{
			Back_Climbing();
		}
		if (m_User_Action_CMD_00 == 'i') // Bottom Stop
		{
			Running();
		}
		if (m_User_Action_CMD_00 == 'j') // Bottom Stop
		{
			Left();
		}
		if (m_User_Action_CMD_00 == 'l') // Bottom Stop
		{
			Right();
		}
		if (m_User_Action_CMD_00 == 'w') // Bottom Stop
		{
			Little_Running();
		}
		if (m_User_Action_CMD_00 == 'a') // Bottom Stop
		{
			Little_Left();
		}
		if (m_User_Action_CMD_00 == 'd') // Bottom Stop
		{
			Little_Right();
		}
    }

    f_SYS_Exit();

    return 0;
}
