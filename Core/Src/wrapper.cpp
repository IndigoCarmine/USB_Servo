
#include <vector>
#include <array>
#include <stdexcept>
#include <algorithm>
#include "cpp_interface.hpp"
#include "tim.h"
#include "gpio.h"



namespace device_abstraction
{
class Servo
{
private:
	TIM_HandleTypeDef &htim;
	uint32_t channel;

public:
	Servo(TIM_HandleTypeDef& htim,uint32_t channel):htim(htim),channel(channel){};

    void start() const
    {
        HAL_TIM_PWM_Start(&htim, channel);
    }

    void stop() const
    {
        HAL_TIM_PWM_Stop(&htim, channel);
    }

    void set_angle(uint32_t angle) const
    {

        __HAL_TIM_SET_COMPARE(&htim, channel, angle+85);
    }
};
class Led
{
private:
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t last_update = 0;
    const uint32_t timeout;
public:
    Led(GPIO_TypeDef* port, uint16_t pin,uint32_t timeout = 100):port(port),pin(pin),timeout(timeout)
    {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    }

    void on()
    {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
        last_update = HAL_GetTick();
    }

    void update() const
    {
        if(HAL_GetTick() - last_update > timeout)
        {
            HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
        }
    }
};


const std::array<Servo,8> servos = {
    Servo(htim1, TIM_CHANNEL_1),
    Servo(htim1, TIM_CHANNEL_2),
    Servo(htim1, TIM_CHANNEL_3),

    Servo(htim2, TIM_CHANNEL_1),
    Servo(htim2, TIM_CHANNEL_2),
    Servo(htim2, TIM_CHANNEL_3),
    Servo(htim2, TIM_CHANNEL_4),

    Servo(htim3, TIM_CHANNEL_1),

};

std::array<Led,2> leds = {
    Led(LED1_GPIO_Port,LED1_Pin),
    Led(LED2_GPIO_Port,LED2_Pin),
};
}





namespace usb_interface
{
    void receive(const std::vector<uint8_t> buf)
    {
        // data structure
        /*
        uint8_t command <<4 | motor id
        ... : some data
        */
        /*
        command:
        0x00: set angle
        0x01: communication test
        0x02: start or stop motor
        */

        //connection establish request
        static const std::vector<uint8_t> HelloUSBServo{(uint8_t)0x10,'H','e','l','l','o','U','S','B','S','e','r','v','o'};
        if(buf.size() == HelloUSBServo.size() && std::equal(buf.begin(),buf.end(),HelloUSBServo.begin()))
        {
            static const std::vector<uint8_t> HelloThisIsUSBServo {(uint8_t)0x10,'H','e','l','l','o','T','h','i','s','I','s','U','S','B','S','e','r','v','o'};
            usb_interface::send(HelloThisIsUSBServo);
            return;
        }

        if(buf.size() != 2)return;
        switch (buf[0]>>4)
        {
        case 0x00:
            {
                const uint8_t servo_id = 0x0F & buf[0];
                const uint8_t angle = buf[1];
                if(device_abstraction::servos.size() > servo_id && servo_id >= 0){
                    device_abstraction::servos.at(servo_id).set_angle(angle);
                }else{
                    usb_interface::send({0x20,0x00});
                    return;
                }
            }
            break;
        case 0x02:
            {
                const uint8_t active_motor = buf[1];
                for(int i=0;i<8;i++){
                    if(active_motor & (1<<i)){
                        device_abstraction::servos.at(i).start();
                    }else{
                        device_abstraction::servos.at(i).stop();
                    }
                }
            }
            break;
        default:
            {
                usb_interface::send({0x30,0x00});
                return;
            }
            break;
        }

        
        
    }

    void on_received()
    {
        device_abstraction::leds.at(0).on();
    }
} // namespace usb_interface

namespace main_interface
{

    void main_cpp()
    {
        device_abstraction::leds.at(1).on();

        while (1)
        {
            //led update
            for(auto& led:device_abstraction::leds)
            {
                led.update();
            }
        }
    }
} // namespace main_interface
