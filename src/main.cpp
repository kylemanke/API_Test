#include <iostream>
#include <exception>

#include "http/http_socket.h"

int main() {
    Socket server;
    HTTPSocket http_client;
    try {
        server.Bind(16000);
        server.Listen(5);

        Socket client = server.Accept();

        HTTPRequest request;
        http_client.SetSocket(client);
        http_client.FetchNextRequest(request);

        std::cout << request.fmt() << std::endl; 
    } catch (std::exception& e) {
        server.Close();
        http_client.Close();
        std::cout << e.what() << std::endl;
        return -1;
    }

    server.Close();
    http_client.Close();

    return 0;
}


