#include <iostream>
#include <exception>

#include "http/http_socket.h"

std::string itoa(uint64 i) {
    std::string ans = "";
    while (i != 0) {
        ans += (i % 10) + '0';
        i /= 10;
    }

    for (uint64 j = 0; j < ans.size() / 2; ++j) {
        char temp = ans[j];
        ans[j] = ans[ans.size()-1-j];
        ans[ans.size() - 1 - j] = temp;
    }

    return ans;
}

int main() {
    Socket server;
    HTTPSocket http_client;
    try {
        server.Bind(16000);
        server.Listen(5);

        //while(1) {
            Socket client = server.Accept();

            HTTPRequest request;
            http_client.SetSocket(client);
            http_client.FetchNextRequest(request);

            std::cout << request.fmt() << std::endl;

            // Send a reply
            HTTPResponse response;
            response.status_line_.version_ = HTTPVersion::kHTTP_1_0;
            response.status_line_.status_code_ = HTTPStatusCode::k200;

            // Set some headers
            response.headers_["access-control-allow-origin"] = "*";
            response.headers_["connection"] = "close";
            response.headers_["content-type"] = "text/plain";
            response.headers_["server"] = "KyleServer (Unix)";
            
            // Set a body
            response.body_ = "{\"test\": \"yep\"}";
            response.headers_["content-length"] = itoa(response.body_.size());

            std::cout << response.fmt() << std::endl;

            http_client.SendResponse(response);

        //    http_client.Close();
        //}

    } catch (std::exception& e) {
        http_client.Close();
        server.Close();
        std::cout << e.what() << std::endl;
        return -1;
    }

    http_client.Close();
    server.Close();

    return 0;
}


