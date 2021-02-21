#define RXD2 33
#define TXD2 4

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Primus-55a0";
const char* password = "3c90666f55a0";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String LEDState = "off";
char c;

// Define timeout conditions for the client
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

//Uncomment this section of code if you want to set a fixed IP Address
IPAddress local_IP(192, 168, 1, 18);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);



void setup() {
  //Begin two Serial streams. One for the serial monitor and the other for serial communication with Arduino.
  Serial.begin(115200);
  Serial2.begin(9600,SERIAL_8N1, RXD2, TXD2); 
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //If you have a fixed IP Address, then you must include this line
  WiFi.config(local_IP, gateway, subnet); 

  //Attempt to connect to the WiFi Network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the ESP32 Server
  server.begin();
}

void loop(){

  //Make the Server listen for Clients.
  WiFiClient client = server.available();

  //Indicate if a client was connected.
  if (client) {                             
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          

    
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /led/on") >= 0) {
              Serial.println("LED Is on");
              LEDState = "on";
            } else if (header.indexOf("GET /led/off") >= 0) {
              Serial.println("LED is off");
              LEDState = "off";
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>LED State " + LEDState + "</p>");
            // If the output26State is off, it displays the ON button       
            if (LEDState=="off") {
              client.println("<p><a href=\"/led/on\"><button class=\"button\">ON</button></a></p>");
              c = 'l';
              Serial2.write(c);
            } else {
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">OFF</button></a></p>");
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
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
