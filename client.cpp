#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

#define BUFFER 1024

int main(int argc, char *argv[])
{
    struct sockaddr_in addr;
    int sockfd;
    fd_set rset;
    int maxfd;
    char recvBuf[BUFFER], sendBuf[BUFFER];
    int bytes;

    if(argc != 2)
    {
        std::cout << "Ip Address Required" << std::endl;
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        std::cerr << "Client Error (Socket): " << strerror(errno) << std::endl;
        exit(1);
    }

    if((connect(sockfd, (struct sockaddr *)&addr, sizeof(addr))) == -1)
    {
        std::cerr << "Client Error (Connect): " << strerror(errno) << std::endl;
        exit(1);
    }

    
    FD_ZERO(&rset);

    while(true)
    {
        FD_SET(fileno(stdin), &rset);
        FD_SET(sockfd, &rset);

        maxfd = std::max(sockfd, fileno(stdin)) + 1;

        int ready = select(maxfd, &rset, NULL, NULL, NULL);
        if(ready == -1)
        {
            std::cerr << "Client Error (Select): " << strerror(errno) << std::endl;
            break;
        }

        if(FD_ISSET(sockfd, &rset))
        {
            if((bytes = read(sockfd, recvBuf, BUFFER)) <= 0)
            {
                if(bytes == 0)
                {
                    std::cout << "Server Terminated" << std::endl;
                    break;
                }
                else
                {
                    std::cout << "Client Error (Read): " << strerror(errno) << std::endl;
                    break;
                }
            }

            recvBuf[bytes] = '\0';

            std::cout << recvBuf << std::endl;
            std::cout.flush();
        }

        if(FD_ISSET(fileno(stdin), &rset))
        {
            std::cin.getline(sendBuf, BUFFER);

            if((write(sockfd, sendBuf, strlen(sendBuf))) == -1)
            {
                std::cerr << "Client Error (Write): " << strerror(errno) << std::endl;
                break;
            }
        }
    }

    close(sockfd);

    return 0;
}