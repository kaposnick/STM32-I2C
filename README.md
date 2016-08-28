# STM32-I2C

This is a tutorial on I2C communication protocol between a STM32F401RE-Nucleo development board and a PCF8598 EEPROM.

We write to a specific memory page 8 bytes. There is a ~10ms delay and we read from those same memory cells and store the result in rxBuffer.

Write Procedure:
-   GENERATE START
-    SEND 7-Bit ADDRESS ( in our case 0x50 )
-    SEND DESTINATION REGISTER ADDRESS ( memory cell ) ( in our case 0x70 - 0x77 )
-    SEND 8 BYTES
-    GENERATE STOP

Read Procedure:
-    GENERATE START
-    SEND 7-Bit ADDRESS 
-    SEND DESTINATION REGISTER ADDRESS ( to read from ) 
-    SEND STOP BIT
-    GENERATE START
-    SEND 7-Bit ADDRESS ( as slave )
-    RECEIVE 8 BYTES
-    GENERATE STOP


