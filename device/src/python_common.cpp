#include "python_common.h"

#if TARGET_HW_PYTHON_CAPABLE
/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: PythonAssert
 * If the passed PyObject pointer is null, the function checks if there were 
 * any Python errors and prints the error. This function is meant to handle
 * checking that PyObject pointers returned from the Python/C api are valid.
 * 
 * RETURN:
 * bool - True if passed object is valid. Otherwise, no return due to infinite loop.
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
bool PythonAssert(PyObject* obj)
{
    if (obj == NULL)
    {
        // Collect the error
        PyObject *ptype, *pvalue, *ptraceback;      
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);

        // Print the error
        const char *pStrErrorMessage = PyUnicode_AsUTF8(pvalue);
        printf("***PythonAssert*** - %s\n", pStrErrorMessage);

        // Decrement reference count to objects
        Py_DECREF(ptype);
        Py_DECREF(pvalue);
        Py_DECREF(ptraceback);
        return false;
    }
    return true;
}
#endif // TARGET_HW_PYTHON_CAPABLE