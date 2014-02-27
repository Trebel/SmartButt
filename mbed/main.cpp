#include "mbed.h"
#include "M2XStreamClient.h"
#include "include_me.h"
#include "MMA8451Q.h"

using namespace mts;

const char key[] = "a88396a6bcdba5898295bf715e4529b8";  // m2x user account master key (given key is inactive)
const char feed[] = "62b83d2c715f6ddcc38056a0f0023d0f"; // blueprint feed id (given key is inactive)
const char f1b1s1Stream[] = "f1b1s1";   // Create a stream name

// ssid and phrase for wifi
std::string ssid = "HackAThon";
std::string phrase = "F0undry!";
Wifi::SecurityType security_type = Wifi::WPA;

//////////////////////////////////////////////////////////////////////
// Include support for on-board green and red LEDs
#define LED_ON  0
#define LED_OFF 1
DigitalOut greenLED(LED_GREEN);
DigitalOut redLED(LED_RED);

// Setup pin 20 (A5) for analog input
AnalogIn ain(A5);



/////////////////////////////////////////////////////////////////////
// Prototype for LED flash routine
void ledFlashTick(bool);

int main()
{
    
    // Connect to WiFi
    for (int i = 6; i >= 0; i = i - 2) {
        wait(2);
        printf("Waiting %d seconds...\n\r", i);
    }
    MTSSerial* serial = new MTSSerial(PTD3, PTD2, 256, 256);
    serial->baud(9600);
    Transport::setTransport(Transport::WIFI);
    Wifi* wifi = Wifi::getInstance();
    printf("Init: %s\n\r", wifi->init(serial) ? "SUCCESS" : "FAILURE");
    printf("Set Network: %s\n\r", getCodeNames(wifi->setNetwork(ssid, security_type, phrase)).c_str());
    printf("Set DHCP: %s\n\r", getCodeNames(wifi->setDeviceIP("DHCP")).c_str());
    printf("Signal Strnegth (dBm): %d\n\r", wifi->getSignalStrength());
    printf("Is Connected: %s\n\r", wifi->isConnected() ? "True" : "False");
    printf("Connect: %s\n\r", wifi->connect() ? "Success" : "Failure");
    printf("Is Connected: %s\n\r", wifi->isConnected() ? "True" : "False");

    // Initialize LEDs
    greenLED = LED_OFF;
    redLED = LED_OFF;

    // Send some data 
    Client client;
    M2XStreamClient m2xClient(&client, key);
    int ret;
    int val;

    while (true) {
        if(ain >= .1) {
            val = 1;
        } else {
            val = 0;
        }
        ledFlashTick(val);
        
        printf("ain %f\r\n", ain);
        printf("sending %d\r\n", val);
        ret = m2xClient.send(feed, f1b1s1Stream, val);
        printf("send() returned %d\r\n", ret); 
        
        
        
        wait(5);
    }
}

void ledFlashTick(bool vacant)
{
    greenLED = vacant;
    redLED = !vacant;
}
