#ifndef DEBUGFLAGS_H
#define DEBUGFLAGS_H

#include <cassert>

#define DEBUG true

#if DEBUG
    //A debug flag for each individual unit test
    #define MEMEBER_PURCHASE_AMOUT_TEST false
#else
    //If DEBUG is false all unit test flags are also false
    #define MEMEBER_PURCHASE_AMOUT_TEST false
#endif

#endif // DEBUGFLAGS_H
