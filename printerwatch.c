#include <stdio.h>
#include <windows.h>
#include <winspool.h>
// Links: http://winapi.freetechsecrets.com/win32/WIN32EnumPrinters.htm

int main(int argc, char **argv)
{
	PRINTER_INFO_2* list = NULL;
	DWORD cnt = 0;
	DWORD sz = 0;
	DWORD Level = 2;
	int sleepTime = 500;
	BOOL debug = FALSE;
	
	if (argc != 3) {
		printf("%s <sleepTime> <debugOrNot>", argv[0]);
		return 1;
	}
	sleepTime = atoi(argv[1]);
	debug = atoi(argv[2]);
	
	// stupid to get malloc value
	/*BOOL EnumPrinters(
		DWORD Flags, // types of printer objects to enumerate 
		LPTSTR Name, // name of printer object 
		DWORD Level, // specifies type of printer info structure 
		LPBYTE pPrinterEnum, // pointer to buffer to receive printer info structures 
		DWORD cbBuf, // size, in bytes, of array 
		LPDWORD pcbNeeded, // pointer to variable with no. of bytes copied (or required) 
		LPDWORD pcReturned // pointer to variable with no. of printer info. structures copied 
	); */
	// PRINTER_ENUM_LOCAL or PRINTER_ENUM_CONNECTIONS...
	EnumPrinters(PRINTER_ENUM_CONNECTIONS, NULL, Level, (LPBYTE)list, 0, &sz, &cnt );
	if ((list = (PRINTER_INFO_2*) malloc( sz )) == 0 ) return 1;
	
	if (!EnumPrinters(PRINTER_ENUM_CONNECTIONS, NULL, Level, (LPBYTE)list, sz, &sz, &cnt )) {
		printf("[-] EnumPrinters failed! Aborting!\r\n");
		return 1;
	}
	printf("[+] EnumPrinters success!\r\n");
	printf("[+] Bytes received %ld\r\n", sz);
	printf("[+] Printers received %ld\r\n", cnt);
	
	char lastJobName[255];
	memset(lastJobName, '\0', sizeof(lastJobName));
	while ("alex") {
		int i;
		
		for (i = 0; i < (int)cnt; i++) {
		  if (debug) printf(" [+] Checking printer %d - %s\r\n",i, list[i].pPrinterName);
		  
			// open the printer
			HANDLE hPrinter;
			/*
			 * BOOL OpenPrinter(
				LPTSTR pPrinterName, // pointer to printer or server name 
				LPHANDLE phPrinter, // pointer to printer or server handle 
				LPPRINTER_DEFAULTS pDefault // pointer to printer defaults structure 
				); 
			 */
			if(!OpenPrinter(list[i].pPrinterName, &hPrinter, NULL)) {
				printf(" [-] Error opening printer %s\r\n", list[i].pPrinterName);
				//return 1;
			}
			
			// get the jobs - https://msdn.microsoft.com/en-us/library/windows/desktop/dd162625(v=vs.85).aspx
			/*BOOL EnumJobs(
			  _In_  HANDLE  hPrinter,
			  _In_  DWORD   FirstJob,
			  _In_  DWORD   NoJobs,
			  _In_  DWORD   Level,
			  _Out_ LPBYTE  pJob,
			  _In_  DWORD   cbBuf,
			  _Out_ LPDWORD pcbNeeded,
			  _Out_ LPDWORD pcReturned
			);
			  typedef struct _JOB_INFO_1 {
			  DWORD      JobId;
			  LPTSTR     pPrinterName;
			  LPTSTR     pMachineName;
			  LPTSTR     pUserName;
			  LPTSTR     pDocument;
			  LPTSTR     pDatatype;
			  LPTSTR     pStatus;
			  DWORD      Status;
			  DWORD      Priority;
			  DWORD      Position;
			  DWORD      TotalPages;
			  DWORD      PagesPrinted;
			  SYSTEMTIME Submitted;
			} JOB_INFO_1, *PJOB_INFO_1;*/
			
			JOB_INFO_1* pJob = malloc(1024*1024);
			DWORD pcbNeeded, pcReturned;
			if (!EnumJobs(hPrinter, 0, 100, 1, (LPBYTE)pJob, 1024*1024, &pcbNeeded, &pcReturned)) {
				printf(" [-] Error enumerating jobs\r\n");
				return 1;
			}
			if (debug) printf(" [+] Jobs received %lu\r\n", pcReturned);
			int j;
			for (j=0; j<pcReturned; j++) {
				//printf ("1 - %s\r\n2 - %s\r\n", lastJobName, pJob[j].pDocument);
				if (strcmp(lastJobName, pJob[j].pDocument) != 0) {
					if (debug) printf("  [+] Document:    %s\r\n", pJob[j].pDocument);
					else {
						printf("%s,", pJob[j].pPrinterName);
						printf("%s,", pJob[j].pMachineName);
						printf("%s,", pJob[j].pUserName);
						printf("%s,", pJob[j].pDocument);
						printf("%ld\r\n", pJob[j].TotalPages);
					}
				}
				strcpy(lastJobName, pJob[j].pDocument);
				//printf ("1 - %s\r\n2 - %s\r\n", lastJobName, pJob[j].pDocument);
			}
			free(pJob);
		}
		Sleep(sleepTime);
	}
	return 0;
}
