#ifndef DEBUGFLAGS_H
#define DEBUGFLAGS_H

#include <cassert>

#define DEBUG true

#if DEBUG
    //A debug flag for each individual unit test

    #define MEMEBER_PURCHASE_AMOUT_TEST true
    #define MEMBER_PURCHASE_SEARCH_TEST true

#else
    //If DEBUG is false all unit test flags are also false
    #define MEMEBER_PURCHASE_AMOUT_TEST false
    #define MEMBER_PURCHASE_SEARCH_TEST false
#endif

#endif // DEBUGFLAGS_H
