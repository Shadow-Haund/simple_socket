#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include "../include/arg_parse.h"
#include "../include/timer.h"

using tcp = boost::asio::ip::tcp;
using io_c = boost::asio::io_context;
using sys_e = boost::system::error_code;

class tcp_client{
    public:
        tcp_client(io_c &io, int use_hz, int hz, std::string serv_ip, int serv_port, std::string msg)
        : io_context_(io), timer_(std::make_shared<boost::asio::steady_timer>(io)), server_end_(boost::asio::ip::make_address(serv_ip), serv_port),
        msg_(msg), hz_(hz), use_hz_(use_hz), socket_(io) {}

        void start(){
            socket_.async_connect(server_end_, [this](sys_e e){
                if (e){
                    std::cerr << e.message() << std::endl;
                }
                else{
                    std::cout << "Connection established" << std::endl;
                    recv_from_serv();
                    send_to_serv();
                }
            }); 
        }

        void set_msg(std::string msg_new){
            msg_ = msg_new;
        }
        
        std::string get_msg(){
            return msg_;
        }

    private:
        void send_to_serv(){
            socket_.async_send(boost::asio::buffer(msg_), [this](sys_e e, size_t buff_s){
                if (e)
                    std::cerr << e.message() << std::endl;
                    
                else if(use_hz_){
                    std::cout << "Sending using timer: " << msg_ << std::endl;
                    hz_timer(timer_, hz_, [this](){send_to_serv();});
                }
                else{
                    std::cout << "Sending with no timer: " << msg_ << std::endl;
                    send_to_serv();
                }
            });
        }


        void recv_from_serv(){
            socket_.async_read_some(boost::asio::buffer(buff_), [this](sys_e e, size_t buff_s){
                if (e)
                    std::cerr << e.message() << std::endl;
                else if (buff_s == 0){
                    std::cout << "Connection is clossed" << std::endl;
                    return;
                }
                else{
                    std::cout << "Receiving: " << std::string(buff_.data(), buff_s) << std::endl;
                    recv_from_serv();
                }
            });
        }

        io_c &io_context_;
        tcp::endpoint server_end_;
        tcp::endpoint client_end_;
        tcp::socket socket_;
        std::array<char, 1024> buff_;
        std::string msg_ = "";
        std::shared_ptr<boost::asio::steady_timer> timer_;
        int hz_;
        int use_hz_;
};

int main(int argc, char* argv[]){
    io_c io_context;
    std::shared_ptr<params> param_val = std::make_shared<params>();
    parse_args(argc, argv, param_val, false);
    tcp_client cl(io_context, param_val->use_hz, param_val->hz, param_val->serv_ip, param_val->serv_port, param_val->msg);
    cl.start();
    io_context.run();
    return 0;
}