#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>

#ifndef __socklen_t_defined
#define __socklen_t_defined
typedef int socklen_t;
#endif

#define UNIX_PATH_MAX   108
#define BUFSIZE         4096

int main(int argc, char *argv[])
{
   int protocol, sock, descIn, descOut;
   struct sockaddr_in inetAddr;
   struct sockaddr_un unixAddr;
   struct sockaddr *addr;
   socklen_t addrLen;

   if (argc!=3)
   {
      fprintf(stderr, "Usage: marcc <protocol> <server_address>\n");
      return 1;
   }
   
   if (!strcmp(argv[1], "tcp"))
      protocol = PF_INET;
   else if (!strcmp(argv[1], "unix"))
      protocol = PF_UNIX;
   else
   {
      fprintf(stderr, "marcc: protocol must be tcp or unix\n");
      return 1;
   }
   
   if (protocol==PF_INET)
   {
      char *name, *port;
      struct hostent *he;
      
      name = malloc(strlen(argv[2])+1);
      if (name==NULL)
      {
         fprintf(stderr, "marcc: out of memory\n");
         return 2;
      }
      strcpy(name, argv[2]);
      
      port = strchr(name, ':');
      if (port==NULL)
      {
         fprintf(stderr, "marcc: invalid tcp/ip address (expected <hostname>:<port>)\n");
         return 1;
      }
      
      *port = 0; port++;
      
      inetAddr.sin_family = AF_INET;
      
      he = gethostbyname(name);
      if (he==NULL)
      {
         fprintf(stderr,"marcc: invalid tcp/ip address\n");
         return 1;
      }
      memcpy(&inetAddr.sin_addr, he->h_addr_list[0], he->h_length);
      
      inetAddr.sin_port = htons(atoi(port));
      if (inetAddr.sin_port==0)
      {
         fprintf(stderr, "marcc: invalid tcp port number (must be 1-65535)\n");
         return 1;
      }

      addr = (struct sockaddr *)&inetAddr;
      addrLen = sizeof(inetAddr);
      
      free(name);
   }
   else
   {
      if (strlen(argv[2])>=UNIX_PATH_MAX)
      {
         fprintf(stderr, "marcc: unix address too long\n");
         return 1;
      }
      unixAddr.sun_family = AF_UNIX;
      strcpy(unixAddr.sun_path, argv[2]);

      addr = (struct sockaddr *)&unixAddr;
      addrLen = sizeof(unixAddr);
   }
   
   sock = socket(protocol, SOCK_STREAM, 0);
   if (sock==-1)
   {
      perror("marcc: can't create socket");
      return 2;
   }
   
   if (connect(sock, addr, addrLen)==-1)
   {
      perror("marcc: can't connect to server");
      return 2;
   }
   
   descIn = fileno(stdin);
   descOut = fileno(stdout);
   
   for (;;)
   {
      fd_set fds;
      int retVal, retVal2;
      char buffer[BUFSIZE];
      
      FD_ZERO(&fds);
      FD_SET(descIn, &fds);
      FD_SET(sock, &fds);
      
      retVal = select(sock+1, &fds, NULL, NULL, NULL);
      if (retVal==-1)
      {
         if (errno==EINTR) continue;
         perror("marcc: error while waiting for data");
         return 2;
      }
      
      if (FD_ISSET(descIn, &fds))
      {
         do
         {
            retVal = read(descIn, buffer, BUFSIZE);
         } while (retVal==-1 && errno==EINTR);
         
         if (retVal==-1)
         {
            perror("marcc: error while reading data from standard input");
            return 2;
         }
         if (retVal==0)
         {
            shutdown(sock, SHUT_WR);
            for (;;)
            {
               retVal = read(sock, buffer, BUFSIZE);
               if (retVal==-1)
               {
                  perror("marcc: error while receiving data from server");
                  return 2;
               }
               if (retVal==0)
               {
                  shutdown(sock, SHUT_RD);
                  close(sock);
                  return 0;
               }
               do
               {
                  retVal2 = write(descOut, buffer, retVal);
               } while (retVal2==-1 && errno==EINTR);
               if (retVal2<retVal)
               {
                  fprintf(stderr, "marcc: error writing to standard output\n");
                  return 2;
               }
            }
         }
         
         do
         {
            retVal2 = write(sock, buffer, retVal);
         } while (retVal2==-1 && errno==EINTR);
         if (retVal2<retVal)
         {
            fprintf(stderr, "marcc: error while sending data to server\n");
            return 2;
         }
      }

      if (FD_ISSET(sock, &fds))
      {
         do
         {
            retVal = read(sock, buffer, BUFSIZE);
         } while (retVal==-1 && errno==EINTR);
         if (retVal==-1)
         {
            perror("marcc: error while receiving data from sserver");
            return 2;
         }
         if (retVal==0)
         {
            shutdown(sock, SHUT_RD);
            for (;;)
            {
               do
               {
                  retVal = read(descIn, buffer, BUFSIZE);
               } while (retVal==-1 && errno==EINTR);
               if (retVal==-1)
               {
                  perror("marcc: error while reading data from standard input");
                  return 2;
               }
               if (retVal==0)
               {
                  shutdown(sock, SHUT_RDWR);
                  close(sock);
                  return 0;
               }
               do
               {
                  retVal2 = write(sock, buffer, retVal);
               } while (retVal2==-1 && errno==EINTR);
               if (retVal2<retVal)
               {
                  fprintf(stderr, "marcc: error while sending data to server\n");
                  return 2;
               }
            }
         }
         
         do
         {
            retVal2 = write(descOut, buffer, retVal);
         } while (retVal2==-1 && errno==EINTR);
         if (retVal2<retVal)
         {
            fprintf(stderr, "marcc: error writing to standard output\n");
            return 2;
         }
      }
   }
}
