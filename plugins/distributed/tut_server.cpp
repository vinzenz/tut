#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <tut/tut.hpp>
#include "tut_cfg.hpp"
#include "tut_server.hpp"

/**  
 * \brief tut server function
 * this function is running with tut tests and waiting for parameters from clients
 * should run register and show parameter function
 *
 * during compilation you should add -DTUT_PLUGIN_SERVER=... (this function) option e.g. -DTUT_PLUGIN_SERVER=__tut_server
 */
void __tut_server()
{
    cfg_parser cfg;
    int port = server::_DEFAULT_PORT;
    int newsockfd, pid;
    server *serv = NULL;

    try {
        cfg.load(_DEFAULT_CFG_FILE);
        port = cfg.get_cfg_int(_CFG_PORT);
    }
    catch(...){}

    while(true) {
        try {
            if( !serv )
                serv = new server(port);

            newsockfd = serv->accept();
            pid = fork();
            if( pid < 0 )
                throw std::runtime_error("ERROR on fork");

            if( pid == 0 ) {
                serv->close();
                std::cout << server::receive(newsockfd) << std::endl;

                //get parameters from xml | simple serialization
                tut::function_parameters *func_parameters = new tut::function_parameters(NULL, tut::function_parameters::_FUNCTION_ENTER, 0);

                //run register and show parameters function
                ((void (*)(tut::function_parameters *))tut::tut_functions::__register_function)(func_parameters);
                ((void (*)(tut::function_parameters *))tut::tut_functions::__show_parameters_function)(func_parameters);

                exit(0);
            }
            else
                serv->close(newsockfd);
        }
        catch(server::server_failure &ex) {
            std::cout << ex.what() << std::endl;

            if( serv ) {
                serv->close(newsockfd);
                delete serv;
                serv = NULL;
            }
        }
        catch(...){}
    }

    delete serv;
}
