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

	unsigned char* finger_template;
	unsigned int finger_template_size;

	int results = CaptureFinger("1128264989", hReader, DPFJ_FMD_ANSI_378_2004, &finger_template, &finger_template_size);

	printf("Results : %u\n", results);
	printf("vFmdSize : %u\n", finger_template_size);

	// for(int i=0; i<finger_template_size; i++){
	// 	printf("%u : %x\n", i, finger_template[i]);
	// }

	std::ofstream out ("data/finger.bin", std::ios::out | std::ios::binary);
	std::string s(reinterpret_cast<char const*>(finger_template), finger_template_size) ;
	out << s;
	out.close();

	// ::: Close Device and Release Library :::
	CloseReader(hReader);
	dpfpdd_exit();
	printf("Capture Done!\n");
  return 0;
}
