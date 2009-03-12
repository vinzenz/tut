#ifndef __SERVER_HPP
#define __SERVER_HPP

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

#define _CFG_PORT               "port"             
#define _CFG_TIMEOUT            "timeout" 

#define _DEFAULT_TIMEOUT        5
#define _DEFAULT_CFG_FILE       "tut_server.cfg"

/**  
 * \brief Simple socket server
 * @author Krzysztof Jusiak 
 * @date 03/02/2009 
 */
class server
{
    int sockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;

public:
    enum {
        _DEFAULT_PORT   = 4444,
        _BACK_LOG       = 5
    };

    class server_failure : public std::exception        
    {
        std::string err_msg;
                        
    public:
        server_failure(const std::string &msg) {}
        ~server_failure() throw() {}
               
        const char* message() const throw() {
            return err_msg.c_str();
        }
    };
    
    server(int portno = _DEFAULT_PORT) : portno(portno) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if( sockfd < 0 )
            throw server_failure("ERROR opening socket");

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
        clilen = sizeof(cli_addr);

        if( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) 
            throw server_failure("ERROR on binding");

        listen(sockfd, _BACK_LOG);
    }

    int accept() {
        int newsockfd = ::accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);

        if( newsockfd < 0 )
            throw server_failure("ERROR on accept");

        return newsockfd;
    }

    void close(int sockfd) {
        ::close(sockfd);
    }

    void close() {
        ::close(sockfd);
    }

    static std::string receive(int sock)
    {
        try
        {
            std::string result = ""; 
            char buffer[256], *ptr; bzero(buffer,256);

            if( read(sock, buffer, 255) > 0 )
            {
                ptr = &buffer[0];

                while(*ptr)
                    result += *ptr++;
            }

            return result;
        }
        catch(...) {}

        return "";
    } 

    int get_socket() { return sockfd; }
};

#endif
