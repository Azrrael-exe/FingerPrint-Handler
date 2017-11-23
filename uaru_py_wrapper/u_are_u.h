#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <string.h>

#include <dpfpdd.h>
#include <dpfj.h>

DPFPDD_DEV SelectAndOpenReader();

int CaptureFinger(const char* szFingerName, DPFPDD_DEV hReader, unsigned char** ppFt, unsigned int* pFtSize);
