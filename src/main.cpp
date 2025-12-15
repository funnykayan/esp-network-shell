#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "TMNL-754055"; // ur wifi name (im adding ethernet soon)
const char* pass = "LFFL7SV6W7959LUP"; // ur wifi password

WiFiServer shellServer(2323); // port 2323 for the shell
WiFiClient shellClient;

String inputBuffer = ""; 
String USER = "admin"; // this is the user, gets changed with usercr command

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD I2C address

// defining the looks
#define C_RESET  "\033[0m"
#define C_GREEN  "\033[32m"
#define C_CYAN   "\033[36m"
#define C_YELLOW "\033[33m"
#define C_RED    "\033[31m"
#define C_BOLD   "\033[1m"

// LED state
int ledPin = -1;

// shell otuput funny stuff
void shellPrint(const String &s) {
  Serial.print(s);
  if (shellClient && shellClient.connected()) shellClient.print(s);
}

void shellPrintln(const String &s = "") {
  shellPrint(s + "\r\n");
}

void prompt() {
  shellPrint(C_GREEN + USER + "@esp32" C_CYAN "> " C_RESET); // defining the prompt
}

// ---------- COMMANDS ----------
void showHelp() { //le list of le commands
  shellPrintln(C_YELLOW "Commands:" C_RESET);
  shellPrintln(" help");
  shellPrintln(" license");
  shellPrintln(" echo <text>");
  shellPrintln(" time");
  shellPrintln(" clear");
  shellPrintln(" reboot");
  shellPrintln(" ledpinselect <pin>");
  shellPrintln(" ledpin <high|low>");
  shellPrintln(" usercr <user>");
}

void showInfo() { // boring info
  shellPrintln(C_BOLD "ESP32 Network Shell" C_RESET);
  shellPrintln(C_BOLD "Developed on 15/12/2025" C_RESET);
  shellPrintln(C_BOLD "By funnykayan12345" C_RESET);
}

void showESPart() { // le cool esp32 art
  shellClient.println(C_YELLOW "####  ####  ####    ####  ####" C_RESET);
  shellClient.println(C_GREEN  "#     #     #  #       #     #" C_RESET);
  shellClient.println(C_CYAN   "####  ####  ####    ####  ####" C_RESET);
  shellClient.println(C_RED    "#        #  #          #  #   " C_RESET);
  shellClient.println(C_BOLD   "####  ####  #       ####  ####" C_RESET);
  shellClient.println("ESP32 Shell-Over-Network (SON) Software");
  shellClient.println("");
}

void showLicense() { // license because yes
  shellPrintln(C_YELLOW "License:" C_RESET);
    shellPrintln("MIT License");

    shellPrintln("Copyright (c) 2025 funnykayan12345");

    shellPrintln("Permission is hereby granted, free of charge, to any person obtaining a copy");
    shellPrintln("of this software and associated documentation files the \"Software\", to deal");
    shellPrintln("in the Software without restriction, including without limitation the rights");
    shellPrintln("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
    shellPrintln("copies of the Software, and to permit persons to whom the Software is");
    shellPrintln("furnished to do so, subject to the following conditions:");

    shellPrintln("The above copyright notice and this permission notice shall be included in all");
    shellPrintln("copies or substantial portions of the Software.");

    shellPrintln("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
    shellPrintln("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
    shellPrintln("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE");
    shellPrintln("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
    shellPrintln("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
    shellPrintln("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
    shellPrintln("SOFTWARE.");
}

void clearScreen() { // let there be clear terminal
  shellPrint("\033[2J\033[H"); 
}

void handleCommand(String cmd) {
  cmd.trim();

  if (cmd == "help") {
    showHelp();
  }

  else if (cmd == "license") {
    showLicense();
  }

  else if (cmd == "clear") {
    clearScreen();
  }

  else if (cmd == "info") {
    showInfo();
  }

  else if (cmd.startsWith("echo ")) {
    shellPrintln(cmd.substring(5));
  }
  
  else if (cmd.startsWith("usercr ")) {
    String newUser = cmd.substring(7);
    newUser.trim();
    if (newUser.length() == 0) {
      shellPrintln(C_RED "Username cannot be empty" C_RESET);
    } else {
      USER = newUser;
      shellPrintln(C_GREEN "Username changed to " + USER + C_RESET);
    }
  }

  else if (cmd == "time") {
    shellPrintln(String(millis()));
  }

  else if (cmd == "reboot") {
    shellPrintln(C_RED "Rebooting..." C_RESET);
    delay(500);
    shellPrintln(C_RED "Restart success, please restart the WiFi shell." C_RESET);
    ESP.restart();
  }

  // ---- LED PIN SELECT ----
  else if (cmd.startsWith("ledpinselect ")) {
    int pin = cmd.substring(13).toInt();

    if (pin < 0) {
      shellPrintln(C_RED "Invalid pin" C_RESET);
      return;
    }

    ledPin = pin;
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    shellPrintln(C_GREEN "LED pin set to GPIO " + String(ledPin) + C_RESET);
  }

  // ---- LED CONTROL ----
  else if (cmd.startsWith("ledpin ")) {
    if (ledPin < 0) {
      shellPrintln(C_RED "No LED pin selected" C_RESET);
      return;
    }

    String state = cmd.substring(7);
    state.trim();

    if (state == "high") {
      digitalWrite(ledPin, HIGH);
      shellPrintln("GPIO " + String(ledPin) + " -> HIGH");
    }
    else if (state == "low") {
      digitalWrite(ledPin, LOW);
      shellPrintln("GPIO " + String(ledPin) + " -> LOW");
    }
    else {
      shellPrintln(C_RED "Usage: ledpin high|low" C_RESET);
    }
  }

  else {
    shellPrintln(C_RED "Unknown command" C_RESET);
  }
}

// input stuff
void processInput(char c) {
  if (c == '\r' || c == '\n') {
    if (inputBuffer.length()) {
      shellPrintln();
      handleCommand(inputBuffer);
      inputBuffer = "";
      prompt();
    }
  } else {
    inputBuffer += c;
    shellPrint(String(c));
  }
}

// wifi stuff
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connecting...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("IP:" + WiFi.localIP().toString());
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected Port 2323");
  } else {
    Serial.println("\nWiFi failed, retrying...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    delay(5000);
    connectWiFi();
  }
}


void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  delay(1000);
  clearScreen();

  Serial.println(C_BOLD "ESP32 Shell Booting" C_RESET);
  connectWiFi();

  shellServer.begin();
  shellServer.setNoDelay(true);

  shellPrintln("Shell ready");
  prompt();
}


void loop() {
  if (!shellClient || !shellClient.connected()) {
    WiFiClient newClient = shellServer.available();
    if (newClient) {
      if (shellClient) shellClient.stop();
      shellClient = newClient;
      shellClient.println("\033[2J\033[H");
      showESPart();
      shellClient.println("ESP32 Network Shell");
      shellClient.println("Type 'help'");
      prompt();
    }
  }

  while (Serial.available()) processInput(Serial.read());
  if (shellClient && shellClient.connected()) {
    while (shellClient.available()) processInput(shellClient.read());
  }

  if (WiFi.status() != WL_CONNECTED) connectWiFi();
}
