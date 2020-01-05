#pragma once

namespace util
{
/** print firmware details to Serial */
void printBanner(const char *firmware_filename, const char *firmware_version, const char *deviceID);

/** Construct the device ID from the chip ID e.g. "HUZZAHESP8266-2DC832". */
char *deviceID(const char *type);

/** Computes string of float rounded to 'digits' places.
    We need this for Blynk because iOS represents any rounded float as #.### */
char *round_to_digits_s(float value, int digits);
} // namespace util
