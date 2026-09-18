#include <iostream>
#include <boost/asio.hpp>
#include <optional>

using tcp = boost::asio::ip::tcp;
using io_c = boost::asio::io_context;
using sys_e = boost::system::error_code;


class session: public std::enable_shared_from_this<session>{
    public:
        session(tcp::socket && socket_move)
        :  socket_session_(std::move(socket_move)){}

    void session_receive(){
        send_to_serv();
        recv_from_serv();
    }

    void set_msg(std::string msg_new){
            msg_ = msg_new;
        }
        
    std::string get_msg(){
        return msg_;
    }

    private:
        void send_to_serv(){
            socket_session_.async_send(boost::asio::buffer(msg_), [self = shared_from_this()](sys_e e, size_t buff_s){
                if (e){
                    std::cerr << e.message() << std::endl;
                    return;
                }
                else{
                    std::cout << "Sending: " << self->msg_ << std::endl;
                    self->send_to_serv();
                }
            });
        }


        void recv_from_serv(){
            socket_session_.async_read_some(boost::asio::buffer(buff_), [self = shared_from_this()](sys_e e, size_t buff_s){
                if (e){
                    std::cerr << e.message() << std::endl;
                    return;
                }
                else if (buff_s == 0){
                    std::cout << "Connection is clossed" << std::endl;
                    return;
                }
                else{
                    std::cout << "Receiving: " << std::string(self->buff_.data(), buff_s) << std::endl;
                    self->recv_from_serv();
                }
            });
        }

        tcp::socket socket_session_;
        std::string msg_ = "from_cl_to_srv";
        std::array<char, 1024> buff_;
};

class server{
    public:
        server(io_c &io, int serv_port)
        : io_context_(io), server_end_(tcp::v4(), serv_port), acceptor_(io, server_end_) {}

        void start(){
            socket_.emplace(io_context_);
            acceptor_.async_accept(*socket_, [this](sys_e e){
                if (e){
                    std::cerr << e.message() << std::endl;
                }
                else{
                    std::cout << "Connection established, creating session" << std::endl;
                    std::make_shared<session>(std::move(*socket_))->session_receive();
                    start();
                }
            });
        }
    private:

        io_c &io_context_;
        tcp::endpoint server_end_;
        tcp::acceptor acceptor_;
        std::optional<tcp::socket> socket_;
};


int main(int argc, char* argv[]){
    io_c io_context;
    int serv_port = 15000;
    if (argc == 2) serv_port = std::stoi(argv[1]);
    server serv(io_context, serv_port);
    serv.start();
    io_context.run();
    return 0;
}