#ifndef __ERRORS_H
#define __ERRORS_H

#include <iostream>

#define _ERROR                      "error: "
#define _IO_ERROR                   "i/o error in file: "
#define _ELF_ERROR                  "ELF error in file: "
#define _PARSE_ERROR                "Parse error in file: "
#define _NAMESPACE_ERROR            "namespace not found: " 
#define _STRUCTURE_ERROR            "structure not found: " 
#define _METHOD_ERROR               "method not found: " 
#define _MEMBER_ERROR               "member not found: " 
#define  MAX_ERR_LENGTH              0x50 
#define _REG_ANALYZING_ERROR        "error analyzing regular expression: "

namespace reflection {

/**  
 * \brief Failure exception
 * @author Krzysztof Jusiak 
 * @date 03/02/2009 
 */
class Failure : public std::exception
{
    std::string err_msg;
    
public:
    Failure(const std::string &msg);
    ~Failure() throw();

    const char* message() const throw();
};

}

#endif
