#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <dpfpdd.h>
#include <dpfj.h>

#include "device.h"

int CaptureFinger(const char* szFingerName, DPFPDD_DEV hReader, DPFJ_FMD_FORMAT nFtType, unsigned char** ppFt, unsigned int* pFtSize){
	int result = 0;
	*ppFt = NULL;
	*pFtSize = 0;

	//prepare capture parameters and result
	DPFPDD_CAPTURE_PARAM cparam = {0};
	cparam.size = sizeof(cparam);
	cparam.image_fmt = DPFPDD_IMG_FMT_ISOIEC19794;
	cparam.image_proc = DPFPDD_IMG_PROC_NONE;
	cparam.image_res = 500;
	DPFPDD_CAPTURE_RESULT cresult = {0};
	cresult.size = sizeof(cresult);
	cresult.info.size = sizeof(cresult.info);
	//get size of the image
	unsigned int nImageSize = 0;
	result = dpfpdd_capture(hReader, &cparam, 0, &cresult, &nImageSize, NULL);
	unsigned char* pImage = (unsigned char*)malloc(nImageSize);

	while(1){
		//wait until ready
		int is_ready = 0;
		while(1){
			DPFPDD_DEV_STATUS ds;
			ds.size = sizeof(DPFPDD_DEV_STATUS);
			result = dpfpdd_get_device_status(hReader, &ds);
			if(DPFPDD_SUCCESS != result){
				printf("dpfpdd_get_device_status(): %u\n", result);
				break;
			}

			if(DPFPDD_STATUS_FAILURE == ds.status){
				printf("Reader failure: %u\n", DPFPDD_STATUS_FAILURE);
				break;
			}

			if(DPFPDD_STATUS_READY == ds.status || DPFPDD_STATUS_NEED_CALIBRATION == ds.status){
				is_ready = 1;
				break;
			}
		}
		if(!is_ready) break;

		//capture fingerprint
		printf("Put %s on the reader, or press Ctrl-C to cancel...\r\n", szFingerName);
		result = dpfpdd_capture(hReader, &cparam, -1, &cresult, &nImageSize, pImage);
		if(cresult.success){
			//captured
			printf("fingerprint captured,\n");

			//get max size for the feature template
			unsigned int nFeaturesSize = MAX_FMD_SIZE;
			unsigned char* pFeatures = (unsigned char*)malloc(nFeaturesSize);

			//create template
			result = dpfj_create_fmd_from_fid(DPFJ_FID_ISO_19794_4_2005, pImage, nImageSize, nFtType, pFeatures, &nFeaturesSize);

			if(DPFJ_SUCCESS == result){
				*ppFt = pFeatures;
				*pFtSize = nFeaturesSize;
				printf("features extracted.\n\n");
			}

		}
		else{
			//bad capture
			printf("bad capture, quality feedback: 0x%x\n", cresult.quality);
			unsigned int i = 0;
			for(i = 1; i < 0x80000000; i <<= 1){
				switch(cresult.quality & i){
				case 0: break;
				case DPFPDD_QUALITY_TIMED_OUT:            printf("    timeout expired \n"); break;
				case DPFPDD_QUALITY_CANCELED:             printf("    capture was canceled \n"); break;
				case DPFPDD_QUALITY_NO_FINGER:            printf("    not a finger detected \n"); break;
				case DPFPDD_QUALITY_FAKE_FINGER:          printf("    fake finger detected \n"); break;
				case DPFPDD_QUALITY_FINGER_TOO_LEFT:      printf("    finger is too far left on the reader \n"); break;
				case DPFPDD_QUALITY_FINGER_TOO_RIGHT:     printf("    finger is too far right on the reader \n"); break;
				case DPFPDD_QUALITY_FINGER_TOO_HIGH:      printf("    finger is too high on the reader \n"); break;
				case DPFPDD_QUALITY_FINGER_TOO_LOW:       printf("    finger is too low in the reader \n"); break;
				case DPFPDD_QUALITY_FINGER_OFF_CENTER:    printf("    finger is not centered on the reader \n"); break;
				case DPFPDD_QUALITY_SCAN_SKEWED:          printf("    scan is skewed too much \n"); break;
				case DPFPDD_QUALITY_SCAN_TOO_SHORT:       printf("    scan is too short \n"); break;
				case DPFPDD_QUALITY_SCAN_TOO_LONG:        printf("    scan is too long \n"); break;
				case DPFPDD_QUALITY_SCAN_TOO_SLOW:        printf("    speed of the swipe is too slow \n"); break;
				case DPFPDD_QUALITY_SCAN_TOO_FAST:        printf("    speed of the swipe is too fast \n"); break;
				case DPFPDD_QUALITY_SCAN_WRONG_DIRECTION: printf("    direction of the swipe is wrong \n"); break;
				case DPFPDD_QUALITY_READER_DIRTY:         printf("    reader needs cleaning \n"); break;
				default: printf("unknown quality bitmask: 0x%x \n", i); break;
				}
			}
			continue;
		}
		break;
	}

	if(NULL != pImage) free(pImage);
	return result;
}

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

	std::ifstream input ("data/finger");
	size = input.tellg();
	inFmd = new char[size];
	input.seekg (0, std::ios::beg);
	input.read (inFmd, size);
	input.close();

	printf("Input Size : %u\n", int(size));
	finger_templates[0] = reinterpret_cast<unsigned char*>(inFmd);;
	finger_templates_size[0] = int(size);

	printf("Template : %s\n", finger_templates[0]);

	unsigned char* vFmd;
	unsigned int vFmdSize;

	int results = CaptureFinger("1128264989", hReader, DPFJ_FMD_ANSI_378_2004, &vFmd, &vFmdSize);

	printf("Capture : %s\n", vFmd);

	//run identification

	//target false positive identification rate: 0.00001
	//for a discussion of  how to evaluate dissimilarity scores, as well as the statistical validity of the dissimilarity score and error rates, consult the Developer Guide
	// unsigned int falsepositive_rate = DPFJ_PROBABILITY_ONE / 100000;
	// unsigned int nCandidateCnt = 2;
	// DPFJ_CANDIDATE vCandidates[2];
	//
	// int result = dpfj_identify(
	// 	DPFJ_FMD_ANSI_378_2004,
	// 	vFmd,
	// 	vFmdSize,
	// 	0,
	// 	DPFJ_FMD_ANSI_378_2004,
	// 	2,
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


	printf("dissimilarity score: 0x%x.\n", falsematch_rate);
	printf("false match rate: %e.\n\n\n", (double)(falsematch_rate / DPFJ_PROBABILITY_ONE));

	// ::: Close Device and Release Library :::
	CloseReader(hReader);
	dpfpdd_exit();
  return 0;
}