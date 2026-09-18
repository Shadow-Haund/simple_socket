#include <iostream>
#include <boost/asio.hpp>
#include <functional>


int hz_to_millis(int mill_hz){
    return 1000/mill_hz;
}

void hz_timer(std::shared_ptr<boost::asio::steady_timer>& timer, int& mill_hz, std::function<void()> callback){
    timer->expires_after(boost::asio::chrono::milliseconds(hz_to_millis(mill_hz)));
    timer->async_wait([&timer, &mill_hz, callback_ = std::move(callback)](const boost::system::error_code& e){
        if (e){ 
            std::cerr << e.message() << std::endl; 
        } 
        callback_();
    });
}