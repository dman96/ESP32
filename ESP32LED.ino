/*This code is an example that creates a web server, using an ESP32, that allows you to wirelessly operate an LED connected to an Arduino. The ESP32 features three serial connections and WiFi module. The WiFi module can rely on existing network credentials to generate a web server. Alternatively, one may create a unique access point and turn the ESP32 into a network host and a server. When using the ESP32 on the Arduino IDE, ensure that the ESP32 board package is included. This can be done by navigating to preferences and pasting the following URL:

https://dl.espressif.com/dl/package_esp32_index.json,http://arduino.esp8266.com/stable/package_esp8266com_index.json
*/


/*Define the serial pins that you will use to communicate with an Arduino. For an ESP32, generally pins 16 (RX) and 17 (TX) are used for serial communication with other devices. If you are using a non-standard ESP32, then the serial pins would need to be changed as needed.
*/

#define RX2 16
#define TX2 17

// Load Wi-Fi library
#include <WiFi.h>

// Include network credentials
const char* ssid = "*****";
const char* password = "*****";

// Set web server port number to 80 (HTTP)
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliary variables to store the LED state
String LEDState = "off";
char c;

// Define timeout conditions for the client
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

/*Uncomment this section of code if you want to set a fixed IP Address. By default, the WiFi library will assign an arbitrary vacant IP Address for clients to connect to.
IPAddress local_IP(192, 168, 1, 18);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
*/

/* Uncomment this section, if you would like to create a unique access point for the ESP32 using your own desired network ID and password.
WiFi.softAP(ssid, password);
*/


void setup() {
  //Begin two Serial streams. One for the serial monitor and the other for serial communication with Arduino.
  Serial.begin(115200);
  Serial2.begin(9600,SERIAL_8N1, RXD2, TXD2); 
  
  // Display a message indicating an imminent connection to the network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /*If you have a fixed IP Address, then you must include this line
  WiFi.config(local_IP, gateway, subnet); 
  */
  
  //Attempt to connect to the WiFi Network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address of Server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the ESP32 Server
  server.begin();
}

void loop(){

  //Make the server listen for clients.
  WiFiClient client = server.available();

  
  if (client) {
    //Indicate if a client was connected.
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          
    
    //Make a String to hold information from the Client
    String currentLine = "";                
    
    //Run this loop while a client is connected
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      
      //If the client is connected, then read the URL from the client and store it into the header string.
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
       
        //Once the end of the URL is read (A new-line character appears), display a standard HTTP success report 
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Check the header and identify whether the LED is on or off
            if (header.indexOf("GET /led/on") >= 0) {
              Serial.println("LED Is on");
              LEDState = "on";
            } else if (header.indexOf("GET /led/off") >= 0) {
              Serial.println("LED is off");
              LEDState = "off";
            }
            
            // Map the foundations of the HTML Webpage for display
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state of the LED  
            client.println("<p>LED State " + LEDState + "</p>");
            
            // If the LED is off, the webpage displays the ON button       
            if (LEDState=="off") {
              
              //Show the "ON" button if the LED is off
              client.println("<p><a href=\"/led/on\"><button class=\"button\">ON</button></a></p>");
              
              //Send a character to the serial port for the Arduino to notify the Arduino that the LED should be off
              c = 'l';
              Serial2.write(c);
            } 
            
            //When the LED is on, the webpage displays the OFF button
            else {
              
              //Show the "OFF" button when the LED is on
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">OFF</button></a></p>");
              
              //Send a character to the serial port for the Arduino to notify the Arduino that the LED should be on
              c = 'h';
              Serial2.write(c);
            } 
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    
    // Clear all variables and close the connection if the client disconnects
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
