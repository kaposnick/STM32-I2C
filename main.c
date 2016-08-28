#include "stm32f4xx.h"

#define NOP()	asm("nop")

#define memoryCell	0x70
#define eepromAddress	0x50

void delay(int a);
void I2CInit();
void i2c_bus_write(uint8_t address, uint8_t *txBuf);
uint8_t i2c_bus_read(uint8_t address, uint8_t *rxbuf);

uint8_t txData[9] = { memoryCell , 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 };
uint8_t *ptxbuf;

uint8_t *prxbuf;
uint8_t rxData[9] = { memoryCell , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

int main(void) {

	ptxbuf = txData;
	prxbuf = rxData;

	I2CInit();

	while (1) {
		i2c_bus_write(eepromAddress, ptxbuf);
		delay(1000000);
		NOP();
		i2c_bus_read(eepromAddress, prxbuf);
		delay(1000000);
		NOP();
	}
}

uint8_t i2c_bus_read(uint8_t address, uint8_t *rxbuf) {
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		;
	}
	I2C_Send7bitAddress(I2C1, address >> 1, I2C_Direction_Transmitter);
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

void I2CInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	/* Enable I2C clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* Enable GPIOB	 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* Configure PB.9 & PB.7 in alternate function -------------------------*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

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
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
}

void delay(int a) {
	volatile int i, j;
	for (i = 0; i < a; i++) {
		j++;
	}
	return;
}

