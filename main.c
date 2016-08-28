/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "stm32f4xx.h"

#define NOP()	asm("nop")

void delay(int a);
void I2CInit();
void GPIOInit();
void i2c_bus_write(uint8_t address, uint8_t *txBuf);
uint8_t i2c_bus_read(uint8_t address, uint8_t *rxbuf);
int isButtonPressed();

uint8_t txData[9] = { 0x70, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 };
uint8_t *ptxbuf;

uint8_t *prxbuf;
uint8_t rxData[9] = { 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

int volatile intNum = 0;

void I2C1_EV_IRQHandler(void) {
	intNum++;
	switch (I2C_GetLastEvent(I2C1)) {
	case I2C_EVENT_MASTER_MODE_SELECT:
		break;
	case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
		break;
	case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
		break;
	case I2C_EVENT_MASTER_BYTE_RECEIVED:
		break;
	case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:
		break;
	default:
		break;
	}
}

int main(void) {

	ptxbuf = txData;
	prxbuf = rxData;

	GPIOInit();
	I2CInit();

	while (1) {
		i2c_bus_write(0xa0, ptxbuf);
		delay(1000000);
		NOP();
		i2c_bus_read(0xa0, prxbuf);
		delay(1000000);
		NOP();
	}
}

uint8_t i2c_bus_read(uint8_t address, uint8_t *rxbuf) {
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		;
	}
	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		;
	I2C_SendData(I2C1, *(rxbuf++));
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		;
	I2C_GenerateSTOP(I2C1, ENABLE);

	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		;
	}
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		;
	for (int i = 1; i < 8; i++) {
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
			;
		*(rxbuf++) = I2C_ReceiveData(I2C1);
	}
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
		;
	*(rxbuf++) = I2C_ReceiveData(I2C1);
	I2C_GenerateSTOP(I2C1, ENABLE);
	return 0x01;
}

void i2c_bus_write(uint8_t address, uint8_t *ptxbuf) {
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		;
	}
	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		;
	I2C_SendData(I2C1, *ptxbuf);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		;

	for (int i = 1; i <= 8; i++) {
		I2C_SendData(I2C1, *(++ptxbuf));
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			;
	}
	I2C_GenerateSTOP(I2C1, ENABLE);
}

void GPIOInit() {

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//	GPIO_DeInit(GPIOC);
	GPIO_InitTypeDef GPIOB_InitStructure;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOB_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIOB_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN | GPIO_PuPd_DOWN;
	GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIOB_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_DeInit(GPIOA);
	GPIO_InitTypeDef GPIOA_InitStructure;
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIOA_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN | GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIOA_InitStructure);
}

void I2CInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	/* Enable I2C clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* Enable GPIOB	 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* Configure PB.10 & PB.11 in alternate function -------------------------*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

	/* Enable I2C1 -------------------------------------------------------------*/
	I2C_DeInit(I2C1);

	/* Init I2C */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x20;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_ClockSpeed = 80000;
	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_EnableIRQ(I2C1_EV_IRQn);
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannel = ENABLE;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_Init(&NVIC_InitStructure);
}

int isButtonPressed() {
	int returnval = 0;
	if (GPIOA->IDR & GPIO_IDR_IDR_0) {
		returnval = 1;
	}
	return returnval;
}

void delay(int a) {
	volatile int i, j;
	for (i = 0; i < a; i++) {
		j++;
	}
	return;
}

