//
// Created by lex on 21.06.22.
//
#include "toIP.h"
#include <ctype.h>
#include <EEPROM.h>
#include <HardwareSerial.h>

uint8_t* toIP::raw_address() {
    return _address.bytes;
}

bool toIP::get_from_string(const char *address) {

    uint16_t acc = 0; // Accumulator
    uint16_t dots = 0;

    while (*address)
    {
        char c = *address++;
        if (isdigit(c)){
            acc = acc * 10 + (c - '0');
            if (acc > 255) {
                _address.bytes[0] = 0;
                _address.bytes[1] = 0;
                _address.bytes[2] = 0;
                _address.bytes[3] = 0;
                return false;
            }
        }
        else if (c == '.')
        {
            if (dots == 3) {
                _address.bytes[0] = 0;
                _address.bytes[1] = 0;
                _address.bytes[2] = 0;
                _address.bytes[3] = 0;
                return false;
            }
            _address.bytes[dots] = acc;
            dots++;
            acc = 0;
        }
    }

    if (dots != 3) {
        // Too few dots (there must be 3 dots)
        _address.bytes[0] = 0;
        _address.bytes[1] = 0;
        _address.bytes[2] = 0;
        _address.bytes[3] = 0;
        return false;
    }
    _address.bytes[3] = acc;
    return true;
}

//toIP::toIP() {
//    _address.bytes[0] = 0;
//    _address.bytes[1] = 0;
//    _address.bytes[2] = 0;
//    _address.bytes[3] = 0;
//}

bool toIP::save_ip(uint8_t position) {
    EEPROM.write(position*4, _address.bytes[0]);
    EEPROM.write(position*4+1, _address.bytes[1]);
    EEPROM.write(position*4+2, _address.bytes[2]);
    EEPROM.write(position*4+3, _address.bytes[3]);
    return true;
}

bool toIP::load_ip(uint8_t position) {
    _address.bytes[0] = EEPROM.read(position*4);
    _address.bytes[1] = EEPROM.read(position*4+1);
    _address.bytes[2] = EEPROM.read(position*4+2);
    _address.bytes[3] = EEPROM.read(position*4+3);
    return true;
}

//toIP::toIP(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
//    _address.bytes[0] = first_octet;
//    _address.bytes[1] = second_octet;
//    _address.bytes[2] = third_octet;
//    _address.bytes[3] = fourth_octet;
//}

void toIP::setIP(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
    _address.bytes[0] = first_octet;
    _address.bytes[1] = second_octet;
    _address.bytes[2] = third_octet;
    _address.bytes[3] = fourth_octet;
}
