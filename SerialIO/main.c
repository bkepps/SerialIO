#include <Windows.h>
#include <stdio.h>
#include <conio.h>

void main(void)
{
	HANDLE hComm;							// Handle to the Serial port
	char  ComPortName[] = "\\\\.\\COM4";	// Name of the Serial port(May Change) to be opened,
	BOOL  Status;							// Status of the various operations 
	DWORD dwEventMask;						// Event mask to trigger
	char  TempChar;							// Temperory Character
	char  SerialBuffer[256];				// Buffer Containing Rxed Data
	DWORD NoBytesRead;						// Bytes read by ReadFile()
	int i = 0;

	FILE* data;
#pragma warning(suppress : 4996)
	data = fopen("C:\\Users\\bensk\\data.txt", "a");
	if (data == NULL)
		return 1;

	/*---------------------------------- Opening the Serial Port -------------------------------------------*/

	hComm = CreateFile(L"COM4",                  // Name of the Port to be Opened
		GENERIC_READ | GENERIC_WRITE, // Read/Write Access
		0,                            // No Sharing, ports cant be shared
		NULL,                         // No Security
		OPEN_EXISTING,                // Open existing port only
		0,                            // Non Overlapped I/O
		NULL);                        // Null for Comm Devices

	if (hComm == INVALID_HANDLE_VALUE)
		printf("\n    Error! - Port %s can't be opened\n", ComPortName);
	else
		printf("\n    Port %s Opened\n ", ComPortName);

	/*------------------------------ Setting the Parameters for the SerialPort -----------------------------*/

	DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	Status = GetCommState(hComm, &dcbSerialParams);      //retreives  the current settings

	if (Status == FALSE)
		printf("\n    Error! in GetCommState()");

	dcbSerialParams.BaudRate = CBR_9600;      // Setting BaudRate = 9600
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None 

	Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

	if (Status == FALSE)
	{
		printf("\n    Error! in Setting DCB Structure");
	}
	else //If Successfull display the contents of the DCB Structure
	{
		printf("\n\n    Setting DCB Structure Successfull\n");
		printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
		printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
		printf("\n       StopBits = %d", dcbSerialParams.StopBits);
		printf("\n       Parity   = %d", dcbSerialParams.Parity);
	}

	/*------------------------------------ Setting Timeouts --------------------------------------------------*/

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (SetCommTimeouts(hComm, &timeouts) == FALSE)
		printf("\n\n    Error! in Setting Time Outs");
	else
		printf("\n\n    Setting Serial Port Timeouts Successfull");

	/*------------------------------------ Setting Receive Mask ----------------------------------------------*/

	Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

	if (Status == FALSE)
		printf("\n\n    Error! in Setting CommMask");
	else
		printf("\n\n    Setting CommMask successfull\n");


	/*------------------------------------ Setting WaitComm() Event   ----------------------------------------*/

	printf("Waiting for Data Reception\n");
	while (!_kbhit()) {
		Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

		/*-------------------------- Program will Wait here till a Character is received ------------------------*/

		if (Status == FALSE)
		{
			printf("Error! in Setting WaitCommEvent()\n");
		}
		else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
		{
			printf("Characters Received\n");

			do
			{
				Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
				SerialBuffer[i] = TempChar;
				i++;
			} while (NoBytesRead > 0);



			/*------------Printing the RXed String to Console----------------------*/
			int j = 0;
			for (j = 0; j < i - 1; j++)		// j < i-1 to remove the dupliated last character
				if (SerialBuffer[j] != '\n') {
					fprintf(data, "%c", SerialBuffer[j]);
					printf("%c", SerialBuffer[j]);
				}
			i = 0;
		}
	}
	Status = CloseHandle(hComm);//Closing the Serial Port
	printf("\n +==========================================+\n");
}//End of Main