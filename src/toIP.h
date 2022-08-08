//
// Created by lex on 21.06.22.
//

#ifndef JSPPINGER_TOIP_H
#define JSPPINGER_TOIP_H
#include <EEPROM.h>
#include "IPAddress.h"

class toIP: public IPAddress{
private:
    union {
        uint8_t bytes[4];  // IPv4 address
        uint32_t dword;
    } _address;
public:
    uint8_t* raw_address();
    bool get_from_string(const char *address);
    bool save_ip(uint8_t position);
    bool load_ip(uint8_t position);

    IPAddress cast_to(){
        uint8_t *address = raw_address();
        return IPAddress(address[0],address[1],address[2],address[3]);
    }

    void setIP(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);

    toIP(){toIP::IPAddress();}
    toIP(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet)
    {
        toIP::IPAddress(first_octet,second_octet,third_octet,fourth_octet);
    }

    //toIP();
    //toIP(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);

};


#endif //JSPPINGER_TOIP_H
