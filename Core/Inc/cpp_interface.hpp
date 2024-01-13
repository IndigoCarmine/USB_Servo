#pragma once

#include <cstdint>
#include <vector>

namespace usb_interface
{
    //it call when data received and its last byte is 0x00
	void receive(const std::vector<uint8_t> buf);
	//__attribute__((weak))

    //it call when nvic interrupt is triggered
	void on_received();


    //cobs encoded send
    void send(const std::vector<uint8_t> buf);
} // namespace USB

namespace main_interface{
    //main function after init
    void main();
} // namespace main_interface
