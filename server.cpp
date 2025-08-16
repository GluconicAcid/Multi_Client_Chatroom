#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

#define BACKLOG 100
#define BUFFER 1024

int main()
{
    struct sockaddr_in servaddr, cliaddr;
    int listenfd, connfd, sockfd;
    int i, maxi, maxfd;
    fd_set readset, allset;
    int ready, client[FD_SETSIZE];
    socklen_t clilen;
    int yes = 1;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd == -1)
    {
        std::cerr << "Server Error (Socket): " << strerror(errno) << std::endl;
        exit(1);
    }

    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cerr << "Server Error (Setsockopt): " << strerror(errno) << std::endl;
        exit(1);
    }

    if((bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) == -1)
    {
        std::cerr << "Server Error (Bind): " << strerror(errno) << std::endl;
        exit(1);
    }

    if((listen(listenfd, BACKLOG)) == -1)
    {
        std::cerr << "Server Error (Listen): " << strerror(errno) << std::endl;
        exit(1);
    }
    
    for(i = 0; i < FD_SETSIZE; i++)
    client[i] = -1;
    
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    maxfd = listenfd;
    maxi = i;


    while(true)
    {
        timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        readset = allset;
        ready = select(maxfd + 1, &readset, NULL, NULL, NULL);
        if(ready == -1)
        {
            std::cerr << "Server Error (Select): " << strerror(errno) << std::endl;
            break;
        }

        if(FD_ISSET(listenfd, &readset))
        {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

            for(i = 0; i < FD_SETSIZE; i++)
            {
                if(client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            }

            if(i == FD_SETSIZE)
            {
                std::cout << "Too many clients" << std::endl;
                continue;
            }

            FD_SET(connfd, &allset);

            maxi = std::max(maxi, i);
            maxfd = std::max(maxfd, connfd);

            if(--ready <= 0)
                continue;
        }

        for(i = 0; i < FD_SETSIZE; i++)
        {
            if((sockfd = client[i]) < 0)
                continue;

            if(FD_ISSET(sockfd, &readset))
            {
                char buf[BUFFER];
                int bytes;
                if((bytes = read(sockfd, buf, BUFFER)) <= 0)
                {
                    if(bytes == -1)
                    {
                        std::cerr << "Server Error (Read): " << strerror(errno) << std::endl;
                        break;
                    }
                    else
                    {
                        close(sockfd);
                        FD_CLR(sockfd, &allset);
                        client[i] = -1;
                        std::cout << "Client Terminated" << std::endl;
                        continue;
                    }
                }

                for(int j = 0; j < FD_SETSIZE; j++)
                {
                    if(client[j] == sockfd || client[j] < 0)
                        continue;
                    
                    if((write(client[j], buf, bytes)) == -1)
                    {
                        std::cerr << "Server Error (Write): " << strerror(errno) << std::endl;
                        break;
                    }
                }
                if(--ready <= 0)
                    break;
            }
        }
    }
    exit(0);
}