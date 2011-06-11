#ifndef __FONLINE_Q__
#define __FONLINE_Q__

// Script constants
#define SKIP_PRAGMAS
#include "_defines.fos"

// Disable macro redefinition warning
#pragma warning (push)
#pragma warning (disable : 4005)
#include "fonline.h"
#pragma warning (pop)

// AngelScript
#include "angelscript.h"
EXPORT extern asIScriptEngine* ASEngine;

#endif