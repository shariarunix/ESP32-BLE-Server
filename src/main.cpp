#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool isLEDBlinking = false;
int delayInMilis = 1000;

// Define the service and characteristic UUIDs
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "abcd1234-5678-1234-1234-123456789abc"

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Client Connected");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Client Disconnected");
  }
};

class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue().c_str();
    Serial.print("Received Command: ");
    Serial.println(value);

    if (value == "ON") {
      Serial.println("Command: Turn On");
      isLEDBlinking = true;
    } else if (value == "OFF") {
      Serial.println("Command: Turn Off");
      isLEDBlinking = false;
    }else if(value == "PLUS") {
      delayInMilis += 1000;
      Serial.print("Current Delay : ");
      Serial.println(delayInMilis);
    }else if(value == "MINUS"){
      if(delayInMilis > 1000){
        delayInMilis -= 1000;
        Serial.print("Current Delay : ");
        Serial.println(delayInMilis);
      }
    }else {
      Serial.println("Unknown command");
    }
  }
};

void setup() {
  Serial.begin(921600);

  Serial.println("Device Started");

  BLEDevice::init("ESP32_BLE_Server_UNIX");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
  
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  Serial.println("Waiting for a client to connect...");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if(isLEDBlinking){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayInMilis);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delayInMilis);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
