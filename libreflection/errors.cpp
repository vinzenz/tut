#include <reflection/errors.h>

namespace reflection {

Failure::Failure(const std::string &msg)
    : err_msg(msg)
{}

Failure::~Failure() throw()
{}

const char *Failure::message() const throw()
{
    return (_ERROR + err_msg + "\n").c_str();
}

}
