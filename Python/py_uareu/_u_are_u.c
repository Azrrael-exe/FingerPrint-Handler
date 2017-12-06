#include <Python.h>
#include "u_are_u.h"

/* Docstrings */
static char module_docstring[] =
    "This module provides an interface to C.";

/* Available functions */
static PyObject *init_device(PyObject *self, PyObject *args);
static PyObject *getFinger(PyObject *self, PyObject *args);
static PyObject *compareFinger(PyObject *self, PyObject *args);
static PyObject *identifyFinger(PyObject *self, PyObject *args);
static PyObject *enrollFinger(PyObject *self, PyObject *args);

/* Module specification */
static PyMethodDef module_methods[] = {
  {"init_device", init_device, 0, NULL},
  {"getFinger", getFinger, METH_VARARGS, NULL},
  {"compareFinger", compareFinger, METH_VARARGS, NULL},
  {"identifyFinger", identifyFinger, METH_VARARGS, NULL},
  {"enrollFinger", enrollFinger, METH_VARARGS, NULL}
};

DPFPDD_DEV hReader = NULL; 							//handle of the selected reader

/* Initialize the module */
PyMODINIT_FUNC initu_are_u(void){
  PyObject *m = Py_InitModule3("u_are_u", module_methods, module_docstring);
  if (m == NULL) return;
  else {
    dpfpdd_init();     // Initalize Library
    hReader = SelectAndOpenReader();
  }
}

/* Initalize the Device */
static PyObject *init_device(PyObject *self, PyObject *args){
  PyObject *out = Py_BuildValue("i", 1);
  return out;
}

/* Read the finger and get the fmd */
static PyObject *getFinger(PyObject *self, PyObject *args){
  char* id;
  if (!PyArg_ParseTuple(args, "s", &id)) return NULL;
  unsigned char* vFmd;
  unsigned int vFmdSize;
  int result = CaptureFinger(id, hReader, &vFmd, &vFmdSize);
  if(DPFPDD_SUCCESS == result){
    PyObject *out = Py_BuildValue("{s:s,s:s#,s:i}","id", id, "fmd", reinterpret_cast<char const*>(vFmd), vFmdSize, "size", vFmdSize);
    return out;
	}
  PyObject *out = Py_BuildValue("");
  return out;
}

static PyObject *compareFinger(PyObject *self, PyObject *args){
  unsigned char* vFmd;
  unsigned int vFmdSize;
  unsigned char* vFmd_reference;
  unsigned int vFmdSize_reference;

  if (!PyArg_ParseTuple(args, "s#is#i", &vFmd, &vFmdSize, &vFmdSize, &vFmd_reference, &vFmdSize_reference, &vFmdSize_reference)) return NULL;

  unsigned int falsematch_rate;

  int result = dpfj_compare(
    DPFJ_FMD_ANSI_378_2004,
    vFmd,
    vFmdSize,
    0,
    DPFJ_FMD_ANSI_378_2004,
    vFmd_reference,
    vFmdSize_reference,
    0,
    &falsematch_rate
  );

  PyObject *out = Py_BuildValue("{s:i,s:i}","result", result, "false_rate", falsematch_rate);
  return out;
}

static PyObject *identifyFinger(PyObject *self, PyObject *args){

  PyObject *py_tuple;
  int len;
  if (!PyArg_ParseTuple(args, "O", &py_tuple)) return NULL;
  len = PyTuple_Size(py_tuple);

  unsigned char* finger;
  unsigned int finger_size;

  finger = (unsigned char*) PyString_AsString(PyTuple_GetItem(py_tuple , 0));
  finger_size = (int) PyInt_AsLong(PyTuple_GetItem(py_tuple, 1));

  int nFingerCnt = (len/2)-1;

  unsigned int int_array[nFingerCnt];
  unsigned char* char_array[nFingerCnt];

  // --- Chars ---
  for(int i = 0; i < nFingerCnt; i++){
    char_array[i] = (unsigned char*) PyString_AsString(PyTuple_GetItem(py_tuple ,(i*2) + 2));
    // printf("%u : %s\n", i, char_array[i]);
  }

  // --- Longitudes ---
  for(int i = 0; i < nFingerCnt; i++){
    int_array[i] = (int) PyInt_AsLong(PyTuple_GetItem(py_tuple, (i*2) + 3));
    // printf("%u : %u\n", i, int_array[i]);
  }

  unsigned int falsepositive_rate = DPFJ_PROBABILITY_ONE / 100000;
  unsigned int nCandidateCnt = nFingerCnt;
  DPFJ_CANDIDATE vCandidates[nFingerCnt];

  int result = dpfj_identify(
    DPFJ_FMD_ANSI_378_2004,
    finger,
    finger_size,
    0,
    DPFJ_FMD_ANSI_378_2004,
    nFingerCnt,
    char_array,
    int_array,
    falsepositive_rate,
    &nCandidateCnt,
    vCandidates
  );

  if(DPFPDD_SUCCESS == result){
    PyObject *out = Py_BuildValue("{s:i,s:i,s:i}","result", result, "candidates", nCandidateCnt, "index", vCandidates[0].fmd_idx);
    return out;
  }

  // PyObject *out = Py_BuildValue("{s:i}","result", result);
  PyObject *out = Py_BuildValue("{s:i,s:i,s:i}","result", result, "candidates", nCandidateCnt, "index", vCandidates[0].fmd_idx);
  return out;
}

// static PyObject *identifyFinger(PyObject *self, PyObject *args){
//
//   PyObject *py_tuple;Ahi e
//   int len;
//   if (!PyArg_ParseTuple(args, "O", &py_tuple)) return NULL;
//   len = PyTuple_Size(py_tuple);
//
//   printf("Input Length : %u\n", len);
//
//   unsigned char* char_array[(len/2)];
//   unsigned int int_array[(len/2)];
//
//   // --- Chars ---
//   for(int i = 0; i < len/2; i++){
//     char_array[i] = (unsigned char*) PyString_AsString(PyTuple_GetItem(py_tuple ,i*2));
//     // printf("%u : %s\n", i, char_array[i]);
//   }
//
//   // --- Longitudes ---
//   for(int i = 0; i < len/2; i++){
//     int_array[i] = (int) PyInt_AsLong(PyTuple_GetItem(py_tuple, (i*2)+1));
//     // printf("%u : %u\n", i, int_array[i]);
//   }
//
//   unsigned int nFingerCnt = (len/2);
//
//   // printf("nFingerCnt : %u\n", nFingerCnt);
//   // printf("finger_size: %u\n", int_array[nFingerCnt - 1]);
//
//   unsigned int falsepositive_rate = DPFJ_PROBABILITY_ONE / 100000;
//   unsigned int nCandidateCnt = nFingerCnt;
//   DPFJ_CANDIDATE vCandidates[nFingerCnt];
//
//   int result = dpfj_identify(
//     DPFJ_FMD_ANSI_378_2004,
//     char_array[nFingerCnt - 1],
//     int_array[nFingerCnt - 1],
//     0,
//     DPFJ_FMD_ANSI_378_2004,
//     nFingerCnt - 1,
//     char_array,
//     int_array,
//     falsepositive_rate,
//     &nCandidateCnt,
//     vCandidates
//   );
//
//   if(DPFPDD_SUCCESS == result){
//     PyObject *out = Py_BuildValue("{s:i,s:i,s:i}","result", result, "candidates", nCandidateCnt, "index", vCandidates[0].fmd_idx);
//     return out;
//   }
//
//   PyObject *out = Py_BuildValue("{s:i}","result", result);
//   // PyObject *out = Py_BuildValue("{s:i,s:i,s:i}","result", result, "candidates", nCandidateCnt, "index", vCandidates[0].fmd_idx);
//   return out;
// }

static PyObject *enrollFinger(PyObject *self, PyObject *args){
  char* id;
  if (!PyArg_ParseTuple(args, "s", &id)) return NULL;
	int bStop = 0;
	while(!bStop){
    int capture_cnt = 0;
		unsigned char* pFmd = NULL;
		unsigned int nFmdSize = 0;
		printf("Enrollment started\n\n");

		//start the enrollment
		int result = dpfj_start_enrollment(DPFJ_FMD_ANSI_378_2004);
		if(DPFJ_SUCCESS != result){
        PyObject *out = Py_BuildValue("{s:s,s:i}","error", "dpfj_start_enrollment()", "result", result);
        return out;
		}

  	//capture fingers, create templates
  	int bDone = 0;
  	int bFirst = 1;
  	while(!bDone){
  		capture_cnt++;
  		printf("%d\r\n", capture_cnt);

  		//capture finger, create template
  		if(bFirst){
  			bFirst = 0;
  			if(0 != CaptureFinger(id, hReader, &pFmd, &nFmdSize)){
  				bStop = 1;
  				break;
  			}
  		}
  		else{
  			if(0 != CaptureFinger(id, hReader, &pFmd, &nFmdSize)){
  				bStop = 1;
  				break;
  			}
  		}

  		//add template to enrollment
  		result = dpfj_add_to_enrollment(DPFJ_FMD_ANSI_378_2004, pFmd, nFmdSize, 0);

  		//template is not needed anymore
  		free(pFmd);
  		pFmd = NULL;

  		if(DPFJ_E_MORE_DATA == result){
  			//need to add another template
  			continue;
  		}
  		else if(DPFJ_SUCCESS == result){
  			//enrollment is ready
  			bDone = 1;
  			break;
  		}
  		else{
        PyObject *out = Py_BuildValue("{s:s,s:i}","error","dpfj_add_to_enrollment()", "result", result);
        return out;
  		}
  	}

  	//determine size (optional, MAX_FMR_SIZE can be used)
  	unsigned char* pEnrollmentFmd = NULL;
  	unsigned int nEnrollmentFmdSize = 0;

    if(bDone){
			result = dpfj_create_enrollment_fmd(NULL, &nEnrollmentFmdSize);

			if(DPFJ_E_MORE_DATA == result){
				pEnrollmentFmd = (unsigned char*)malloc(nEnrollmentFmdSize);
				if(NULL == pEnrollmentFmd){
					printf("Error : malloc()");
				}
				else{
					result = dpfj_create_enrollment_fmd(pEnrollmentFmd, &nEnrollmentFmdSize);
				}
			}
			if(DPFJ_SUCCESS != result){
				printf("Error : dpfj_create_enrollment_fmd()");
				nEnrollmentFmdSize = 0;
			}

			if(NULL != pEnrollmentFmd && 0 != nEnrollmentFmdSize){
				printf("Enrollment template created, size: %d\n\n\n", nEnrollmentFmdSize);
			}
		}

    result = dpfj_finish_enrollment();
    if(DPFJ_SUCCESS != result){
      PyObject *out = Py_BuildValue("{s:s,s:i}","error","dpfj_finish_enrollment()", "result", result);
      return out;
    }

    PyObject *out = Py_BuildValue("{s:s,s:s#,s:i}","id", id, "fmd", reinterpret_cast<char const*>(pEnrollmentFmd), nEnrollmentFmdSize, "size", nEnrollmentFmdSize);
    return out;
  }
  PyObject *out = Py_BuildValue("");
  return out;
}
