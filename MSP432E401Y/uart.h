//Initialize UART0, based on textbook.  Clock code modified.
static char printf_buffer[1023];
void UART_Init(void);

// Wait for new input, then return ASCII code 
	char UART_InChar(void);
	// Wait for buffer to be not full, then output 
	void UART_OutChar(char data);
	void UART_printf(const char* array);
	void Status_Check(char* array, int status);