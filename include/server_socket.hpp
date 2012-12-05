#ifndef __SERVER_SOCKET_HPP__
#define __SERVER_SOCKET_HPP__

#include<memory>
#include "new_socket.hpp"

#define MAX_LISTEN 20

template <typename T>
class ServerSocket: public Socket<T,typename std::enable_if<Convertible<T*, base_addr*>()>::type>
{
    public:
        //ServerSock(int listen_port);
        ServerSocket(unsigned int listen_port,string ipaddress="localhost", int max_listen = 10);
        ServerSocket(string listen_port,string ipaddress="localhost", int max_listen = 10);
        unique_ptr<Socket<T>> acceptConn(unique_ptr<Socket<T>> newSock);
        
        // Destructor
		~ServerSocket() {}
		
    private:
        string _ipaddress;
        int _listen_port;
        int _max_listen; 

};
//if Port number is int
template <typename T>
ServerSocket<T>::ServerSocket(unsigned int listen_port, string ipaddress,int max_listen):_ipaddress(ipaddress),_listen_port(listen_port), _max_listen(max_listen)
{
    
    T addr(std::to_string(_listen_port),_ipaddress);
    struct addrinfo *p = addr.getResult();
    for(p = addr.getResult(); p != NULL; p = p->ai_next) 
    {
        if (bind(this->getSockfd(), p->ai_addr, p->ai_addrlen) == -1)
        {
        	close(this->getSockfd());
            throw SockError("Error in Bind");
            continue;
        }
        else
        {
            break;    
        }
    }

    if (p == NULL)
    {
        //bind error, throw expection    
        return;
    }

    if (listen(this->getSockfd(), _max_listen) < 0)
    {
        throw SockError("Error in Listen."); 
    }
}


//If port number given in String    
template <typename T>
ServerSocket<T>::ServerSocket(string listen_port,string ipaddress, int max_listen):_ipaddress(ipaddress),_listen_port(stoi(listen_port)), _max_listen(max_listen)
{
    T addr(listen_port,_ipaddress);
    struct addrinfo *p = addr.getResult();
    std::cout<<"Sock fd is "<<this->getSockfd()<<std::endl;
    for(p = addr.getResult(); p != NULL; p = p->ai_next) 
    {
        std::cout<<"Addr "<<p->ai_addrlen<<std::endl;
        if (bind(this->getSockfd(), p->ai_addr, p->ai_addrlen) == -1)
        {
        	close(this->getSockfd());
            throw SockError("Error in Bind");
            continue;
        }
        else
        {
            std::cout<<"Bind done"<<std::endl;
            break;
        }
    }
    
    if (p == NULL)
    {
        //bind error, throw expection    
        return;
    }

    if (listen(this->getSockfd(), _max_listen) < 0)
    {
        throw SockError("Error in Listen");  
    }
    else
    {
        std::cout<<"Listening";    
    }
}


template <typename T>
unique_ptr<Socket<T>> ServerSocket<T>::acceptConn(unique_ptr<Socket<T>> newSock)
{
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  unsigned int newsockfd = accept(this->getSockfd(), (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) 
         throw SockError("Error in Accept"); 
  
  newSock.reset(new Socket<T>(newsockfd));
  return newSock; 
}


using server_sock_stream = ServerSocket<inet_stream_addr>;
#endif 
