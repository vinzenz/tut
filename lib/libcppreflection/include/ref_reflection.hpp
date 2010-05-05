#ifndef __REFLECTION_H
#define __REFLECTION_H

#include <iostream>
#include <vector>
#include <map>
#include <regex.h>
#include <stdarg.h>
#include <elf.h>
#include "ref_errors.hpp"
#include "ref_namespace.hpp"
#include "ref_structure.hpp"
#include "ref_member.hpp"
#include "ref_method.hpp"

#define _NO_POINTER                 0x00
#define _IS_POINTER                 0x01
#define _NO_CONST                   0x00
#define _IS_CONST                   0x01
#define _VOID_TYPE                  "void"
#define _UNKNOWN_TYPE               "<unknown>"
#define _NO_TYPE                    ""
#define _EMPTY_LINE                 ""
#define _SUB_LINE                   "  "
#define _DEFAULT_NAMESPACE          ""
#define _NO_STRUCTURE               ""
#define _SPECIAL_MANGLE             '$'
#define _SPECIAL_MANGLE_BEGIN       "$_ZN"
#define _MANGLE_CONSTRUCTOR         'C'
#define _MANGLE_DESTRUCTOR          'D'
#define _SPECIAL_DESTRUCTOR         '~'
#define _POINTER_SIZE               4


namespace reflection {

class Reflection
{
    #define _DW_TAG_NAMESPACE               "DW_TAG_namespace"
    #define _DW_TAG_STRUCTURE_TYPE          "DW_TAG_structure_type"
    #define _DW_TAG_MEMBER                  "DW_TAG_member"
    #define _DW_TAG_SUBPROGRAM              "DW_TAG_subprogram"
    #define _DW_TAG_INHERITANCE             "DW_TAG_inheritance"
    #define _DW_TAG_BASE_TYPE               "DW_TAG_base_type"
    #define _DW_TAG_POINTER_TYPE            "DW_TAG_pointer_type"
    #define _DW_TAG_TYPEDEF                 "DW_TAG_typedef"
    #define _DW_TAG_ARRAY_TYPE              "DW_TAG_array_type"
    #define _DW_TAG_REFERENCE_TYPE          "DW_TAG_reference_type"
    #define _DW_TAG_CONST_TYPE              "DW_TAG_const_type"
    #define _DW_TAG_SUBROUTINE_TYPE         "DW_TAG_subroutine_type"
    #define _DW_TAG_ENUMERATION_TYPE        "DW_TAG_enumeration_type"
    #define _DW_TAG_UNION_TYPE              "DW_TAG_union_type"
    #define _DW_TAG_SUBRANGE_TYPE           "DW_TAG_subrange_type"
    #define _DW_AT_NAME                     "DW_AT_name"
    #define _DW_AT_TYPE                     "DW_AT_type"
    #define _DW_AT_MIPS_LINKAGE_NAME        "DW_AT_MIPS_linkage_name"
    #define _DW_AT_SIBLING                  "DW_AT_sibling"
    #define _DW_AT_SPECIFICATION            "DW_AT_specification"
    #define _DW_AT_ACCESSIBILITY            "DW_AT_accessibility"
    #define _DW_AT_BYTE_SIZE                "DW_AT_byte_size"
    #define _DW_AT_DATA_MEMBER_LOCATION     "DW_AT_data_member_location"
    #define _DW_OP_PLUS_UCONST              "DW_OP_plus_uconst"

    #define _AT_NAME                        0x00
    #define _AT_POINTER_TYPE                0x00
    #define _AT_SIBLING                     0x01
    #define _AT_TYPE                        0x01
    #define _AT_BYTE_SIZE                   0x01
    #define _AT_DATA_MEMBER_LOCATION        0x01
    #define _AT_SPECIFICATION               0x02
    #define _AT_ACCESSIBILITY               0x02
    #define _AT_OP_PLUS_UCONST              0x03
    #define _AT_MIPS_LINKAGE_NAME           0x03
    #define _AT_STRUCTURE_BYTE_SIZE         0x03

    #define _STD_SS                         "std::basic_string<char,std::char_traits<char>,std::allocator<char>>"
    #define _STD_STRING_SS                  "std::string"
    #define _STD_SI                         "std::basic_istream<char,std::char_traits<char>>"
    #define _STD_STRING_SI                  "std::fstream"
    #define _STD_SO                         "std::basic_ostream<char,std::char_traits<char>>"
    #define _STD_STRING_SO                  "std::ifstream"
    #define _STD_SD                         "std::basic_iostream<char,std::char_traits<char>>"
    #define _STD_STRING_SD                  "std::ofstream"

    struct Tag
    {
        std::string name;
        std::string value;
        std::vector<std::string> tags;
    };

    struct BaseType
    {
        BaseType(const std::string &name, const std::string &value, const std::string &ns, unsigned int byteSize, unsigned short pointerType, unsigned short constType) :
            name(name), value(value), ns(ns), byteSize(byteSize), pointerType(pointerType), constType(constType)
        {}

        std::string name;
        std::string value;
        std::string ns;
        unsigned int byteSize;
        unsigned short pointerType;
        unsigned short constType;
    };

    class RegElement
    {
        struct RegValue
        {
            std::vector<std::string> allow;
            std::vector<std::string> deny;
        };

        #define _REG_BEGIN  '^'
        #define _REG_END    '$'
        #define _REG_NEG    '!'

        RegValue elements;

        bool checkReg(const std::string &reg, const std::string name)
        {
            regex_t myre;
            int err;
            char err_msg[MAX_ERR_LENGTH];
            bool result = false;

            if( (err = regcomp(&myre, reg.c_str(), REG_EXTENDED)) != 0 )
            {
                regerror(err, &myre, err_msg, MAX_ERR_LENGTH);
                throw Failure(_REG_ANALYZING_ERROR + reg + " : " +  err_msg);
            }

            if( (err = regexec(&myre, name.c_str(), 0, NULL, 0)) == 0 )
                result = true;
            else
            if( err != REG_NOMATCH )
            {
                regerror(err, &myre, err_msg, MAX_ERR_LENGTH);
                throw Failure(_REG_ANALYZING_ERROR + reg + " : " +  err_msg);
            }
            regfree(&myre);

            return result;
        }

    public:
        void allow(const std::string &name) {
            if( name.length() > 0 && name[0] == _REG_NEG )
            {
                elements.deny.push_back( _REG_BEGIN + name.substr(1) + _REG_END );
            }
            else
            {
                elements.allow.push_back( _REG_BEGIN + name + _REG_END );
            }
        }
        void deny(const std::string &name) { elements.deny.push_back( "^" + name + "$" ); }

        bool check(const std::string &name)
        {
            if( !elements.allow.size() && !elements.deny.size() )
            {
                return true;
            }

            for(std::vector<std::string>::iterator it = elements.deny.begin(); it < elements.deny.end(); it++)
            {
                if( checkReg(*it, name) )
                {
                    return false;
                }
            }

            for(std::vector<std::string>::iterator it = elements.allow.begin(); it < elements.allow.end(); it++)
            {
                if( checkReg(*it, name) )
                {
                    return true;
                }
            }

            return false;
        }

        unsigned int size() {
            return elements.allow.size() + elements.deny.size();
        }

    };

    std::string fileName;
    std::vector<Namespace *> types;
    RegElement elements;
    typedef std::vector<Namespace *>::iterator iterator;

    std::vector<Tag> *initTags();
    bool checkELF(const std::string &);
    bool setTmpValues(const std::string &, std::vector<Tag>, int, std::string *);
    bool isEmpty(const std::string &);
    bool isAll(std::vector<Tag>, int, const std::string *);
    int getLineNr(const std::string &);
    std::string getParseValue(const std::string &, const std::string &);

public:
    enum {
        _PUBLIC                         = 0x01,
        _PROTECTED                      = 0x02,
        _PRIVATE                        = 0x03
    };

    enum tshow {
        _SHOW_TEXT                      = 0x01,
        _SHOW_OFFSET                    = 0x02,
        _SHOW_POINTER                   = 0x04,
        _SHOW_PARAMETER                 = 0x04,
        _SHOW_SIZE                      = 0x08,
        _SHOW_ALL                       = (_SHOW_TEXT | _SHOW_OFFSET | _SHOW_POINTER | _SHOW_PARAMETER | _SHOW_SIZE)
    };

    enum ttype {
        _LOAD_FUNCTIONS                 = 0x01,
        _LOAD_STRUCTURES                = 0x02,
        _LOAD_ALL                       = (_LOAD_FUNCTIONS | _LOAD_STRUCTURES)
    };

    static bool shortMode;
    std::vector<BaseType *> baseTypes;
    std::vector<Method *> methodsPointers;

    Reflection();
    Reflection(bool);
    Reflection(const std::string&);
    Reflection(const std::string&, bool);
    virtual ~Reflection();

    void allow(const std::string &);
    void deny(const std::string &);
    std::vector<Namespace *> *getTypes();
    Namespace *getType(const std::string &);
    Namespace *getNamespace(const std::string &);
    Structure *getClass(const std::string &);
    Structure *isClass(const std::string &);
    Structure *getStructure(const std::string &);
    iterator begin() { return types.begin(); }
    iterator end() { return types.end(); }
    Method *getMethodByPtr(long long);
    Method *getFunctionByPtr(long long);
    std::string getFileName();
    void loadStructures(const std::string &);
    void load(ttype = _LOAD_ALL);
    void load(const std::string&, ttype = _LOAD_ALL);
    std::string toString(tshow);
    std::string toString();

    static std::vector<Method*> loadMethods(const std::string &);
};

std::string getDemangle(const std::string &);
std::string getShortType(std::string);

std::string trim(const std::string &);
std::string replaceAll(const std::string &, const std::string &, const std::string &);
std::string eraseChar(std::string txt, char c);

}

#define _READ_ELF   "readelf --debug-dump=info"
#define _NM         "nm"

template <typename _CastTo, typename _CastFrom>
_CastTo union_cast(_CastFrom _value)
{
    union Union{ _CastFrom _from; _CastTo _to; } unionCast;
    unionCast._from = _value;
    return unionCast._to;
}

#endif
