#ifndef __CFG_HPP
#define __CFG_HPP

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>

/**  
 * \brief Config file parser
 * @author Krzysztof Jusiak 
 * @date 03/02/2009 
 */
class cfg_parser
{
    struct cfg_element
    {
        std::string name;        
        std::vector<std::string> values;

        cfg_element(const std::string &name, const std::string &value) : name(name) {
            this->values.push_back(value);
        }

        cfg_element() : name("") {}
    };

    #define _COMMENT        '#'
    #define _SEPARATOR      ','
    #define _END_NAME       ':'

    std::string fileName;
    std::vector<cfg_element *> elements;

    void add(const std::string &name, const std::string &value) {
        if( !name.length() || !value.length() )
             return;

        for(std::vector<cfg_element *>::iterator it = elements.begin(); it < elements.end(); it++) {
            if( (*it)->name == name ) {
                for(std::vector<std::string>::iterator iv = (*it)->values.begin(); iv < (*it)->values.end(); iv++)
                    if( *iv == value )
                        return;
    
                (*it)->values.push_back(value);
                return;
            }
        }
        elements.push_back( new cfg_element(name, value) );
    }

    void toLower(std::string &str) {
        std::transform(str.begin(), str.end(), str.begin(), (int(*)(int)) std::tolower);
    }
   

public:
    class cfg_failure : public std::exception
    {
        std::string err_msg;

    public:
        cfg_failure(const std::string &msg) {}
        ~cfg_failure() throw() {}

        const char* message() const throw() {
            return err_msg.c_str();
        }
    };

    cfg_parser(const std::string &fileName) : fileName(fileName) {
        load(fileName);
    }

        cfg_parser() : fileName("") {
    }

    ~cfg_parser() {
        elements.clear();
    }

    void load(const std::string &fileName = "") {
        std::string line;
        std::ifstream file(fileName.c_str());

        if( !file )
            throw cfg_failure("i/o error: '" + fileName + "'");

        while( getline(file, line) ) {
            std::string name = "", value = ""; bool isName = false; toLower(line);

            for(std::string::iterator it = line.begin(); it < line.end(); it++) {
                if( *it == _COMMENT )
                    break;
                else
                if( isName == false && *it == _END_NAME )
                    isName = true;
                else
                if( isName == false )
                    name += *it;
                else
                if( *it != ' ' && *it != '\t' && *it != '\r' && *it != '\n' )
                    if( *it == _SEPARATOR ) {
                        add(name, value);
                        value = "";
                    }
                    else
                        value += *it;
            }

            if( value != "" )
                add(name, value);
        }

        file.close();
    }
    
    std::vector<std::string> get_cfg_all(const std::string &name) {
        for(std::vector<cfg_element *>::iterator it = elements.begin(); it < elements.end(); it++)
            if( (*it)->name == name )
                return (*it)->values;

        throw cfg_failure("not found element error: '" + name + "'");
    }

    std::string get_cfg(const std::string &name) {
        return get_cfg_all(name)[0];
    }

    int get_cfg_int(const std::string &name) {
        return atoi( get_cfg(name).c_str() );
    }
};

#endif
