#include <Python.h>
#include "wrapper.h"

/* Docstrings */
static char module_docstring[] =
    "This module provides an interface to C.";
static char response_docstring[] =
    "Retunrs a constant int value.";

/* Available functions */
static PyObject *response(PyObject *self, PyObject *args);

/* Module specification */
static PyMethodDef module_methods[] = {
    {"response", response, METH_VARARGS, response_docstring},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_wrapper(void) {
  PyObject *m = Py_InitModule3("_wrapper", module_methods, module_docstring);
  if (m == NULL) return;
}

static PyObject *response(PyObject *self, PyObject *args){
  int input;
  if (!PyArg_ParseTuple(args, "h", &input)) return NULL;

  int value = res(input);

  PyObject *ret = Py_BuildValue("h", value);
  return ret;
}
