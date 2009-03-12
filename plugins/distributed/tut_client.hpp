#ifndef __CLIENT_HPP
#define __CLIENT_HPP

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

#define _CFG_NAME                   "name"
#define _CFG_PORT                   "port"
#define _CFG_SERVER                 "server"
#define _CFG_REFLECTION             "reflection"
#define _CFG_TIMEOUT                "timeout"
#define _CFG_DEBUG                  "debug"
#define _CFG_OPTIONS                "options"
#define _CFG_PARAMETERS             "parameters"

#define _CFG_SEPARATOR              '|'
#define _CFG_DEBUG_REFLECTION       "reflection"
#define _CFG_DEBUG_FUNCTIONS        "functions"
#define _CFG_DEBUG_METHODS          "methods"
#define _CFG_DEBUG_PARAMETERS       "parameters"
#define _CFG_DEBUG_ORDER            "order"

#define _CFG_OPTION_LOCAL           "local"
#define _CFG_OPTION_CERR            "cerr"
#define _CFG_OPTION_COUT            "cout"
#define _CFG_OPTION_FENTER          "fenter"
#define _CFG_OPTION_FEXIT           "fexit"
#define _CFG_OPTION_NAMES           "names"
#define _CFG_OPTION_WARNINGS        "warnings"

#define _DEFAULT_TIMEOUT            5
#define _DEFAULT_CFG_FILE           "tut_client.cfg"

/**  
 * \brief Simple socket client
 * @author Krzysztof Jusiak 
 * @date 03/02/2009 
 */
class client
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

public:
    enum {
        _DEFAULT_PORT   = 4444,
    };

    class client_failure : public std::exception
    {
        std::string err_msg;

    public:
        client_failure(const std::string &msg) {}
        ~client_failure() throw() {}

        const char* message() const throw() {
            return err_msg.c_str();
        }
    };

    client(std::string hostName, int portno = _DEFAULT_PORT) : portno(portno) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if( sockfd < 0 )
            throw client_failure("ERROR opening socket");

        server = gethostbyname(hostName.c_str());
        if( server == NULL )
            throw client_failure("ERROR, no such host: " + hostName);

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);
    }

    void connect() {
        if( ::connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 )
            throw client_failure("ERROR connecting");
    }

    int get_socket() { return sockfd; }
};

#endif
