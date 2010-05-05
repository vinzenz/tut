#include "ref_type.hpp"

namespace reflection
{

std::vector<Method*> getMethodPtr(const std::string& fileName, const std::string& regex)
{
    std::string tmpName = regex;
    std::vector<Method*> l_list;

    if( tmpName[0] == '&' )
    {
        tmpName = tmpName.substr(1, tmpName.length() - 1);
    }
    tmpName = reflection::eraseChar(tmpName, ' ');

    std::vector<reflection::Method*> l_methods = Reflection::loadMethods(fileName);

    for(std::vector<reflection::Method*>::iterator im = l_methods.begin(); im < l_methods.end(); im++)
    {
        if( (*im)->getPtr() > 0 )
        {
            std::string tmp = reflection::eraseChar(reflection::Method::getDemangle((*im)->getMangle()), ' ');

            regex_t myre;
            int err;

            if( (err = regcomp(&myre, regex.c_str(), REG_EXTENDED)) == 0 )
            {
                if( (err = regexec(&myre, tmp.c_str(), 0, NULL, 0)) == 0 )
                {
                    l_list.push_back(*im);
                }
            }
            regfree(&myre);
        }
    }

    return l_list;
}

} // namespace reflection

