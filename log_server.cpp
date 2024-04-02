#include <iostream>
#include <fstream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

class Server {
public:
    Server(io_service& ioService, short port) : acceptor(ioService, tcp::endpoint(tcp::v4(), port)),
                                                socket(ioService) {
        startAccept();
    }

private:
    void startAccept() {
        acceptor.async_accept(socket, [this](const boost::system::error_code& ec) {
            if (!ec) {
                std::cout << "New connection established.\n";
                log("New connection established.");
                startRead();
            }
            startAccept();
        });
    }

    void startRead() {
        async_read_until(socket, buffer, '\n', [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            if (!ec) {
                std::istream is(&buffer);
                std::string message;
                std::getline(is, message);

                std::cout << "Received message: " << message << "\n";
                log("Received message: " + message);

                startRead();
            } else {
                std::cerr << "Error reading from socket: " << ec.message() << "\n";
                log("Error reading from socket: " + ec.message());
            }
        });
    }

    void log(const std::string& message) {
        std::ofstream logFile("server_log.txt", std::ios_base::app);
        logFile << message << "\n";
    }

    tcp::acceptor acceptor;
    tcp::socket socket;
    streambuf buffer;
};

int main() {
    try {
        boost::asio::io_service ioService;
        Server server(ioService, 12345);
        ioService.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
