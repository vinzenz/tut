#ifndef __TUT_PLUGIN_HPP
#define __TUT_PLUGIN_HPP

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>

#include <tut/reflection/reflection.h>
#include <tut/tut_parameter.hpp>

/**  
 * \brief Interface to tut plugins
 * @author Krzysztof Jusiak 
 * @date 03/02/2009 
 */
class tut_plugin
{
    std::vector<std::string> servers;           ///servers names e.g. "127.0.0.1", "192.168.0.1"
    std::string name;                           ///client name to identify by server e.g. "Client-1"
    unsigned int port;                          ///port number on which stand server
    unsigned int timeout;                       ///max waiting send time [seconds]

    #define _NAME_REFLECTION                    "reflection"
    #define _NAME_FUNCTION                      "function"
    #define _NAME_FUNC_PARAMETER                "function parameters"
    #define _NAME_METHOD                        "method"
    #define _NAME_WARNING                       "tut warning"

public:

    enum {
        _DEBUG_REFLECTION           = 0x01,     ///show all reflection types
        _DEBUG_FUNCTIONS            = 0x02,     ///show pointers of all functions -> [0x80b320]
        _DEBUG_METHODS              = 0x04,     ///show register methods -> ns::function(int)
        _DEBUG_FUNC_PARAMETERS      = 0x08,     ///run default parameters function
        _DEBUG_FUNC_ORDER           = 0x10      ///functions oder in system -> 3
    };

    /**
     * keep debug modes compilation
     */
    unsigned short debug;                       ///debug mode

    enum toption {
        _OPTION_NO_SEND             = 0x01,     ///send results to server or only local
        _OPTION_DEBUG_OUT_CERR      = 0x02,     ///debug output to cerr
        _OPTION_DEBUG_OUT_COUT      = 0x04,     ///debug output to cout
        _OPTION_FUNC_MODE_ENTER     = 0x08,     ///show specifieted debug information on enter function
        _OPTION_FUNC_MODE_EXIT      = 0x10,     ///show specifieted debuginformation on exit function
        _OPTION_SHOW_NAMES          = 0x20,     ///show names with debug ouput -> [function][0x00b322]
        _OPTION_SHOW_WARNINGS       = 0x40      ///show warnings
    };

    /**
     * keep options compiliation
     */
    unsigned short options;                     ///options, specially for debug information

    enum {
        _DEFAULT_PORT               = 4444,     ///should be the big one
        _DEFAULT_TIMEOUT            = 5,        ///max time for waiting to server connection e.g. 5 seconds
        _DEFAULT_DEBUG              = 0,        ///default: don't show local debug informations
        _DEFAULT_OPTIONS            =           ///default: send info to servers and show warnings
            (_OPTION_DEBUG_OUT_CERR | _OPTION_FUNC_MODE_ENTER | _OPTION_FUNC_MODE_EXIT | _OPTION_SHOW_NAMES | _OPTION_SHOW_WARNINGS)
    };

    tut_plugin() :
        name(""), port(_DEFAULT_PORT), timeout(_DEFAULT_TIMEOUT), debug(_DEFAULT_DEBUG), options(_DEFAULT_OPTIONS) {
    }

    /**
     * to do plugin, just write
     * implementation for this functions
     */

    /** 
     * \brief init plugin
     * this function should set every parameters such as servers, name, port, timeout, ...
     */
    void init();

    /** 
     * \brief init reflection for program
     * this function should allow(deny) all defined by user classes, namespaces, etc.
     */
    void init_reflection(reflection::Reflection &);

    /** 
     * \brief send to servers
     * this function should send function parameters to all servers
     */
    void send(tut::function_parameters *);
};

#endif
