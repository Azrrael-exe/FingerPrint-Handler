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

	unsigned char* finger_template;
	unsigned int finger_template_size;

	int results = CaptureFinger("1128264989", hReader, DPFJ_FMD_ANSI_378_2004, &finger_template, &finger_template_size);

	printf("Results : %u\n", results);
	printf("vFmdSize : %u\n", finger_template_size);

	std::ofstream output ("data/finger");
	output.write(reinterpret_cast<char*>(&finger_template), finger_template_size);
	output.close();

	// ::: Close Device and Release Library :::
	CloseReader(hReader);
	dpfpdd_exit();
	printf("Capture Done!\n");
  return 0;
}
