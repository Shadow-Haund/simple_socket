#include <iostream>
#include <boost/asio.hpp>
#include <functional>

class HzTimer{
    public:
    HzTimer(std::shared_ptr<boost::asio::steady_timer> timer, int mill_hz, std::function<void()> callback)
    : timer_(timer), mill_hz_(mill_hz), callback_(callback) {};

    void watchdog_timer(){
            timer_->expires_after(boost::asio::chrono::milliseconds(hz_to_millis(mill_hz_)));
            timer_->async_wait([this](const boost::system::error_code& e){
                if (e){ 
                    std::cerr << e.message() << std::endl; 
                } 
                callback_();
            });
        }
    int hz_to_millis(int mill_hz){
        return 1000/mill_hz;
    }
    
    private:
        std::shared_ptr<boost::asio::steady_timer> timer_;
        int mill_hz_;
        std::function<void()> callback_;

};