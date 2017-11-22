#include <Python.h>
#include "u_are_u.h"

/* Docstrings */
static char module_docstring[] =
    "This module provides an interface to C.";
static char response_docstring[] =
    "Retunrs a constant int value.";

/* Available functions */
static PyObject *getFinger(PyObject *self, PyObject *args);

/* Module specification */
static PyMethodDef module_methods[] = {
    {"getFinger", getFinger, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};


char szReader[MAX_DEVICE_NAME_LENGTH]; 	//name of the selected reader
DPFPDD_DEV hReader = NULL; 							//handle of the selected reader
DPFPDD_DEV SelectAndOpenReader(char* szReader, size_t nReaderLen){
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

/* Initialize the module */
PyMODINIT_FUNC initu_are_u(void){
  PyObject *m = Py_InitModule3("u_are_u", module_methods, module_docstring);
  if (m == NULL) return;
  else {
    dpfpdd_init();     // Initalize Library
    // char szReader[MAX_DEVICE_NAME_LENGTH]; 	//name of the selected reader
    // DPFPDD_DEV hReader = NULL; 							//handle of the selected reader
    hReader = SelectAndOpenReader(szReader, sizeof(szReader));
  }
}

static PyObject *getFinger(PyObject *self, PyObject *args){
  char* id;
  if (!PyArg_ParseTuple(args, "s", &id)) return NULL;

  unsigned char* vFmd;
  unsigned int vFmdSize;

  int result = CaptureFinger(id, hReader, &vFmd, &vFmdSize);
  // std::string s(reinterpret_cast<char const*>(vFmd), vFmdSize);
  std::string s(reinterpret_cast<char const*>(vFmd), vFmdSize);
  PyObject *out = Py_BuildValue("{s:s#,s:i}", "fmd",vFmd, vFmdSize, "size", vFmdSize);
  return out;
}
