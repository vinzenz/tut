#include <iostream>
#include <stdexcept>
#include <vector>
#include <tut/reflection/reflection.h>
#include <tut/tut_function.hpp>
#include <tut/tut_plugin.hpp>
#include "tut_cfg.hpp"
#include "tut_client.hpp"

/**
 * here is an example of tut plugin client
 * (implementation for tut_plugin.hpp)
 */

/**  
 * \brief init plugin
 * this function is call whe start first function in program and should
 * init all tut_plugin class values
 */
void tut_plugin::init() {
    cfg_parser cfg(_DEFAULT_CFG_FILE);
            
    this->port = cfg.get_cfg_int(_CFG_PORT);
    this->name = cfg.get_cfg(_CFG_NAME);
    this->servers = cfg.get_cfg_all(_CFG_SERVER);
    try {
        this->timeout = cfg.get_cfg_int(_CFG_TIMEOUT);
    }
    catch(...){
    }
    try {
        std::string conf = cfg.get_cfg(_CFG_DEBUG);

        this->debug = _DEBUG_REFLECTION * (conf.find(_CFG_DEBUG_REFLECTION) != std::string::npos) +
                      _DEBUG_FUNCTIONS * (conf.find(_CFG_DEBUG_FUNCTIONS) != std::string::npos) +
                      _DEBUG_METHODS * (conf.find(_CFG_DEBUG_METHODS) != std::string::npos) +
                      _DEBUG_FUNC_PARAMETERS * (conf.find(_CFG_DEBUG_PARAMETERS) != std::string::npos) +
                      _DEBUG_FUNC_ORDER * (conf.find(_CFG_DEBUG_ORDER) != std::string::npos);
    }
    catch(...){
    }
    try {
        std::string conf = cfg.get_cfg(_CFG_OPTIONS);

        this->options = _OPTION_NO_SEND * (conf.find(_CFG_OPTION_LOCAL) != std::string::npos) +
                        _OPTION_DEBUG_OUT_CERR * (conf.find(_CFG_OPTION_CERR) != std::string::npos) +
                        _OPTION_DEBUG_OUT_COUT * (conf.find(_CFG_OPTION_COUT) != std::string::npos) +
                        _OPTION_FUNC_MODE_ENTER * (conf.find(_CFG_OPTION_FENTER) != std::string::npos) +
                        _OPTION_FUNC_MODE_EXIT * (conf.find(_CFG_OPTION_FEXIT) != std::string::npos) +
                        _OPTION_SHOW_NAMES * (conf.find(_CFG_OPTION_NAMES) != std::string::npos) +
                        _OPTION_SHOW_WARNINGS * (conf.find(_CFG_OPTION_WARNINGS) != std::string::npos);
    }
    catch(...){
    }

    //access functions to use with client
    //add here all functions which user can definied in tut_client.cfg (parameters)
    #define access_for_function(function) get_pointer(&tut::tut_functions::tut_user_functions::function)
    access_for_function(empty);
    access_for_function(simple);
    access_for_function(tree);

    reflection::Reflection ref;
    ref.load(reflection::Reflection::_LOAD_FUNCTIONS);
    //set function which define how show function parameters | don't use show_parameters... | function should return std::string
    try {
        std::string conf = cfg.get_cfg(_CFG_PARAMETERS);
        tut::tut_functions::set_show_parameters_function((void *)tut::tut_reflection::get_function_ptr("tut::tut_functions::tut_user_functions::" + conf, ref));
    }
    catch(std::exception &e){
        tut::tut_functions::set_show_parameters_function( get_pointer( &get_user_functions(simple) ) );
    }
}

/**  
 * \brief init reflection
 * this function should add all types which should be registered
 * @param ref reference to reflection
 */
void tut_plugin::init_reflection(reflection::Reflection &ref) {
    cfg_parser cfg(_DEFAULT_CFG_FILE);
    std::vector<std::string> cfg_ref = cfg.get_cfg_all(_CFG_REFLECTION);
        for(std::vector<std::string>::iterator it = cfg_ref.begin(); it < cfg_ref.end(); it++)
            ref.allow(*it);
}

/**  
 * \brief send
 * this function is call when some function is executed and should send parameters to all servers
 * @param func_parameters function parameters
 */
void tut_plugin::send(tut::function_parameters *func_parameters) {
    for(std::vector<std::string>::iterator it = servers.begin(); it < servers.end(); it++) {
        client *cl = new client(*it, this->port);
        cl->connect();

        std::string xml_param = "<tut_client><name>" + this->name + "</name>" + func_parameters->to_xml() + "</tut_client>";
        if( write(cl->get_socket(), xml_param.c_str(), xml_param.length()) < 0 )
            throw std::runtime_error("ERROR writing socket");

        delete cl;
    }
}
