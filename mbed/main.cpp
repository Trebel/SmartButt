#include "mbed.h"
#include "M2XStreamClient.h"
#include "include_me.h"

using namespace mts;

const char key[] = "a88396a6bcdba5898295bf715e4529b8";  // enter your m2x user account master key
const char feed[] = "62b83d2c715f6ddcc38056a0f0023d0f"; // enter your blueprint feed id
const char stream[] = "switchvoltage";   // Create a stream name

// set to 1 for cellular shield board
// set to 0 for wifi shield board
#define CELL_SHIELD 0

// ssid and phrase for wifi
std::string ssid = "HackAThon";
std::string phrase = "F0undry!";
Wifi::SecurityType security_type = Wifi::WPA;

int main()
{
#if CELL_SHIELD
    MTSSerialFlowControl* serial = new MTSSerialFlowControl(PTD3, PTD2, PTA12, PTC8);
    serial->baud(115200);
    Transport::setTransport(Transport::CELLULAR);
    Cellular* cell = Cellular::getInstance();
    cell->init(serial, PTA4, PTC9); //DCD and DTR pins for KL46Z

    int max_tries = 5;
    int i;
    std::string apn = "wap.cingular";

    i = 0;
    while (i++ < max_tries) {
        if (cell->getRegistration() == Cellular::REGISTERED) {
            printf("registered with tower\n\r");
            break;
        } else if (i >= max_tries) {
            printf("failed to register with tower\n\r");
        } else {
            wait(3);
        }
    }

    printf("signal strength: %d\n\r", cell->getSignalStrength());

    i = 0;
    printf("setting APN to %s\n\r", apn.c_str());
    while (i++ < max_tries) {
        if (cell->setApn(apn) == SUCCESS) {
            printf("successfully set APN\n\r");
            break;
        } else if (i >= max_tries) {
            printf("failed to set APN\n\r");
        } else {
            wait(1);
        }
    }

    i = 0;
    printf("bringing up PPP link\n\r");
    while (i++ < max_tries) {
        if (cell->connect()) {
            printf("PPP link is up\n\r");
            break;
        } else if (i >= max_tries) {
            printf("failed to bring PPP link up\n\r");
        } else {
            wait(1);
        }
    }
#else
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
#endif

    /* send some data */
    Client client;
    M2XStreamClient m2xClient(&client, key);
    int ret;
    int num;
    while (true) {
        num = rand();
        printf("sending %d\r\n", num);
        ret = m2xClient.send(feed, stream, num);
        printf("send() returned %d\r\n", ret);
        wait(5);
    }
}