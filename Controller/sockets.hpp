#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <string>
#include <cstring>
#include <exception>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <sstream>

//#include "gameWindow.hpp"

class Sockets {
    private:
        int max_fd;
    
        fd_set fd_reads;

        int connection_socket;

        std::string ip;

        const int STD_SIZE = 8192;

        unsigned int connection_number;

        int port;

        struct timeval dropout_time = {0,50};

        std::vector<int> sockets_list;

        std::mutex mutex_alter_socket_list;
        std::mutex mutex_alter_inbound_messages;
        std::mutex mutex_alter_outbound_messages;
        std::mutex mutex_halt_loop;

        std::mutex *mutex_alter_inbound_messages_other;
        std::mutex *mutex_alter_outbound_messages_other;

        std::vector<std::pair<std::string, int>> inbound_messages;
        std::vector<std::pair<std::string, int>> outbound_messages;

        std::vector<std::string> *inbound_messages_other;
        std::vector<std::string> *outbound_messages_other;

        void receiveMessage(int nsock);

        void acceptConnections();

        int receiveLoopClient();

        int receiveLoopServer();

        void sendLoopClient();
        
        void sendLoopServer();

    public:
        void configureMessaging(std::mutex *mutex_alter_inbound_messages_other, std::mutex *mutex_alter_outbound_messages_other, std::vector<std::string> *inbound_messages_other, std::vector<std::string> *outbound_messages_other);

        void clearSocket();

        int startServer();

        int startClient();

        int sendMessage(int fd, std::string msg);

        void endConnection(std::thread* network_thread);

        void realClientLoop();

        void realServerLoop();

        Sockets(std::string ip, unsigned int connection_number, int port) {
            Sockets::ip = ip;
            Sockets::connection_number = connection_number;
            Sockets::port = port;
        }

        bool halt_loop = false;
};
    // Sockets sock = Sockets("127.0.0.1", 128, 4277);