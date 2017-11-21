#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <dpfpdd.h>
#include <dpfj.h>

#include "device.h"

int main(){
	// ::: Initalize Library and Open Device :::
	dpfpdd_init(); 													// Initalize Library
	char szReader[MAX_DEVICE_NAME_LENGTH]; 	//name of the selected reader
	DPFPDD_DEV hReader = NULL; 							//handle of the selected reader
	hReader = SelectAndOpenReader(szReader, sizeof(szReader));

	unsigned char* vFmd;
	unsigned int vFmdSize;

	int results = CaptureFinger("1128264989", hReader, DPFJ_FMD_ANSI_378_2004, &vFmd, &vFmdSize);

	printf("Results : %u\n", results);
	printf("vFmdSize : %u\n", vFmdSize);
	printf("vFmd : %s\n", vFmd);

	std::ofstream output ("data/finger" , std::ios::out | std::ios::binary);
	output.write(reinterpret_cast<const char*>(&vFmd), vFmdSize);
	output.close();

	std::streampos size;
	char* inFmd;

	std::ifstream input ("data/finger", std::ios::in|std::ios::binary|std::ios::ate);
	size = input.tellg();
	inFmd = new char[size];
	input.seekg (0, std::ios::beg);
	input.read (inFmd, size);
	input.close();

	//printf("vFmdSize : %u\n", size);
	printf("vFmd : %s\n", inFmd);

	// int result = dpfj_identify(
	// 	DPFJ_FMD_ANSI_378_2004, 			// Format
	// 	vFmd[nFingerCnt - 1],
	// 	vFmdSize[nFingerCnt - 1],
	// 	0,
	// 	DPFJ_FMD_ANSI_378_2004,
	// 	nFingerCnt - 1,
	// 	vFmd,
	// 	vFmdSize,
	// 	falsepositive_rate,
	// 	&nCandidateCnt,
	// 	vCandidates
	// );


	// ::: Close Device and Release Library :::
	CloseReader(hReader);
	dpfpdd_exit();
  return 0;
}
