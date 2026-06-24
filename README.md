Smart Greenhouse Automation (ESP32 + Firebase)This repository contains an Arduino sketch (sketch_may17a.ino) for an automated greenhouse control system powered by an ESP32. It monitors environmental data and receives real-time actuator control commands using Google's Firebase Realtime Database.
🚀 FeaturesReal-time Environmental Monitoring: Tracks temperature, relative humidity, and 3 channels of soil moisture.
Actuator Remote Control: Listens for remote commands to toggle a cooling fan and three independent water pumps.
Firebase Integration: Uses the Firebase_ESP_Client library to stream data and read commands seamlessly over Wi-Fi.Fail-safe Startup: 
Initializes all relays to a safe OFF state upon boot to prevent accidental flooding or power surges.
🛠️ Hardware Requirements & PinoutComponentsMicrocontroller: 
ESP32   Ambient Sensor: DHT22 (Temperature & Humidity)   Soil Moisture Sensors: 3x Capacitive/Resistive Soil Moisture Sensors  
Actuators: 1x Fan, 3x Water Pumps (Driven via an Active-Low Relay Module)   
Pin ConfigurationComponentESP32 PinDetailsDHT22 
SensorGPIO 33Ambient temperature and humidity   
Soil Sensor 1GPIO 32Analog Input   
Soil Sensor 2GPIO 35Analog Input   
Soil Sensor 3GPIO 34Analog Input   
Fan RelayGPIO 25Active-Low Relay   
Pump 1 RelayGPIO 14Active-Low Relay   
Pump 2 RelayGPIO 27Active-Low Relay   
Pump 3 RelayGPIO 26Active-Low Relay   
⚠️ Note: The relay controls are configured as Active-Low (LOW turns the device ON, HIGH turns it OFF).  
💾 Firebase Database StructureThe sketch interacts with your Firebase Realtime Database using the following JSON structure:  
JSON{
  "greenhouse": {
    "current": {
      "temperature": 24.5,
      "humidity": 60.2,
      "soil1": 45,
      "soil2": 72,
      "soil3": 15
    },
    "fan_status": "OFF",
    "pump1": "OFF",
    "pump2": "OFF",
    "pump3": "OFF"
  }
}
/current: Sensor data payload pushed by the ESP32 every 5 seconds.  
Control Nodes (fan_status, pump1, etc.): Write "ON" or "OFF" to these paths via a web dashboard, mobile app, or script to instantly trigger the hardware.  
⚙️ Setup & Installation1. 
PrerequisitesEnsure you have the following libraries installed in your Arduino IDE:
WiFi.h (Built-in for ESP32)   
Firebase_ESP_Client   DHT sensor library (by Adafruit)   
2. ConfigurationOpen sketch_may17a.ino and update the following definitions with your network and database credentials:  C++// Wi-Fi Credentials
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Firebase Project Settings
#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"
3. CalibrationIf your soil moisture percentage reads incorrectly, 
adjust the ADC raw limits based on your sensor type:  C++#define WET_VALUE 1500  // Raw ADC value in water
#define DRY_VALUE 4095  // Raw ADC value in open air
4. UploadSelect your ESP32 Dev Module board in Arduino IDE.Connect your ESP32 to your computer via USB.Choose the correct COM Port and click Upload.
Open the Serial Monitor (115200 baud) to monitor live readings and system connectivity.  
📝 LicenseThis project is open-source and available under the MIT License.
