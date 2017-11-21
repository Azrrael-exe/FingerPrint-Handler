#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <dpfpdd.h>
#include <dpfj.h>

DPFPDD_DEV SelectAndOpenReader(char* szReader, size_t nReaderLen){
	DPFPDD_DEV hReader = NULL;
	strncpy(szReader, "", nReaderLen);
	unsigned int nReaderCnt = 1;
	DPFPDD_DEV_INFO* pReaderInfo = (DPFPDD_DEV_INFO*)malloc(sizeof(DPFPDD_DEV_INFO) * nReaderCnt);
	int bStop = 0;
	while(!bStop){
		//enumerate the readers
		while(NULL != pReaderInfo){
			unsigned int i = 0;
			for(i = 0; i < nReaderCnt; i++){
				pReaderInfo[i].size = sizeof(DPFPDD_DEV_INFO);
			}

			unsigned int nNewReaderCnt = nReaderCnt;
			int result = dpfpdd_query_devices(&nNewReaderCnt, pReaderInfo);

			//quit if error
			if(DPFPDD_SUCCESS != result && DPFPDD_E_MORE_DATA != result){
				printf("dpfpdd_query_devices() -> %u\n", result );
				free(pReaderInfo);
				pReaderInfo = NULL;
				nReaderCnt = 0;
				break;
			}

			//allocate memory if needed and do over
			if(DPFPDD_E_MORE_DATA == result){
				DPFPDD_DEV_INFO* pri = (DPFPDD_DEV_INFO*)realloc(pReaderInfo, sizeof(DPFPDD_DEV_INFO) * nNewReaderCnt);
				pReaderInfo = pri;
				nReaderCnt = nNewReaderCnt;
				continue;
			}

			//success
			nReaderCnt = nNewReaderCnt;
			break;
		}
		//list readers
		if(0 != nReaderCnt){
			printf("Available reader:");
			unsigned int i = 0;
			for(i = 0; i < nReaderCnt; i++){
				printf("  %s\n", pReaderInfo[i].name);
				// printf("Index:  %u\n",i);
			}
			bStop = 1;
		}
		// else printf("\n\nNo readers available\n");
	}
	int result = dpfpdd_open(pReaderInfo[0].name, &hReader);
	if(DPFPDD_SUCCESS == result){
		printf("Device Open! \n");
	}
	else{
		printf("Device Failed at Open :/ \n" );
	}
	return hReader;
}

int CaptureFinger(const char* szFingerName, unsigned char** ppFt, unsigned int* pFtSize);
