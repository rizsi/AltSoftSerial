/** 
Simple UART0 initialization and access code. Used for simple debugging.
Not used by the library itself only be the examples.
*/
// Debug send a byte by waiting for send possible
void UART0_Send(uint8_t data);
// Debug send a null terminated string by waiting for send possible
void UART0_Send_str(char * str);
// Send data as binary 0s and 1s
void UART0_Send_Bin(uint8_t data);
/// Send number in decimal format @return number of characters written
uint8_t UART0_Send_uint32(uint32_t v);
/// Send number in decimal format @return number of characters written
uint8_t UART0_Send_int32(int32_t v);
/// Initialize UART0 hardware and start receiving.
void UART0_Init();

/// Read data - blocking call, not buffered other than the 1 byte hardware receiver
uint8_t UART0_read();
/// Is there a byte in the UART0 receiver byte?
uint8_t UART0_available();

