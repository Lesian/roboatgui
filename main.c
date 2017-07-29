/***************************************************************************************************************/
/******************************************Wei Wang************************************************/
/****************************************Thruster Test************************************************/
/******************************************2016.11.21************************************************/
/***************************************************************************************************************/
#include "stm32f10x.h"
#include "configuration.h"
#include "PWM_Update.h"
#include "I2C_EEPROM.h"
#include "PWM_Calculation.h"
#include "delay.h"
#include "main.h"
#include "math.h"
#include "string.h"

//�����������ID��
#define STARTBYTE 0xAA
#define ENDBYTE 0xFC

#define UARTHEARDER_NUMBER 5 //��������ĳ���


float LeftRotationalSPeed = 0;//��������ת�ٷ�Χ��-375~375,��Ӧ���ת�ٶ�1100us�������ת�ٶȵ�ʱ��1900us�� 
float RightRotationalSPeed = 0;
float FrontRotationalSPeed = 0;
float RearRotationalSPeed = 0;

float RotationalSpeed[4] = { 0 };
int INTSPEED;
int UARTCOMMAND_TOTALNUMBER=UARTHEARDER_NUMBER;
char CommandDataLength;
char SensorDataLength;


struct 
{
	char StartByte;
	char RobotID;
	char CommandType; 
	char DataLength;
	/************Data Area**************/
	float ForceData[4];// The forces received from the higher-level controller
	char LatchingCommand[4];
	char SensorCommand[8]; 
  /************Data Area**************/
	char EndByte;
} ReceivingPacket;

struct 
{
	char StartByte;
	char RobotID;
	char CommandType; 
	char DataLength;
	/************Data Area**************/
	float GPSData[3];
	float IMUData[6];
  /************Data Area**************/
	char EndByte;
} SendingPacket;


//struct
//{
//	float ForceData[4];//The forces received from the higher-level controller
//	char LatchingCommand[4];
//	char SensorCommand[8]; 
//} CommandData;



//struct  
//{ 
//	char DataLength; 
//	float GPSData[3];//The forces received from the higher-level controller
//	float IMUData[6];
//} SensorData;

extern int PWM_Value[4];

unsigned char UpdateRateFlag;//CPG�������͵ı�־λ

char Thruster_Init_Flag = 0;  //��������ʼ����־λ
int Thruster_Init_TimeCount = 0;  //��������ʼ��ʱ�ļ�ʱ������λ
char UARTSendCount = 0;

/*Parameters for Wireless Communication*/
unsigned char UART2CommandData;
unsigned char UARTCommandDataBuffer[100];
unsigned char UARTSendDataBuffer[200]; // The matrix to store the sending byte data
unsigned char UARTCommandCount = 0;	//�洢ARTCommandDataBuffer�����õļ���
unsigned char UARTCommandFinishFlag = 0;	//���յ�����Զ���������ݲ��Ҵ洢�б���ɺ�ı�־λ
char temp4byte[4];
float FloatCommand;


/*Parameters for IMU*/
unsigned char IMUBYTENUMBER=28;
char IMUTEST[28];

unsigned char IMUTestCount = 0;	//�洢ARTCommandDataBuffer�����õļ���
unsigned char IMUCount = 0;	//�洢ARTCommandDataBuffer�����õļ���
unsigned char IMUCommandFinishFlag;
unsigned char UART3CommandData;
unsigned char IMUResponceFlag;
unsigned char IMULengthFlag;
float RobotStateIMU[13];


/*Parameters for Indoor GPS*/
unsigned char INDOORGPS_BYTENUMBER=29;
char INDOORGPSBYTE[29];

unsigned char INDOORGPSBYTECount = 0;	//�洢ARTCommandDataBuffer�����õļ���
unsigned char INDOORGPSCommandFinishFlag;
unsigned char UART1CommandData;
unsigned char INDOORGPSDestinationAddressFlag;
unsigned char INDOORGPSSecondByteFlag;
unsigned char INDOORGPSFrameTypeFlag;
int32_t RobotStateGPS[4];
uint16_t IndoorGPSFrameType;
char TempInt2byte[2];
char IndoorGPSIntDataCount=0;

unsigned char* f2bdata;	  //ָ�����ݴ��ݸ�����
float ForceData[4];//The forces received from the higher-level controller
float StateData[9];//The state sent to the higher-level controller: Time, x, y, angular velocity, yaw, pitch, roll, linear ax, linear ay;
char StateDataSendingFreqFlag;
char State_Num = 9;
char ReceiveForceDataFlag = 0;
char StateCount = 0;
char FloatToByteCount = 0;
char FrameByteCount = 0;

char ReceiveForceCount = 0;
char ReceiveByteCount = 0;





int main(void)
{

	RCC_Configuration();//ʱ�ӳ�ʼ��
	GPIO_Configuration();//GPIO�˿ڳ�ʼ��
	TIM_Configuration();//����Thruster��TIM3��ʼ��
	USART_Configuration(); //UART initialize
	Systick_Configuration();//ϵͳ��ʱ����ʼ������������20msһ�ε�UpdateRateFlag
	NVIC_Configuration();		//�ж�����
	//IMU Initialization
	//I2C_Configuration();	//����ݮ�ɽ���Ӳ��I2Cͨ�ŵ��豸��ʼ��
	//CPG_Configuration();   //��EEPROM�����ʼ����;
	//IR_ADC_Config();	//AD������ʼ���������к���͵�Դ����ģ�������
	//EXTI_Configuration();	//�ⲿ�жϳ�ʼ�� 
	PWM_Calculation(LeftRotationalSPeed, RightRotationalSPeed, FrontRotationalSPeed, RearRotationalSPeed); //��ʼ��ʱ�ĸ����������ٶȶ�Ϊ�㣬�����־�ֹ��

	while (1)
	{
		if (UpdateRateFlag == 1)	//Every 20ms UpdateRateFlag=1 to update the Motor signal.
		{
			if (Thruster_Init_Flag == 1) //If the thruster initialization is finished
			{
				
				
				/********************************UART2 Receiving command from or sending states to high-level controller  *************************************/
				/********************************UART2 Receiving command from or sending states to high-level controller  *************************************/
				if (UARTCommandFinishFlag == 1)
				{
					
					UARTCommandFinishFlag = 0;
				}
				/********************************UART2 Receiving command from or sending states to high-level controller  *************************************/
				/********************************UART2 Receiving command from or sending states to high-level controller  *************************************/
				
				
				/****************************** UART1 reading data from indoor GPS ultrasonic locolization system.**********************************/
				/****************************** UART1 reading data from indoor GPS ultrasonic locolization system.**********************************/
				if(INDOORGPSCommandFinishFlag==1)
				{
					INDOORGPSCommandFinishFlag=0;
					temp4byte[0] = INDOORGPSBYTE[5];
					temp4byte[1] = INDOORGPSBYTE[6];
					temp4byte[2] = INDOORGPSBYTE[7];
					temp4byte[3] = INDOORGPSBYTE[8];
			    RobotStateGPS[0] = Receive_byte_to_uint32(temp4byte); //The FloatCommand here is the Direction Freference of the robot.
					for(IndoorGPSIntDataCount=0;IndoorGPSIntDataCount<3;IndoorGPSIntDataCount++)
					{
						temp4byte[0] = INDOORGPSBYTE[IndoorGPSIntDataCount*4+9];
						temp4byte[1] = INDOORGPSBYTE[IndoorGPSIntDataCount*4+10];
						temp4byte[2] = INDOORGPSBYTE[IndoorGPSIntDataCount*4+11];
						temp4byte[3] = INDOORGPSBYTE[IndoorGPSIntDataCount*4+12];
						RobotStateGPS[IndoorGPSIntDataCount+1] = Receive_byte_to_int32(temp4byte); //The FloatCommand here is the Direction Freference of the robot.
					}					
					//USART_printf(USART2, "Time,x, y, and z are %d,%d,%d,%d\r\n",RobotStateGPS[0],RobotStateGPS[1],RobotStateGPS[2],RobotStateGPS[3]);
				  SendingPacket.GPSData[0]=RobotStateGPS[0];
			    SendingPacket.GPSData[1]=RobotStateGPS[1];
				  SendingPacket.GPSData[2]=RobotStateGPS[2];

				}
				/****************************** UART1 reading data from indoor GPS ultrasonic locolization system.**********************************/
				/****************************** UART1 reading data from indoor GPS ultrasonic locolization system.**********************************/
				
				
				/********************************* UART3 reading data from IMU***************************************/
				/********************************* UART3 reading data from IMU***************************************/
				if(IMUCommandFinishFlag==1)
				{
					IMUCommandFinishFlag=0;
					RobotStateIMU[2] =(((int16_t)(IMUTEST[7] << 8)) | IMUTEST[6])/16.0;//Angular Velocity
					RobotStateIMU[3] =(((int16_t)(IMUTEST[9] << 8)) | IMUTEST[8])/16.0;//Yaw
					RobotStateIMU[4] =(((int16_t)(IMUTEST[11] << 8)) | IMUTEST[10])/16.0;//Roll	
					RobotStateIMU[5] =(((int16_t)(IMUTEST[13] << 8)) | IMUTEST[12])/16.0;//Pitch
					RobotStateIMU[10] =(((int16_t)(IMUTEST[23] << 8)) | IMUTEST[22])/16.0;//Linear Ax	
					RobotStateIMU[11] =(((int16_t)(IMUTEST[25] << 8)) | IMUTEST[24])/16.0;//Linear Ay
					//USART_printf(USART2, "Angular velocity, yaw, roll and pitch are %d,%d,%d,%d\r\n",(signed int)(RobotStateIMU[2]),(signed int)(RobotStateIMU[3]),(signed int)(RobotStateIMU[4]),(signed int)(RobotStateIMU[5]));
			    SendingPacket.IMUData[0]=RobotStateIMU[2];
				  SendingPacket.IMUData[1]=RobotStateIMU[3];
					SendingPacket.IMUData[2]=RobotStateIMU[4];
				  SendingPacket.IMUData[3]=RobotStateIMU[5];
					SendingPacket.IMUData[4]=RobotStateIMU[10];
				  SendingPacket.IMUData[5]=RobotStateIMU[11];
				}
				IMUTestCount++;
				if(IMUTestCount==5)//IMU Data Update Frequency is setting to 10Hz now, request data from IMU by sending the commnand
				{   
					USART_SendData(USART3,0XAA);
				  while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
				  USART_SendData(USART3,0X01);
				  while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
					USART_SendData(USART3,0X14);//Start Address: 0x14
				  while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
					USART_SendData(USART3,0X1A);//Read 26 Byte DATA: angular velocity, Eule angle, Quaemion data and linear acceleration.
				  while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );							
				IMUTestCount=0;		
				}
				/********************************* UART3 reading data from IMU***************************************/
				/********************************* UART3 reading data from IMU***************************************/
				
				
				
				/********************************* State data sending to the High-level controller***************************************/
				/********************************* State data sending to the High-level controller***************************************/
				/********************************* State data sending to the High-level controller***************************************/				
       StateDataSendingFreqFlag++;
				if (StateDataSendingFreqFlag == 10) // Send the states to high level controller
					{
						
					}
				/********************************* State data sending to the High-level controller***************************************/
				/********************************* State data sending to the High-level controller***************************************/
				/********************************* State data sending to the High-level controller***************************************/		
				
				UpdateRateFlag = 0;
			}
			else //If the thruster initialization is not finished, continue to wait
			{
				UpdateRateFlag = 0;
				Thruster_Init_TimeCount = Thruster_Init_TimeCount + 1;
				if (Thruster_Init_TimeCount == 150)
				{
					Thruster_Init_Flag = 1;
					//USART_printf(USART2, " Thruster initialized, ready to move.\r\n");
				}
			}

		}//if (UpdateRateFlag == 1)
	}//while (1)
}

/*
********************************************************************************
** �������� �� SysTickHandler(void)
** �������� �� Systick��ʱ���жϷ�����
********************************************************************************
*/
void SysTick_Handler(void)
{
	PWM_LEFT(PWM_Value[0]);
	PWM_RIGHT(PWM_Value[1]);
	PWM_FRONT(PWM_Value[2]);
	PWM_REAR(PWM_Value[3]);
	UpdateRateFlag = 1;
}


/*
********************************************************************************
** �������� �� USART1_IRQHandler(void)
** �������� �� USART1�жϷ������
** ���幦��˵����Receive Indoor GPS Data
********************************************************************************
*/
void USART1_IRQHandler(void)                	//����2�жϷ������
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_ClearFlag(USART1, USART_FLAG_RXNE);	  	  		//����жϱ�־λ
		UART1CommandData = USART_ReceiveData(USART1); 	//(USART1->DR);	��ȡ���յ�������	

		if(INDOORGPSFrameTypeFlag==1)
		{
			INDOORGPSBYTE[INDOORGPSBYTECount++] = UART1CommandData;
			if(INDOORGPSBYTECount == INDOORGPS_BYTENUMBER)
			{ 
					INDOORGPSFrameTypeFlag=0;
					INDOORGPSBYTECount=0;
				  INDOORGPSCommandFinishFlag=1;
			}
		}
		else if(INDOORGPSDestinationAddressFlag==1 && INDOORGPSSecondByteFlag==1)
		{
			INDOORGPSBYTE[INDOORGPSBYTECount++] = UART1CommandData;
			if(INDOORGPSBYTECount == 4)
			{ 
				TempInt2byte[0]=INDOORGPSBYTE[2];
				TempInt2byte[1]=INDOORGPSBYTE[3];
				IndoorGPSFrameType=Receive_byte_to_uint16(TempInt2byte);
				if(IndoorGPSFrameType==0x0011)
				{
					INDOORGPSDestinationAddressFlag=0;
					INDOORGPSSecondByteFlag=0;
					INDOORGPSFrameTypeFlag=1;
				}
				else
				{
					INDOORGPSDestinationAddressFlag=0;
					INDOORGPSSecondByteFlag=0;
					INDOORGPSFrameTypeFlag=0;
					INDOORGPSBYTECount=0;
				}
			}
		}
		else if(INDOORGPSDestinationAddressFlag==1)
		{
				if(UART1CommandData==0x47) //The length of the IMU DATA
				{
					INDOORGPSSecondByteFlag=1;
					INDOORGPSBYTE[INDOORGPSBYTECount++] = UART1CommandData;
				}
				else
				{
					INDOORGPSDestinationAddressFlag=0;
					INDOORGPSBYTECount=0;
				}
		}
		else if(UART1CommandData==0xFF) //The Destination Address of the Indoor GPS DADA
		{
		    INDOORGPSDestinationAddressFlag=1;
        INDOORGPSBYTE[INDOORGPSBYTECount++] = UART1CommandData;
		}
		
	}
}



/*
********************************************************************************
** �������� �� USART2_IRQHandler(void)
** �������� �� USART2�жϷ������
** ���幦��˵����Receive Wireless Command
********************************************************************************
*/
void USART2_IRQHandler(void)                	
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_ClearFlag(USART2, USART_FLAG_RXNE);	  	  		//����жϱ�־λ
		UART2CommandData = USART_ReceiveData(USART2); 							  	 //(USART3->DR);	��ȡ���յ�������			
		UARTCommandDataBuffer[UARTCommandCount++] = UART2CommandData;	//�����ڽ��յ����ݴ���Instruction_Packet��
		if (UARTCommandCount == UARTCOMMAND_TOTALNUMBER) // If recive all the byte data in the packet
		{
			UARTCommandCount = 0; //Recieive all the data, reset UARTCommandCount flat to be ready for reciving next packet 
			ReceivingPacket.StartByte=UARTCommandDataBuffer[0];
			ReceivingPacket.EndByte=UARTCommandDataBuffer[UARTCOMMAND_TOTALNUMBER-1];
			ReceivingPacket.RobotID=UARTCommandDataBuffer[1];
			ReceivingPacket.CommandType=UARTCommandDataBuffer[2];
			ReceivingPacket.DataLength=UARTCommandDataBuffer[3];
			UARTCOMMAND_TOTALNUMBER=UARTHEARDER_NUMBER;//Reset the UARTCOMMAND_TOTALNUMBER to UARTHEARDER_NUMBER.
		}
		else if (UARTCommandCount == (UARTHEARDER_NUMBER-1))
		{ 
			CommandDataLength=UART2CommandData;
			UARTCOMMAND_TOTALNUMBER=UARTHEARDER_NUMBER+CommandDataLength;
		}
	}
}
/*
********************************************************************************
** �������� �� USART3_IRQHandler(void)
** �������� �� USART3�жϷ������
** ���幦��˵��: Reading Data from IMU
********************************************************************************
*/
void USART3_IRQHandler(void)                	//����3�жϷ������
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_ClearFlag(USART3, USART_FLAG_RXNE);	  	  		//����жϱ�־λ	
		UART3CommandData = USART_ReceiveData(USART3);
		
		if(IMUResponceFlag==1 && IMULengthFlag==1)
		{
			IMUTEST[IMUCount++] = UART3CommandData;
			if(IMUCount == IMUBYTENUMBER)
			{ 
				IMUCount=0;
				IMUResponceFlag=0;
				IMULengthFlag=0;
				IMUCommandFinishFlag=1;
			}
		}
		else
		{ 
			if(IMUResponceFlag==1)
			{
				if(UART3CommandData==(IMUBYTENUMBER-2)) //The length of the IMU DATA
				{
					IMULengthFlag=1;
					IMUTEST[IMUCount++] = UART3CommandData;
				}
			}
		  else if(UART3CommandData==0xBB) //THe ResponseByte of the IMU DADA
		  {
		    IMUResponceFlag=1;
        IMUTEST[IMUCount++] = UART3CommandData;
		  }
	  }
	}
}

