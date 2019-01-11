#ifndef PROCESS_H
#define PROCESS_H

#ifdef _WIN32

#include "ipchanger/Process_Windows.h"

#else

#include "ipchanger/Process_Unix.h"

#endif // WIN32
#endif // PROCESS_H
