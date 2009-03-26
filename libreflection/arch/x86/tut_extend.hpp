#ifndef __TUT_EXTENDS_H
#define __TUT_EXTENDS_H

#include <iostream>
#include <vector>

/** 
 * \brief backtrace
 * @return vector of string with functions which are on the stack
 */
std::vector<std::string> __x86_backtrace__()
{
    std::vector<std::string> *result = new std::vector<std::string>();
    register void *_ebp __asm__ ("ebp");
    register void *_esp __asm__ ("esp");
    unsigned int *rfp;

    for(rfp = *(unsigned int **)_ebp; rfp; rfp = *(unsigned int **)rfp)
    {
        int diff = *rfp - (unsigned int)rfp;

        if( (void *)rfp < _esp || diff > 4 * 1024 || diff < 0 )
            break;

        result->push_back(tut::tut_reflection::get_function_name( rfp[1]-4 ));
    }

    return *result;
}

#endif
