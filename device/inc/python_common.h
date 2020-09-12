#ifndef _SRC_PYTHON_COMMON
#define _SRC_PYTHON_COMMON

#include "build_switches.h"

#if TARGET_HW_RASPBERRYPI
#include "python3.5/Python.h"
#endif

#if TARGET_HW_DESKTOP
#include "python3.6/Python.h"
#endif

#if TARGET_HW_PYTHON_CAPABLE
#define PROTO_PATH "proto."
#endif

#if TARGET_HW_PYTHON_CAPABLE
bool PythonAssert(PyObject* obj);
#endif // TARGET_HW_PYTHON_CAPABLE

#endif // _SRC_PYTHON_COMMON