//
//  server.h
//  mini-sql
//
//  Created by Дмитрий Маслюков on 21.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef server_h
#define server_h
#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  /* Windows XP. */
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#endif

static const char * okhdr = "HTTP/1.1 200 OK\r\nContent-length:";
static const char * okbody = "\r\nContent-Type: %s\r\nConnection: close\r\n\r\n";
static const char * statdir = "/svelte-app/public";


class Server{
    int sockfd;
   public:
    class Connection {
        int clfd;
        char rdbuf [2048];
    public:
        Connection(int cl, const char * src = 0){
            clfd = cl;
            if(src){
                strcpy(rdbuf, src);
            }
        }
        const char * get_query() {
            int bread = 0;
            int read = 1;
            
            
            char encoded [2048];
            char * writer = encoded;
            for(char * rdr = rdbuf; *rdr != 0; ++rdr){
                if( *rdr == '%' ){
                    rdr += 2;
                    char numbuf [3];
                    strncpy(numbuf, rdr-1, 2);
                    numbuf[2] = 0;
                    int res = (int)strtol(numbuf, 0, 16);
                    switch (res) {
                        case 34:
                            *writer ++ = '"';
                            break;
                        case 32:
                            *writer ++ = ' ';
                            break;
                        case 40:
                            *writer ++ = '(';
                            break;
                        case 41:
                            *writer ++ = ')';
                            break;
                        case 61:
                            *writer ++ = '=';
                            break;
                        case 60:
                            *writer ++ = '<';
                            break;
                        case 62:
                            *writer ++ = '>';
                        default:
                            break;
                    }
                } else {
                    *writer ++ = *rdr;
                }
            }
            strcpy(rdbuf, encoded);
            return rdbuf;
        }
        
        void write_answer( const char * answer ){
            if(strncmp(answer, "HTTP/1.1", 8) != 0){
                write(clfd, okhdr, strlen(okhdr));
                char clen [10];
                sprintf(clen, "%zu", strlen(answer));
                char body [1000];
                sprintf(body, okbody, "text/html");
                write(clfd, clen, strlen(clen));
                write(clfd, body, strlen(body));
            }
            write(clfd, answer, strlen(answer));
            shutdown(clfd, SHUT_RDWR);
            close(clfd);
        }
        
    };
    

    Server(int portno = 27177){
#ifdef _WIN32
        WSADATA wsa_data;
        return WSAStartup(MAKEWORD(1,1), &wsa_data);
#endif
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in servaddr;
        servaddr.sin_family    = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(portno);
        int enable = 1;
        
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
        bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
        listen(sockfd, 100);
        printf("listening %d\n", portno);
    }
    
    Connection connect(){
        socklen_t l;
        struct sockaddr_in addr;
    cl_connect:;
        int clfd = accept(sockfd, (sockaddr*)&addr, &l);
        printf("client fd: %d, my fd: %d\n", clfd, sockfd);
        char rdbuf [4096];
        bzero(rdbuf, 4095);
        //firstly read the headers
        int bread = 0;
        int read = 1;
        while(read && !strstr(rdbuf, "\r\n\r\n")){
            read = (int)recv(clfd, rdbuf+bread, 1, MSG_WAITALL);
            if( read < 0 )
                return Connection(-1);
            bread += read;
            rdbuf[ bread ] = 0;
            //printf( "Got %d, %s\n", read, rdbuf );
        }
        char type[10];
        char path [2048];
        sscanf(rdbuf, "%s%s", type, path);
        char * path_wo_params = strtok(path, "?");
        if(strstr(path_wo_params, ".") != 0){
            char strbuf [2000];
            char * fname = strchr(path_wo_params, '/');
            sprintf(strbuf, "%s%s", statdir, fname);
            FILE * f = fopen(strbuf + 1, "r");
            fseek(f, 0, SEEK_END);
            long len = ftell(f);
            rewind(f);
            write(clfd, okhdr, strlen(okhdr));
            char clen [10];
            char bodybuf [100];
            char * ext = strchr(strbuf, '.') + 1;
            const char * mime = "text/html";
            if(strcmp(ext, "js")==0){
                mime = "application/javascript";
            }
            if(strcmp(ext, "css")==0){
                mime = "text/css";
            }
            if(strcmp(ext, "svg")==0){
                mime = "image/svg+xml";
            }
            sprintf(bodybuf, okbody, mime);
            sprintf(clen, "%lu", len);
            write(clfd, clen, strlen(clen));
            write(clfd, bodybuf, strlen(bodybuf));
            char *rdbuf_ = (char*)malloc(len);
            fread(rdbuf_, len, 1, f);
            write(clfd, rdbuf_, len);
            free(rdbuf_);
            fclose(f);
            close(clfd);
            goto cl_connect;
        }
        return Connection(clfd, strtok(0, "\r\n"));
    }
    
    
};


#endif /* server_h */
