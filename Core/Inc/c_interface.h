/*
 * cpp_api.h
 *
 *  Created on: Dec 25, 2023
 *      Author: taman
 */

#ifndef INC_CPP_API_H_
#define INC_CPP_API_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif
void usb_recived();
void usb_receive(uint8_t* buf, uint32_t len);
void main_cpp();
#ifdef __cplusplus
}
#endif

#endif /* INC_CPP_API_H_ */
