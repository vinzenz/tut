#ifndef __X86_H
#define __X86_H

#define __x86_eax__ "=a"    ///eax
#define __x86_ebx__ "=b"    ///ebx

/**  
 * \brief get value from register
 * @param reg register -> __x86_eax__, __x86_ebx__
 * @param result here will be result
 */
#define __x86_get_register_value__(reg, result)\
__asm__ __volatile__\
(\
    ""\
    : reg (result)\
)

/**  
 * \brief get float value, use static __double value
 */
#define __x86_get_floating_value__()\
__asm__ __volatile__\
(\
    "fstpl __double \n"\
    "fldl __double \n"\
)

/**  
 * \brief run function from pointer
 * @param ptr pointer to function
 */
#define __x86_run_function__(ptr)\
__asm __volatile__\
(\
    "call *%%eax \n"\
    :\
    : "a" (ptr)\
)

/**  
 * \brief end function 
 * this function is make the most of new body
 */
#define __x86_end_function__()\
__asm__ __volatile__\
(\
    "movl %ebp, %esp \n"\
    "popl %ebp \n"\
)

/**
 * \brief get function parameters
 * @param result here will be pointer to first parameter
 */
#define __x86_get_parameters__(result)\
    __asm__ __volatile__\
    (\
        "addl (%%ebp), %%eax \n"\
        : "=a" (result)\
        : "a" (8)\
    );\

/**
 * \brief get parameter from memory
 * this function is use to get values from pointers types such as classes etc.
 * @param offset parameter offset on the stack
 * @param result pointer to memory where are real values
 */
#define __x86_get_parameter_from_memory__(offset, result)\
__asm__ __volatile__\
(\
    "addl (%%ebp), %%eax \n"\
    "movl (%%eax), %%ebx \n"\
    : "=b" (result)\
    : "a" (offset)\
)

/**
 * \brief push parameters on the stack
 * this function push parametes on the stack and then you can execute function by run_function macro
 * @param parameters vector with parameters types
 * @param paramters_ptr pointer to parameters begin on the stack
 */
#define __x86_push_function_parameters__(parameters, parameters_ptr)\
{\
    unsigned int size = parameters->size();\
    unsigned int i=0;\
    for(std::vector<std::string>::iterator ip = parameters->begin(); ip < parameters->end(); ip++, i++)\
        if( (*ip) == _TYPE_DOUBLE ) size++;\
    unsigned int _ptr = 0;\
    for(unsigned int i=0; i < size; i++, parameters_ptr++, _ptr += sizeof(int *))\
    {\
        __asm__ __volatile__\
        (\
            "addl %%esp, %%ebx \n"\
            "movl %%eax, (%%ebx) \n"\
            :\
            : "a" (parameters_ptr), "b" (_ptr)\
        );\
    }\
}

#endif
