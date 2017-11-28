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

/* Module specification */
static PyMethodDef module_methods[] = {
  {"init_device", init_device, 0, NULL},
  {"getFinger", getFinger, METH_VARARGS, NULL},
  {"compareFinger", compareFinger, METH_VARARGS, NULL},
  {"identifyFinger", identifyFinger, METH_VARARGS, NULL}
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

  PyObject *out = Py_BuildValue("i", falsematch_rate);
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

  unsigned int int_array[(len/2)-1];
  unsigned char* char_array[(len/2)-1];

  // printf("len : %u\n", len);

  // --- Chars ---
  for(int i = 1; i < len/2; i++){
    char_array[i] = (unsigned char*) PyString_AsString(PyTuple_GetItem(py_tuple ,i*2));
    // printf("%u : %s\n", i, char_array[i]);
  }

  // --- Longitudes ---
  for(int i = 1; i < len/2; i++){
    int_array[i] = (int) PyInt_AsLong(PyTuple_GetItem(py_tuple, (i*2)+1));
    // printf("%u : %u\n", i, int_array[i]);
  }


  unsigned int falsepositive_rate = DPFJ_PROBABILITY_ONE / 100000;
  unsigned int nFingerCnt = (len/2)-1;
  unsigned int nCandidateCnt = (len/2)-1;
  DPFJ_CANDIDATE vCandidates[(len/2)-1];

  int result = dpfj_identify(
    DPFJ_FMD_ANSI_378_2004, finger, finger_size, 0,
    DPFJ_FMD_ANSI_378_2004,
    nFingerCnt, char_array, int_array,
    falsepositive_rate,
    &nCandidateCnt,
    vCandidates
  );

  if(DPFPDD_SUCCESS == result){
    PyObject *out = Py_BuildValue("{s:i,s:i,s:i}","result", result, "candidates", nCandidateCnt, "index", vCandidates[0].fmd_idx);
    return out;
  }

  PyObject *out = Py_BuildValue("");
  return out;
}
