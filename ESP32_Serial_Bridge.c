// ESP32 Wifi Serial Bridge

#include "config.h"
#include <WiFi.h>

#define OTA_HANDLER 
#define MODE_AP


#define PROTOCOL_TCP

bool debug = true;

#define VERSION "1.10"

// For AP mode:
const char *ssid = "Network Name Here";  // Network SSID (Change to desired)
const char *pw = "Pasword Here"; // Password to connect to network (Change to desired)
IPAddress ip(192, 168, 4, 1); // Connect to this IP
IPAddress netmask(255, 255, 255, 0);

// Connect the phone to above defined SSID
//                    Internet(TCP) -> 192.168.4.1:8880  for UART0
//                                  -> 192.168.4.1:8881  for UART1
//                                  -> 192.168.4.1:8882  for UART2





#define NUM_COM   3                 // total number of COM Ports
#define DEBUG_COM 0                 // debug output to COM0
/*************************  COM Port 0 *******************************/
#define UART_BAUD0 19200            // Baudrate UART0
#define SERIAL_PARAM0 SERIAL_8N1    // Data/Parity/Stop UART0
#define SERIAL0_RXPIN 21            // receive Pin UART0
#define SERIAL0_TXPIN 1             // transmit Pin UART0
#define SERIAL0_TCP_PORT 8880       // Wifi Port UART0
/*************************  COM Port 1 *******************************/
#define UART_BAUD1 19200            // Baudrate UART1
#define SERIAL_PARAM1 SERIAL_8N1    // Data/Parity/Stop UART1
#define SERIAL1_RXPIN 16            // receive Pin UART1
#define SERIAL1_TXPIN 17            // transmit Pin UART1
#define SERIAL1_TCP_PORT 8881       // Wifi Port UART1
/*************************  COM Port 2 *******************************/
#define UART_BAUD2 19200            // Baudrate UART2
#define SERIAL_PARAM2 SERIAL_8N1    // Data/Parity/Stop UART2
#define SERIAL2_RXPIN 15            // receive Pin UART2
#define SERIAL2_TXPIN 4             // transmit Pin UART2
#define SERIAL2_TCP_PORT 8882       // Wifi Port UART2

#define bufferSize 1024

//////////////////////////////////////////////////////////////////////////


#ifdef OTA_HANDLER  
#include <ArduinoOTA.h> 

#endif // OTA_HANDLER

HardwareSerial Serial_one(1);
HardwareSerial Serial_two(2);
HardwareSerial* COM[NUM_COM] = {&Serial, &Serial_one , &Serial_two};

#define MAX_NMEA_CLIENTS 4
#ifdef PROTOCOL_TCP
#include <WiFiClient.h>
WiFiServer server_0(SERIAL0_TCP_PORT);
WiFiServer server_1(SERIAL1_TCP_PORT);
WiFiServer server_2(SERIAL2_TCP_PORT);
WiFiServer *server[NUM_COM]={&server_0,&server_1,&server_2};
WiFiClient TCPClient[NUM_COM][MAX_NMEA_CLIENTS];
#endif


uint8_t buf1[NUM_COM][bufferSize];
uint16_t i1[NUM_COM]={0,0,0};

uint8_t buf2[NUM_COM][bufferSize];
uint16_t i2[NUM_COM]={0,0,0};

uint8_t BTbuf[bufferSize];
uint16_t iBT =0;


void setup() {

  delay(500);
  
  COM[0]->begin(UART_BAUD0, SERIAL_PARAM0, SERIAL0_RXPIN, SERIAL0_TXPIN);
  COM[1]->begin(UART_BAUD1, SERIAL_PARAM1, SERIAL1_RXPIN, SERIAL1_TXPIN);
  COM[2]->begin(UART_BAUD2, SERIAL_PARAM2, SERIAL2_RXPIN, SERIAL2_TXPIN);
  
  if(debug) COM[DEBUG_COM]->println("\n\nESP32_Serial_Bridge");
  #ifdef MODE_AP 
   if(debug) COM[DEBUG_COM]->println("Open ESP Access Point mode");
  //AP mode (phone connects directly to ESP) (no router)
  WiFi.mode(WIFI_AP);
   
  WiFi.softAP(ssid, pw); // configure ssid and password for softAP
  delay(2000); // VERY IMPORTANT
  WiFi.softAPConfig(ip, ip, netmask); // configure ip address for softAP

  #endif


  #ifdef MODE_STA
   if(debug) COM[DEBUG_COM]->println("Open ESP Station mode");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  if(debug) COM[DEBUG_COM]->print("try to Connect to Wireless network: ");
  if(debug) COM[DEBUG_COM]->println(ssid);
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    if(debug) COM[DEBUG_COM]->print(".");
  }
  if(debug) COM[DEBUG_COM]->println("\nWiFi connected");
  
  #endif

#ifdef OTA_HANDLER  
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request

  ArduinoOTA.begin();
#endif // OTA_HANDLER    

  #ifdef PROTOCOL_TCP
  COM[0]->println("Starting TCP Server 1");  
  if(debug) COM[DEBUG_COM]->println("Starting TCP Server 1");  
  server[0]->begin(); // start TCP server 
  server[0]->setNoDelay(true);
  COM[1]->println("Starting TCP Server 2");
  if(debug) COM[DEBUG_COM]->println("Starting TCP Server 2");  
  server[1]->begin(); // start TCP server 
  server[1]->setNoDelay(true);
  COM[2]->println("Starting TCP Server 3");
  if(debug) COM[DEBUG_COM]->println("Starting TCP Server 3");  
  server[2]->begin(); // start TCP server   
  server[2]->setNoDelay(true);
  #endif

  esp_err_t esp_wifi_set_max_tx_power(50);  //lower WiFi Power
}


void loop() 
{  
#ifdef OTA_HANDLER  
  ArduinoOTA.handle();
#endif // OTA_HANDLER

#ifdef PROTOCOL_TCP
  for(int num= 0; num < NUM_COM ; num++)
  {
    if (server[num]->hasClient())
    {
      for(byte i = 0; i < MAX_NMEA_CLIENTS; i++){
        //find free/disconnected spot
        if (!TCPClient[num][i] || !TCPClient[num][i].connected()){
          if(TCPClient[num][i]) TCPClient[num][i].stop();
          TCPClient[num][i] = server[num]->available();
          if(debug) COM[DEBUG_COM]->print("New client for COM"); 
          if(debug) COM[DEBUG_COM]->print(num); 
          if(debug) COM[DEBUG_COM]->println(i);
          continue;
        }
      }
      //no free/disconnected spot so reject
      WiFiClient TmpserverClient = server[num]->available();
      TmpserverClient.stop();
    }
  }
#endif
 
  for(int num= 0; num < NUM_COM ; num++)
  {
    if(COM[num] != NULL)          
    {
      for(byte cln = 0; cln < MAX_NMEA_CLIENTS; cln++)
      {               
        if(TCPClient[num][cln]) 
        {
          while(TCPClient[num][cln].available())
          {
            buf1[num][i1[num]] = TCPClient[num][cln].read(); // read char from client
            if(i1[num]<bufferSize-1) i1[num]++;
          } 

          COM[num]->write(buf1[num], i1[num]); // now send to UART(num):
          i1[num] = 0;
        }
      }
  
      if(COM[num]->available())
      {
        while(COM[num]->available())
        {     
          buf2[num][i2[num]] = COM[num]->read(); // read char from UART(num)
          if(i2[num]<bufferSize-1) i2[num]++;
        }
        // now send to WiFi:
        for(byte cln = 0; cln < MAX_NMEA_CLIENTS; cln++)
        {   
          if(TCPClient[num][cln])                     
            TCPClient[num][cln].write(buf2[num], i2[num]);
        }

        i2[num] = 0;
      }
    }    
  }
}