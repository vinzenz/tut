#ifndef TUT_H_PARAMETERS
#define TUT_H_PARAMETERS

#include <iostream>
#include <sstream>
#include <vector>
#include <tut/reflection/reflection.h>
#include <tut/tut_function.hpp>
#include <tut/tut.hpp>

namespace tut {

/**  
 * \brief Keep function parameters
 * @author Krzysztof Jusiak
 * @date 03/02/2009
 */
class function_parameters
{
    #define get_int(value)              (int)(value)
    #define get_char(value)             *((char *)(value))
    #define get_char_pointer(value)     (char *)(value)
    #define get_double(value)           *((double *)value)
    #define get_float(value)            *((double *)value)

    #define _TYPE_VOID                  "void"
    #define _TYPE_CHAR                  "char"
    #define _TYPE_CHAR_POINTER          "char*"
    #define _TYPE_INT                   "int"
    #define _TYPE_DOUBLE                "double"
    #define _TYPE_FLOAT                 "float"

    #define _UNKNOWN                    "<unknown>"
    #define _UNKNOWN_VALUE              (void *)-1
    #define _UNKNOWN_SIZE               -1
    #define _NULL                       "NULL"

    #define _DEFAULT_PRECISION          5
    #define _DEFAULT_CLIENT             "tut_localhost"

public:
    enum tfmode {
        _FUNCTION_ENTER                 = 0x01,
        _FUNCTION_EXIT                  = 0x02,
        _FUNCTION_START_REGISTERING     = 0x04,
        _FUNCTION_END_REGISTERING       = 0x08
    };

private:

    /**  
     * \brief Keep single parameter
     * @author Krzysztof Jusiak
     * @date 03/02/2009
     */
    class parameter
    {
        std::string name;                                       ///parameter name
        std::string type;                                       ///parameter type
        std::vector<void *> value;                              ///list of values parameters
        std::string parent;                                     ///structure parent of parameter
        std::vector<parameter *> parameters;                    ///more parameters
        bool isClass;                                           ///if parameter is class
        typedef std::vector<parameter *>::iterator iterator;

    public:
        parameter(const std::string &name, const std::string &type, const std::string &parent) : 
            name(name), type(type), parent(parent), isClass(false) 
        {}

        parameter(const std::string &type, void *value) : 
            name(""), type(type), parent(""), isClass(false)
        {
            this->value.push_back(value);
        }

        parameter(const std::string &type, bool isClass) : 
            name(""), type(type), parent(""), isClass(isClass)
        {}

        parameter() : 
            name(""), type(""), parent(""), isClass(false)
        {}

        const std::string &get_name() { return name; }
        const std::string &get_type() { return type; }
        void add(parameter *p) { parameters.push_back(p); }
        void add_value(void *v) { value.push_back(v); }
        unsigned int get_size() { return parameters.size(); }    
        parameter *get_parameter(unsigned int pos) { return parameters[pos]; }
        void *get_value() { return value.size() > 0 ? value[value.size()-1] : NULL; }
        void *get_value(unsigned int i) { return value[i]; }
        bool is_class() { return isClass; }
        bool is_table(){ return value.size() > 1; }
        unsigned int get_table_size(){ return value.size(); }
        const std::string &get_parent() { return parent; }
        iterator begin() { return parameters.begin(); }
        iterator end() { return parameters.end(); }
    };

    /** 
     * \brief add new parameter
     * @param type type parameter e.g. "double"
     * @param value pointer to parameter value
     */
    void new_parameter(const std::string &type, void *value)
    {
        if( active == NULL )
            parameters.push_back( new parameter(type, value) );
        else
            active->get_parameter(active->get_size()-1)->add_value(value);
    }

    /** 
     * \brief add structure parameters
     * @param structure structure class parameter
     * @param ptr pointer where structure is in memory
     */
    void add_structure_parameters(reflection::Structure *structure, int *ptr)
    {
        int pop = 0;
        for(std::vector<reflection::Member *>::iterator member = structure->getMembers()->begin(); member < structure->getMembers()->end(); member++)
        {
            active->add( 
                new parameter((*member)->getName(), (*member)->getType() + ((*member)->getPointerType() == _IS_POINTER ? "*" : ""), (*member)->getStructure()->getName()) );

            *(ptr += ((*member)->getOffset() - pop) / sizeof(int *) );
    
            add_parameter( (*member)->getType() + ((*member)->getPointerType() == _IS_POINTER ? "*" : ""), ptr, 
                           (member == structure->getMembers()->end()-1 ? (*member)->getStructure()->getByteSize() : (*(member+1))->getOffset()) - (*member)->getOffset()
                         );

            pop = (*member)->getOffset();
        }

        active = NULL;
    }

public:
    reflection::Method *method;                             ///method which have this parameters

private:
    std::vector<parameter *> parameters;                    ///parameters of method
    tfmode functionMode;                                    ///where this parameters was throwed (begin, end of function)
    parameter *active;                                      ///temporary parameter to more compileted types
    unsigned long functionOrder;                            ///number of order function
    std::string client;                                     ///client name -> default localhost
    static unsigned int maxTableLength;                     ///max table length, unsafe (for classes you know size of static declared tables, but not for parameters)
    typedef std::vector<parameter *>::iterator iterator;

public:
    function_parameters(reflection::Method *method = NULL, tfmode functionMode = _FUNCTION_ENTER, unsigned long functionOrder = 0x00, const std::string &client = _DEFAULT_CLIENT) :
        method(method), functionMode(functionMode), active(NULL), functionOrder(functionOrder), client(client) {}

    ~function_parameters() {
        parameters.clear();
    }

    std::string get_name() { return method->getDemangle(); }
    tfmode get_mode() { return functionMode; }
    unsigned long get_order() { return functionOrder; }

    unsigned int get_size() { return parameters.size(); }
    iterator begin() { return parameters.begin(); }
    iterator end() { return parameters.end(); }

    /**
     * \brief add parameter
     * @param type type parameter e.g. "double"
     * @param value pointer to parameter
     */
    void add_parameter(const std::string &type, void *value) {
        new_parameter(type, value);
    }

    /**
     * \brief add parameter
     * @param type type parameter e.g. "double"
     * @param ptrValue pointer to parameter
     * @param length parameter size, usefull to get tables, but only in classes, in arguments use maxTableLength
     */
    void add_parameter(const std::string &type, int *ptrValue, unsigned int length)
    {
        reflection::Structure *structure;

        if( type == _TYPE_INT )
            new_parameter(type, (void *)(*(int *)ptrValue));
        else
        if( type == _TYPE_CHAR )
            new_parameter(type, (void *)((char *)ptrValue));
        else
        if( type == _TYPE_CHAR_POINTER )
            new_parameter(type, (void *)(*(char **)ptrValue));
        else
        if( type == _TYPE_DOUBLE )
            new_parameter(type, (void *)((double *)ptrValue));
        else
        if( type == _TYPE_FLOAT )
            new_parameter(type, (void *)((float *)ptrValue));
        else
        if( (structure = tut_reflection::reflect.isClass(reflection::eraseChar(type, '*'))) != NULL )
        {
            if( active == NULL )
            {
                parameters.push_back( new parameter(type, true) );
                active = parameters[get_size()-1];
            }
            else
            {
                parameters.push_back( new parameter(type, true) );
                active = active->get_parameter(active->get_size()-1);
            }

            add_structure_parameters(structure, ptrValue);
        }
        else //unknown
            new_parameter(type, _UNKNOWN_VALUE);
    }

    /**
     * \brief convect parameters to xml
     * @return xml parameters string
     */
    std::string to_xml()
    {
        std::stringstream ss;

        ss << "<function_parameters>";
        ss << "<name>" << get_name() << "</name>";
        ss << "<mode>" << get_mode() << "</mode>";
        ss << "<parameters>";

        for(std::vector<function_parameters::parameter *>::iterator it = begin(); it < end(); it++)
        {
            ss << "<type>" << (*it)->get_type() << "</type>";
            ss << "<value>";

            if( (*it)->is_class() == false )
                ss << function_parameters::get_value((*it)->get_type(), (*it)->get_value());

            ss << "</value>";
        }
        ss << "</parameters>";
        ss << "</function_parameters>";

        return ss.str();
    }

    /**
     * \brief get value from void *
     * @param type type parameter e.g. "double"
     * @param value pointer to parameter
     * @param precision float precision
     * @return parameter value string
     */
    static std::string get_value(const std::string &type, void *value, unsigned short precision = _DEFAULT_PRECISION)
    {
        std::stringstream ss;

        if( value == NULL )
            ss << _NULL;
        else
        if( type == _TYPE_INT )
            ss << get_int(value);
        else
        if( type == _TYPE_CHAR )
            ss << get_char(value);
        else
        if( type == _TYPE_CHAR_POINTER )
            ss << get_char_pointer(value);
        else
        if( type == _TYPE_DOUBLE )
        {
            ss.precision(precision);
            ss.setf(std::ios::fixed,std::ios::floatfield);
            ss << get_double(value);
        }
        else
        if( type == _TYPE_FLOAT )
        {
            ss.precision(precision);
            ss.setf(std::ios::fixed,std::ios::floatfield);
            ss << get_float(value);
        }
        else
            ss << _UNKNOWN;

        return ss.str();
    }

    /**
     * \brief get max table length
     * @param maxTableLength max table length
     */
    static void set_max_table_length(unsigned int maxTableLength)
    {
        function_parameters::maxTableLength = maxTableLength;
    }

    /**
     * \brief get max table length
     * @return max table length
     */
    static unsigned int get_max_table_length()
    {
        return maxTableLength;
    }
};

}

#endif
