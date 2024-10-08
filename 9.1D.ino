#include <FreeRTOS_SAMD21.h>
#include <DHT.h>
#include <WiFiNINA.h>
#include "ADC_DEAKIN.h"      // Include your ADC class
#include "TIMER_DEAKIN.h"    // Include your Timer class
#include "GPIO_DEAKIN.h"     // Include your GPIO class

// Your WiFi credentials
const char* ssid = "NETGEAR21";        // WiFi SSID
const char* password = "69699696"; // WiFi Password

// Updated Pin Definitions
#define FSR_PIN A3          // FSR (Force Sensitive Resistor) connected to A3
#define DHT_PIN 4           // DHT22 sensor connected to D4
#define DHT_TYPE DHT22
#define BUZZER_PIN 5        // Buzzer connected to D5
#define BUTTON_PIN 2        // Pin connected to the button

// Variables for Alarm Conditions
int tapCount = 0;
unsigned long lastTapTime = 0;
unsigned long lastButtonPressTime = 0;  // Last recorded button press time
unsigned long lastDebounceTime = 0;     // Last debounce check time
unsigned long debounceDelay = 100;      // Debounce time of 100ms
unsigned long tapTimeout = 7000;        // 7 seconds to register 5 taps
unsigned long nurseCallDelay = 5000;    // Delay of 5 seconds after nurse is called
bool alarmActivated = false;             // Alarm status
bool buttonPressed = false;
bool buttonState = HIGH;                 // Default button state due to internal pull-up
bool lastButtonState = HIGH;             // Previous state of the button
bool nurseCalled = false;                 // Indicator for nurse call status
unsigned long nurseCallTime = 0;         // Time when nurse call was made
bool resetAllowed = false;                // Tracks if alarm reset is permitted after delay

// Create Objects
ADC_DEAKIN pressureSensor(FSR_PIN);      // Pressure sensor
TIMER_DEAKIN temperatureTimer;            // Timer for temperature
TIMER_DEAKIN pressureTimer;               // Timer for pressure
GPIO_DEAKIN buzzer(BUZZER_PIN);          // Buzzer

WiFiServer server(80); // Create a server that listens on port 80
DHT dht(DHT_PIN, DHT_TYPE); // DHT sensor object

void setup() {
    Serial.begin(9600);
    while (!Serial);

    // Initialize DHT sensor and ADC
    dht.begin();
    buzzer.setLow(); // Initialize buzzer state

    // Initialize button pin with internal pull-up resistor
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP()); // Print IP address

    server.begin(); // Start the server

    Serial.println("Initialization complete. The system is ready.");

    // Create FreeRTOS tasks
    xTaskCreate(measureTemperature, "Measure Temperature", 1000, NULL, 1, NULL);
    xTaskCreate(measurePressure, "Measure Pressure", 1000, NULL, 1, NULL);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
}

void loop() {
    handleClient(); // Handle client requests
}

// Handle client connections and serve the web page
void handleClient() {
    WiFiClient client = server.available(); // Check for incoming clients
    if (client) {
        String currentLine = ""; // String to hold incoming data
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    // New line means the end of the request
                    if (currentLine.length() == 0) {
                        // Check for reset request
                        if (currentLine.startsWith("GET /reset")) {
                            // Reset the alarm state
                            resetAlarm();
                            Serial.println("Alarm has been reset by the nurse.");
                        }

                        // Send a response
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();
                        client.print("<html><head><title>Bed Monitoring System</title>");
                        client.print("<style>");
                        client.print("body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 20px; }");
                        client.print("h1 { color: #333; }");
                        client.print("p { font-size: 18px; margin: 10px 0; }");
                        client.print("a { text-decoration: none; color: #007BFF; }");
                        client.print("a:hover { text-decoration: underline; }");
                        client.print(".container { background: white; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }");
                        client.print("</style></head><body>");
                        client.print("<div class='container'>");
                        client.print("<h1>Bed Monitoring System</h1>");
                        client.print("<p>Temperature: ");
                        client.print(dht.readTemperature());
                        client.print(" °C</p>");
                        client.print("<p>Humidity: ");
                        client.print(dht.readHumidity());
                        client.print("%</p>");
                        client.print("<p>Occupancy: ");
                        client.print(pressureSensor.analogRead());
                        client.print("</p>");
                        client.print("<p><a href=\"/reset\">Reset Alarm</a></p>");
                        client.print("</div></body></html>");
                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        client.stop(); // Close the connection
    }
}

// Function to reset the alarm state
void resetAlarm() {
    alarmActivated = false; 
    buzzer.setLow();
    tapCount = -1;  // Reset tap count to -1 when the alarm is reset
}

// Function to measure body temperature and manage the alarm system
void measureTemperature(void *pvParameters) {
    (void) pvParameters;
    temperatureTimer.start(300000); // Interval set to 5 minutes
    while (1) {
        if (temperatureTimer.hasElapsed()) {
            float temperature = dht.readTemperature();  // Retrieve temperature from DHT sensor
            float humidity = random(30, 100);  // Simulate humidity between 30% and 100%

            if (!isnan(temperature)) {
                Serial.print("Current Temperature: ");
                Serial.print(temperature);
                Serial.print(" °C | Humidity: ");
                Serial.print(humidity);
                Serial.println("%");

                if (temperature >= 38.0 && temperature < 39.0) {
                    Serial.println("Alert: Mild fever detected.");
                } else if (temperature >= 39.0 || temperature < 35.0) {
                    Serial.println("ALERT: Temperature outside safe range! Activating buzzer.");
                    buzzer.setHigh();
                    vTaskDelay(1000 / portTICK_PERIOD_MS); // Buzzer sounds for 1 second
                    buzzer.setLow();
                } else {
                    Serial.println("Temperature remains stable and within normal limits.");
                }
            } else {
                Serial.println("Error: Unable to read temperature.");
            }
            temperatureTimer.reset();
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);  // Allow FreeRTOS to manage tasks
    }
}

// Function to check bed occupancy and manage tap alarm for HELP signal
void measurePressure(void *pvParameters) {
    (void) pvParameters;
    pressureTimer.start(60000);  // Interval set to 1 minute
    while (1) {
        if (pressureTimer.hasElapsed()) {
            int pressureValue = pressureSensor.analogRead();  // Read ADC value from pressure sensor
            Serial.print("Pressure Sensor Reading: ");
            Serial.println(pressureValue);

            if (pressureValue < 100) {
                Serial.println("Status: Bed is unoccupied.");
            } else {
                Serial.println("Status: Bed is occupied.");
            }
            pressureTimer.reset();
        }

        // Manage nurse call delay: Ignore button presses during nurseCallDelay
        if (nurseCalled && (millis() - nurseCallTime) < nurseCallDelay) {
            vTaskDelay(100 / portTICK_PERIOD_MS);  // Yield control to FreeRTOS
            continue;
        } else if (nurseCalled && (millis() - nurseCallTime) >= nurseCallDelay && !resetAllowed) {
            resetAllowed = true;  // Permit alarm reset after delay
            Serial.println("Reset is now permitted following nurse call delay.");
        }

        // Button debouncing and handling
        int reading = digitalRead(BUTTON_PIN);

        // If the button state changes
        if (reading != lastButtonState) {
            lastDebounceTime = millis();  // Reset debounce timer
        }

        // Only consider the press if it has been stable for debounceDelay
        if ((millis() - lastDebounceTime) > debounceDelay) {
            // If button is pressed (LOW) and wasn't pressed previously
            if (reading == LOW && !buttonPressed) {
                if (millis() - lastButtonPressTime > debounceDelay) {
                    lastButtonPressTime = millis();  // Update last press time
                    // If this is the first tap, start the 2-second timeout
                    if (tapCount == 0) {
                        lastTapTime = millis();  // Record time of first tap
                    }
                    tapCount++;
                    Serial.print("Tap detected. Total Count: ");
                    Serial.println(tapCount);

                    if (tapCount == 5) {
                        Serial.println("ALERT: Nurse assistance is required!");
                        alarmActivated = true;  // Changed from alarmTriggered for clarity
                        buzzer.setHigh();  // Activate buzzer
                        nurseCalled = true;  // Begin nurse call delay
                        nurseCallTime = millis();  // Record time of nurse call
                        resetAllowed = false;  // Prevent immediate reset
                    }
                    buttonPressed = true;  // Register the button press
                }
            } else if (reading == HIGH) {
                // Button released, allow for the next press
                buttonPressed = false;
            }
        }

        // Save the reading for the next iteration
        lastButtonState = reading;

        // Reset tap count if 2 seconds have passed without reaching 5 taps
        if (tapCount > 0 && (millis() - lastTapTime) > tapTimeout && tapCount < 5) {
            Serial.println("Timeout: Tap count has been reset.");
            tapCount = 0;  // Reset tap count
        }

        // If alarm is triggered and reset is allowed, reset when button is pressed again
        if (alarmActivated && resetAllowed && digitalRead(BUTTON_PIN) == LOW) {
            Serial.println("Nurse has arrived. ALARM has been reset.");
            resetAlarm(); // Call the reset function
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);  // Yield control to FreeRTOS
    }
}
