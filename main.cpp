#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266HTTPClient.h>

char TempString[228]; //19 (clock pluses) * 6 (addresses) * 2 (twice)
int BitCount = 0;
unsigned long TimeStamp;

void IRAM_ATTR blink(void)
{
  // Read SDA as char
  TempString[BitCount] = digitalRead(4) + '0';
  // Next bit
  BitCount++;
}

char GetDigit(int a, int z)
{
  // Serial.println(String(TempString).substring(0, 7)); //0x36
  // Serial.println(String(TempString).substring(19, 26)); //0x35
  // Serial.println(String(TempString).substring(38, 45)); //0x34
  // Serial.println(String(TempString).substring(57, 64)); //0x37
  // Serial.println(String(TempString).substring(76, 83)); //0x35
  // Serial.println(String(TempString).substring(95, 102)); //0x34
  // Serial.println(String(TempString).substring(114, 121)); //0x36*
  // Serial.println(String(TempString).substring(133, 140)); //0x35
  // Serial.println(String(TempString).substring(152, 159)); //0x34
  // Serial.println(String(TempString).substring(171, 178)); //0x37
  // Serial.println(String(TempString).substring(190, 197)); //0x35*
  // Serial.println(String(TempString).substring(209, 216)); //0x34*
  //
  //            -- 4 --
  //          /        /
  //         3        5
  //        /        /
  //        -- 2 --
  //      /        /
  //     1        6
  //    /        /
  //    -- 7 --
  //
  //  e.g.
  //  1234567
  //  0101111 = 3

  switch (String(TempString).substring(a, z).toInt())
  {
  case (int)1011111: //0 1011111
    return '0';
  case (int)110: //1 0000110
    return '1';
  case (int)1101101: //2 1101101
    return '2';
  case (int)101111: //3 0101111
    return '3';
  case (int)110110: //4 0110110
    return '4';
  case (int)111011: //5 0111011
    return '5';
  case (int)1111011: //6 1111011
    return '6';
  case (int)1110: //7 0001110
    return '7';
  case (int)1111111: //8 1111111
    return '8';
  case (int)111111: //9 0111111
    return '9';
  }
  return 'X';
}

void setup()
{
  // SDA
  pinMode(4, INPUT);
  // SCL
  pinMode(5, INPUT);
  // Keep LED off until connected to Wi-Fi
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  //wm.resetSettings();//reset settings - wipe credentials for testing
  wm.autoConnect("WeMos");
  // Connected to Wi-Fi
  digitalWrite(LED_BUILTIN, 0);
  // Detect SCL
  attachInterrupt(digitalPinToInterrupt(5), blink, RISING);
}

void loop()
{
  // Look for first bit
  if (BitCount > 0)
  {
    TimeStamp = millis();
    // Wait for all bits to come in
    while (millis() - TimeStamp < 200);
    BitCount = 0;
    char Temperature[4];
    Temperature[0] = GetDigit(219, 226);
    Temperature[1] = GetDigit(200, 207);
    Temperature[2] = '.';
    Temperature[3] = GetDigit(124, 131);
    if ((WiFi.status() == WL_CONNECTED))
    {
      // Turn LED off to show busy sending
      digitalWrite(LED_BUILTIN, 1);
      std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
      client->setInsecure();
      HTTPClient https;
      if (https.begin(*client, "https://script.google.com/macros/s/AKfyfcyi8ftJan2J72b4sXGsD2udIVpXMyfGINi9s2LHhVIgHwpIGIxcUddBKR5fBEm/exec"))
      {
        https.POST("{\"Temperature\":\"" + String(Temperature).substring(0, 4) + "\",\"ESPChipId\":\"" + ESP.getChipId() + "\"}");
        https.end();
      }
      digitalWrite(LED_BUILTIN, 0);
    } else digitalWrite(LED_BUILTIN, 1);
  }
}
