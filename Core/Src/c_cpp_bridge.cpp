/*
 * cpp_api.c
 *
 *  Created on: Dec 25, 2023
 *      Author: taman
 */

#include <vector>

#include "c_interface.h"
#include "cpp_interface.hpp"

#include "usbd_cdc_if.h"


// from c_interface.h
extern "C" {
    void usb_receive(uint8_t* buf, uint32_t len) {
        const auto buf_vec = std::vector<uint8_t>(buf,buf+len);
    	usb_interface::receive(buf_vec);
    }

    void usb_recived(){
    	usb_interface::on_received();
    }
    void main_cpp() {
    	main_interface::main();
    }
}

// from cpp_interface.hpp
namespace usb_interface
{
    //prototypes
	void cobs_encode(const uint8_t data[], uint8_t return_data[], int data_size);


    //cobs encoded send
    void send(const std::vector<uint8_t> buf) {
        std::vector<uint8_t> encoded_buf(buf.size() + 2);
        cobs_encode(buf.data(), encoded_buf.data(), buf.size());
    	CDC_Transmit_FS(encoded_buf.data(), encoded_buf.size());
    }


    void cobs_encode(const uint8_t data[], uint8_t return_data[], int data_size)
    {
        int zero_index = data_size + 1; // this is return_data index
        return_data[zero_index] = 0x00;
        for (int i = data_size; i > 0; i--)
        {
            if (data[i - 1] == 0x00)
            {
                return_data[i] = (uint8_t)(zero_index - i);
                zero_index = i;
            }
            else
            {
                return_data[i] = data[i - 1];
            }
        }
        return_data[0] = zero_index;
    }

} // namespace USB


