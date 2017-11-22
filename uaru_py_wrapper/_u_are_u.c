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

/* Initialize the module */
// PyMODINIT_FUNC init_u_are_u(void){
//   PyObject *m = Py_InitModule3("_u_are_u", module_methods, module_docstring);
//   if (m == NULL) return;
// }

/* Initialize the module */
PyMODINIT_FUNC initu_are_u(void){
  PyObject *m = Py_InitModule3("u_are_u", module_methods, module_docstring);
  if (m == NULL) return;
}

static PyObject *getFinger(PyObject *self, PyObject *args){
  char* id;
  if (!PyArg_ParseTuple(args, "s", &id)) return NULL;

  PyObject *out = Py_BuildValue("{s:i,s:i}","abc", 123, "def", 456);
  return out;
}
