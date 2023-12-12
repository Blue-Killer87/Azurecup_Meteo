
#define IP_POS1 0
#define IP_POS2 15
#define HTTP_POS1 30
#define HTTP_POS2 130
#define SSID_POS 230
#define PASSWORD_POS 294

void setWiFiServer();

String readEEPROM(int numberOfStart, int len);

void handleServer();

void serversOn();
