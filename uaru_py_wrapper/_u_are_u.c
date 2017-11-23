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
  {"identifyFinger", identifyFinger, 0, NULL}
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

  for(unsigned int i=0; i < vFmdSize; i++){
    printf("%u : %x\n", i, vFmd[i]);
  }

  for(unsigned int i=0; i < vFmdSize_reference; i++){
    printf("%u : %x\n", i, vFmd_reference[i]);
  }

  PyObject *out = Py_BuildValue("");
  return out;
}

static PyObject *identifyFinger(PyObject *self, PyObject *args){
  PyObject *out = Py_BuildValue("");
  return out;
}
