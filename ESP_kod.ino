/*
Verze: 4.5
Finální prototyp kódu pro zařízení Meteo
Obsahuje: Funkční konfigurační prostředí HTML pro připojení se k sít,
funkční připojení k Azure IoTc a zasílání dat z čidel BME280 a SPG30. Integrovaný Deepsleep mód.

Optimalizováno na nejnižší spotřebu.
 */


#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include "web.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
//#include "MQ135.h"
#include "sensirion_common.h"
#include "sgp30.h"


//--------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include "config.h"
#include "src/iotc/common/string_buffer.h" //Custom knihovna na Azure pro spojení
#include "src/iotc/iotc.h" //Custom knihovna na Azure pro kompatibilitu s IOT central
//--------------------------------------------------------------------


#define SEALEVELPRESSURE_HPA (1013.25)
//#define MQ135pin (A0)

#define repeat(n) for (int i = 0; i < n; i++) 

// konfigurace pinu do config stavu
const int WAKEUP_PIN1 = 0;  //Config mod = Uzemnění SDA (GPIO2) do 1 vteřiny po resetu
bool BUTTONSTATE1 = false;



HTTPClient http;
WiFiClient client;
Adafruit_BME280 bme; // I2C
//MQ135 senzorMQ = MQ135(MQ135pin); 
unsigned long delayTime;


ADC_MODE(ADC_VCC); // 3.3v senzor napětí


//Konfigurace komunikace mezi ESP a Azure IOT central
//------------------------------------------------
const char* SCOPE_ID = "0ne005AD54E";
const char* DEVICE_ID = "meteo1";
const char* DEVICE_KEY = "JWK3SHPYhMc4Hbn2HOIEODgo0oGx7d79427mjjY99q0=";
//------------------------------------------------


//Nastavení pro Azure--------------------------------------------------
void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo);
#include "src/connection.h"

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo) {


  
  // Stav připojení--------------------------------------------------------------------
  if (strcmp(callbackInfo->eventName, "ConnectionStatus") == 0) {
    LOG_VERBOSE("Is connected ? %s (%d)",
                callbackInfo->statusCode == IOTC_CONNECTION_OK ? "YES" : "NO",
                callbackInfo->statusCode);
    isConnected = callbackInfo->statusCode == IOTC_CONNECTION_OK;
    return;
  }

 AzureIOT::StringBuffer buffer;
  if (callbackInfo->payloadLength > 0) {
    buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
  }

  LOG_VERBOSE("- [%s] event was received. Payload => %s\n",
              callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

  if (strcmp(callbackInfo->eventName, "Command") == 0) {
    LOG_VERBOSE("- Command name was => %s\r\n", callbackInfo->tag);
  }
}

//Prvotní nastavení pro Wifi-------------------------------------------------------------------
void wifi_setup() {
    // Zjistí SSID a Heslo, pomocí nich se připojí na síť
    WiFi.mode(WIFI_STA);
    String ssid = readEEPROM(230, 64);
    String password = readEEPROM(294, 100);
    char *s = const_cast<char *>(ssid.c_str()); 
    char *p = const_cast<char *>(password.c_str());
    Serial.print("Connection to SSID:");
    Serial.println(ssid);
    Serial.print("password:");
    Serial.println(password);
    delay(400);
    WiFi.begin(s, p);
    if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting");
    delay(100);
     }
    Serial.println();
    Serial.print("WiFi connected, IP address: "); //výpis přidělené adresy
    Serial.println(WiFi.localIP());
 }

bool check_conf_mode() {
    // Zjištění config módu
    if (!BUTTONSTATE1) {
        Serial.println("config mode:");
        setWiFiServer();
        return true;
       
    }  else
        return false;
}


//Setup pro boot mód-------------------------------------------------
void setup_boot() {
    Serial.begin(74880);
    EEPROM.begin(512);
    pinMode(WAKEUP_PIN1, INPUT_PULLUP);
    delay(1000);
    BUTTONSTATE1 = digitalRead(WAKEUP_PIN1);
    debug();
}


//Debugování paměti-------------------------------------------------
void debug() { 
    Serial.println(readEEPROM(SSID_POS, 64));
    Serial.println(readEEPROM(PASSWORD_POS, 64));
    Serial.println(readEEPROM(IP_POS1, 15));
    Serial.println(readEEPROM(HTTP_POS1, 100));
    Serial.println(readEEPROM(IP_POS2, 15));
    Serial.println(readEEPROM(HTTP_POS2, 100));
    Serial.print("BUTTONSTATE1=");
    Serial.println(BUTTONSTATE1);
}



//Setup------------------------------------------------------------
void setup() {
    setup_boot();
    if (!check_conf_mode()) {
        Serial.println("run mode");
        wifi_setup();
    }


    
bool status;
  Wire.begin(); //Inicializace I2C sběrnice
  status = bme.begin(0x76); 
 if (!status) {
    Serial.println("Senzor BME280 nebyl nalezen, zkontrolujte připojení!"); //Exit status pro chybu BME280
    while (1);
  }


//Nastavení Co2 a Tvoc senzoru------------------------------
    s16 err;
    u16 scaled_ethanol_signal, scaled_h2_signal;
      while (sgp_probe() != STATUS_OK) {
        Serial.println("SGP failed");
        while (1);
    }
     err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                            &scaled_h2_signal);
    if (err == STATUS_OK) {
        Serial.println("SPG30 Funkční");
    } else {
        Serial.println("SPG30 Porucha");
    }
    err = sgp_iaq_init();

    
//Měření po spuštění (sample)--------------------------------
 Serial.println("-- Default Test --");
  delayTime = 1000;
//Teplota [°C]
  Serial.println();
  Serial.print("Teplota = ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");
//Tlak [hPa]
  Serial.print("Tlak = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");
//Vlhkost vzduchu [%]
  Serial.print("Vlhkost = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
//Napětí (Za stabilizátorem, aka okolo 3.3V)[V]
  Serial.print("Napětí = ");
  Serial.print(ESP.getVcc()/ 1000.0);
  Serial.println(" V");

    delay(1000);



    
  delay(delayTime);

//------------------------------------------ 

connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY); //Přihlášení zařízení

if (context != NULL) {
    lastTick = 0;  // Nastavení času před poslední inicializaci příkazu
  }
Serial.println();
Serial.print("Připojeno");

 delay(1000);
    handleServer(); //Startuje server pro HTML





delay(500);
/*

if (!check_conf_mode()) {
//DeepSleep(); //variace pro rychlejší přístup do Deepsleep módu
}


*/


}



void loop() {
 if (!check_conf_mode()) {
  
  static int num = 0;
  num = num + 1;
  Serial.println(num);

    s16 err = 0;
    u16 tvoc_ppb, co2_eq_ppm;
    err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
    if (err == STATUS_OK) {
        Serial.print("tVOC  Concentration:");
        Serial.print(tvoc_ppb);
        Serial.println("ppb");

        Serial.print("CO2eq Concentration:");
        Serial.print(co2_eq_ppm);
        Serial.println("ppm");
    } else {
        Serial.println("error reading IAQ values\n");
    }
  
//Měření---------------------
int Teplota = bme.readTemperature();
int Vlhkost = bme.readHumidity();
int Napeti = ESP.getVcc() /1000.0;
int Tlak = bme.readPressure() / 100.0F;
delay(300);
int tVOC = tvoc_ppb;
int Co2 = co2_eq_ppm;
//---------------------------

 if ((Teplota < 100) && (Vlhkost < 101) && (Tlak < 1500)){ //neposílá nesmysly pokud se vyskytnou
  
  

  if (isConnected) {
    unsigned long ms = millis();
      char msg[64] = {0}; 
      int pos = 0, errorCode = 0;



//Posílání jednotlivých telemetrií------------------------------------------------------------
      lastTick = ms;
      if (loopId++ % 2 == 0) {  //Odeslání údajů
        pos = snprintf(msg, sizeof(msg) - 3, "{\"Teplota\": %d}",
                     Teplota);
        errorCode = iotc_send_telemetry(context, msg, pos);
        
        pos = snprintf(msg, sizeof(msg) - 1, "{\"Vlhkost\": %d}",
                       Vlhkost);
        errorCode = iotc_send_telemetry(context, msg, pos);

        pos = snprintf(msg, sizeof(msg) - 1, "{\"Napeti\": %d}",
                       Napeti);
        errorCode = iotc_send_telemetry(context, msg, pos);

         pos = snprintf(msg, sizeof(msg) - 1, "{\"Tlak\": %d}",
                       Tlak);
        errorCode = iotc_send_telemetry(context, msg, pos);

         pos = snprintf(msg, sizeof(msg) - 1, "{\"tVOC\": %d}",
                       tVOC);
        errorCode = iotc_send_telemetry(context, msg, pos);

         pos = snprintf(msg, sizeof(msg) - 1, "{\"Co2\": %d}",
                       Co2);
        errorCode = iotc_send_telemetry(context, msg, pos);

        delay(1000);
   
      } else {  //Odešli vlastnosti
       
      }
      msg[pos] = 0;

      if (errorCode != 0) {
        LOG_ERROR("Zpráva neodeslána, nastala chyba %d", errorCode);
      }
    }

    iotc_do_work(context);  // Urychlení práce pro IOT central (reconnect.endpoint)
  } 
  else {
    iotc_free_context(context);
    context = NULL;
    }


delay(1000);



if(num > 20){
ESP.deepSleep(600e5);    
BME280_Sleep(0x76);  
   }
}
else{

      handleServer();
      Serial.print("Conf mode, loop");
      delay(10000);
  }
  
}
       void BME280_Sleep(int device_address){
    Serial.println("BME280 přechází do DeepSleep módu...");
    Wire.beginTransmission(device_address);
    Wire.write((uint8_t)0xF4);
    Wire.write((uint8_t)0b00000000);
    Wire.endTransmission();
}
