#include <Arduino.h>
#include "Ethernet.h"
#include "utility/w5100.h"
#include <EthernetICMP.h>
#include "toIP.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // max address for ethernet shield

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
byte selector = 0;
char buffer [256];
EthernetICMPPing ping(0, (uint16_t)random(0, 255));

toIP pingAddr_1(127,0,0,1);
toIP pingAddr_2(127,0,0,1);

unsigned long previousMillis = 0;        // will store last time LED was updated
uint16_t interval = 500;           // interval at which to blink (milliseconds)

EthernetClass Shield;

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        inputString += inChar;
        // if the incoming character is a newline, set a flag
        // so the main loop can do something about it:
        if (inChar == '\r') {
            if (inputString.length()==2)
                selector = byte(inputString.charAt(0));
            else if (inputString.length()==1)
                selector = 13;
            stringComplete = true;
        }
    }
}

uint8_t* parse_string(String address, uint8_t position){
    uint8_t *result;
    auto *ipaddr = new toIP();
    if (address.length()>7) {
        if (ipaddr->get_from_string(address.c_str())) {
            auto result = ipaddr->raw_address();
            ipaddr->save_ip(position);
            sprintf(buffer, "New IP: %d.%d.%d.%d", result[0], result[1], result[2], result[3]);
        }
    }
    else {
        ipaddr->load_ip(position);
        auto result = ipaddr->raw_address();
        sprintf(buffer, "Default IP: %d.%d.%d.%d", result[0], result[1], result[2], result[3]);
    }

    Serial.println(buffer);
    return result;
}


toIP get_addr(uint8_t position){
    toIP result;
    result.load_ip(position);
    return result;
}

void load_preferences(){
    auto result = get_addr(0);
    uint8_t *address = result.raw_address();
    sprintf(buffer, "IP: %d.%d.%d.%d", address[0],address[1],address[2],address[3]);
    Serial.println(buffer);

    Shield.setLocalIP(result.cast_to());

    pingAddr_1 = get_addr(1);
    uint8_t *address_1 = pingAddr_1.raw_address();
    sprintf(buffer, "ping IP: %d.%d.%d.%d", address_1[0],address_1[1],address_1[2],address_1[3]);
    Serial.println(buffer);

    pingAddr_2 = get_addr(2);
    uint8_t *address_2 = pingAddr_2.raw_address();
    sprintf(buffer, "ping IP: %d.%d.%d.%d", address_2[0],address_2[1],address_2[2],address_2[3]);
    Serial.println(buffer);
}

void print_preferences(){
    auto result = get_addr(0);
    uint8_t *address = result.raw_address();
    sprintf(buffer, "IP: %d.%d.%d.%d", address[0],address[1],address[2],address[3]);
    Serial.println(buffer);

    result = get_addr(1);
    uint8_t *address_1 = result.raw_address();
    sprintf(buffer, "ping IP: %d.%d.%d.%d", address_1[0],address_1[1],address_1[2],address_1[3]);
    Serial.println(buffer);

    result = get_addr(2);
    uint8_t *address_2 = result.raw_address();
    sprintf(buffer, "ping IP: %d.%d.%d.%d", address_2[0],address_2[1],address_2[2],address_2[3]);
    Serial.println(buffer);
}

void check_uart(){
    if (stringComplete) {
        switch (selector){
            case 48:{
                print_preferences();
                break;
            }
            case 49:{
                auto ps = parse_string(inputString,0);
                break;
            }
            case 50:{
                auto ps = parse_string(inputString,1);
                break;
            }
            case 51:{
                auto ps = parse_string(inputString,2);
                break;
            }
            case 52:{
                load_preferences();
                Serial.println("Reload config done");
                break;
            }
            case 13:{
                Serial.println();
                sprintf(buffer, "Hello, Select your option:");
                Serial.println(buffer);
                sprintf(buffer, "0: Print current config");
                Serial.println(buffer);
                sprintf(buffer, "1: Setup device ip Addr");
                Serial.println(buffer);
                sprintf(buffer, "2: First ping IP addr");
                Serial.println(buffer);
                sprintf(buffer, "3: Second ping IP addr");
                Serial.println(buffer);
                sprintf(buffer, "4: Reload preferences");
                Serial.println(buffer);
                break;
            }
            default:break;

        }
        // clear the string:
        inputString = "";
        stringComplete = false;
    }
}




void setup() {
    pinMode(8, OUTPUT);
    digitalWrite(8, LOW);

    Serial.begin(9600);
    Shield.begin(mac,get_addr(0).cast_to());
    load_preferences();
}

void loop() {
    check_uart();
    unsigned long currentMillis = millis();
//    delay(500);
    if(currentMillis - previousMillis > interval) {

        // save the last time you blinked the LED
        previousMillis = currentMillis;
        EthernetICMPEchoReply  echoReply_1 = ping(pingAddr_1.cast_to(), 1);
        EthernetICMPEchoReply  echoReply_2 = ping(pingAddr_2.cast_to(), 1);
        if (echoReply_1.status == SUCCESS)
        {
            Serial.println("IP1 ok");
            digitalWrite(8, LOW);
        }
        else if (echoReply_2.status == SUCCESS)
        {
            Serial.println("IP2 ok");
            digitalWrite(8, LOW);
        }
        else
        {
            Serial.println("starlink up");
            digitalWrite(8, HIGH);
        }
    }
}