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

	unsigned char* finger_templates[1];
	unsigned int finger_templates_size[1];

	std::streampos size;
	char* inFmd;

	std::ifstream input ("data/finger.bin", std::ios::in|std::ios::binary|std::ios::ate);
	size = input.tellg();
	inFmd = new char[size];
	input.seekg (0, std::ios::beg);
	input.read (inFmd, size);
	input.close();

	printf("Input Size : %u\n", int(size));
	finger_templates[0] = reinterpret_cast<unsigned char*>(inFmd);;
	finger_templates_size[0] = int(size);

	// for(int i=0;i<finger_templates_size[0];i++){
	// 	printf("%u : %x\n", i, finger_templates[0][i]);
	// }

	unsigned char* vFmd;
	unsigned int vFmdSize;

	int results = CaptureFinger("1128264989", hReader, DPFJ_FMD_ANSI_378_2004, &vFmd, &vFmdSize);

	//run identification

	//target false positive identification rate: 0.00001
	//for a discussion of  how to evaluate dissimilarity scores, as well as the statistical validity of the dissimilarity score and error rates, consult the Developer Guide
	// unsigned int falsepositive_rate = DPFJ_PROBABILITY_ONE / 100000;
	// unsigned int nCandidateCnt = 1;
	// DPFJ_CANDIDATE vCandidates[1];
	//
	// int result = dpfj_identify(
	// 	DPFJ_FMD_ANSI_378_2004,
	// 	vFmd,
	// 	vFmdSize,
	// 	0,
	// 	DPFJ_FMD_ANSI_378_2004,
	// 	1,
	// 	finger_templates,
	// 	finger_templates_size,
	// 	falsepositive_rate,
	// 	&nCandidateCnt,
	// 	vCandidates
	// );

	unsigned int falsematch_rate;

	int result = dpfj_compare(
		DPFJ_FMD_ANSI_378_2004,
		vFmd,
		vFmdSize,
		0,
		DPFJ_FMD_ANSI_378_2004,
		finger_templates[0],
		finger_templates_size[0],
		0,
		&falsematch_rate
	);

	printf("result : %u\n", result);
	printf("dissimilarity score: 0x%x.\n", falsematch_rate);
	printf("false match rate: %e.\n\n\n", (double)(falsematch_rate / DPFJ_PROBABILITY_ONE));

	// ::: Close Device and Release Library :::
	CloseReader(hReader);
	dpfpdd_exit();
  return 0;
}
