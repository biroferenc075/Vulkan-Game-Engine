#include <iostream>
#include <string>

#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;
bool shouldStop = false;


class tcp_client : public boost::enable_shared_from_this<tcp_client> {
public:
    tcp_client(tcp::socket& socket) : socket_(socket) {}

    void readThread() {
        while (!shouldStop)
        {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket_.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof) {
                shouldStop = true;
                break; // Connection closed cleanly by peer.
            }
            else if (error) {
                shouldStop = true;
                throw boost::system::system_error(error); // Some other error.
            }
                
            std::cout.write(buf.data(), len);
            std::cout << std::endl;
        }
    }

    void writeThread() {
        std::string inp;
        do {
            std::cin >> inp;

            boost::asio::async_write(socket_, boost::asio::buffer(inp), 
                boost::bind(&tcp_client::handle_write, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        } while (inp.at(0) != 'x' && inp.length() == 1 && !shouldStop);
        shouldStop = true;
    }
private:
    tcp::socket& socket_;

    void handle_write(const boost::system::error_code& /*error*/,
        size_t /*bytes_transferred*/)
    {
    }
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: streamingclient <host>" << std::endl;
            return 1;
        }
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
            resolver.resolve(argv[1], "daytime");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        auto client = tcp_client(socket);
        
        boost::thread t(boost::bind(&tcp_client::readThread, &client));
        
        client.writeThread();
        
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

