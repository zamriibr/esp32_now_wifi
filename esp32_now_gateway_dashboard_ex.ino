/*
  Daniel Carrasco
  This and more tutorials at https://www.electrosoftcloud.com/
*/
#include <esp_now.h>
#include <WiFi.h>
#include <HTTPClient.h>
static const char* ssid     = "your-wifi-ssid";
static const char* password = "your-wifi-password";

String apiKey_thingspeak = "your-thingspeak-apikey";
String apiKey_ifft = "your-ifft-apikey";
bool new_data=false;

// Structure to keep the temperature and humidity data
// Is also required in the client to be able to save the data directly
typedef struct temp_humidity {
  float temperature;
  float humidity;
};
// Create a struct_message called myData
temp_humidity dhtData;
// callback function executed when data is received
void OnRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&dhtData, incomingData, sizeof(dhtData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Temperature: ");
  Serial.println(dhtData.temperature);
  Serial.print("Humidity: ");
  Serial.println(dhtData.humidity);

  new_data = true;
  
}
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_AP_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  //Get MAC address
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("There was an error initializing ESP-NOW");
    return;
  }
  
  // Once the ESP-Now protocol is initialized, we will register the callback function
  // to be able to react when a package arrives in near to real time without pooling every loop.
  esp_now_register_recv_cb(OnRecv);
}
void loop() {

  if(new_data == true)
  {
    HTTPClient http;
  
    // Your Domain name with URL path or IP address with path
    // example http://maker.ifttt.com/trigger/data_new/with/key/my-api-key?value1=10&value2=20
    
    // set url for thingspeak or/and iffft
    String url = "http://api.thingspeak.com/update?api_key=" + apiKey_thingspeak + "&field1=" + String(dhtData.temperature) + "&field2=" + String(dhtData.humidity);
    //String url = "http://maker.ifttt.com/trigger/data_new/with/key/" + apiKey_ifft + "?value1=" + String(dhtData.temperature) + "&value2=" + String(dhtData.humidity);
    
    http.begin(url);
    int httpCode = http.GET();
    Serial.println(httpCode);
    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
      Serial.println("data sent");
    }
  
     http.end();
     new_data = false;
     delay(1000);
  }
  
}
