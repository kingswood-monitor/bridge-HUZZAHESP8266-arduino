#include <Arduino.h>

#include "utils.h"

namespace util
{
void printBanner(const char *firmware_filename, const char *firmware_version, const char *deviceID)
{
    Serial.println();
    Serial.println("======================================================================");
    Serial.print("# Kingswood LoRa / WiFi Bridge v.");

    Serial.println(firmware_version);
    Serial.println("----------------------------------------------------------------------");
    Serial.print("Firmware filename   : ");
    Serial.println(firmware_filename);
    Serial.print("Sensor ID           : ");
    Serial.println(deviceID);
    Serial.println("======================================================================");
    Serial.println();
}

char *deviceID(const char *type)
{
    char buf[7];
    sprintf(buf, "%lX", ESP.getChipId());

    char *deviceID;
    deviceID = (char *)malloc(strlen(type) + 8);

    strcpy(deviceID, type);
    strcat(deviceID, "-");
    strcat(deviceID, buf);

    return deviceID;
}

char *round_to_digits_s(float value, int digits)
{
    static char outstr[15];
    return dtostrf(value, 7, digits, outstr);
    return outstr;
}
} // namespace util
