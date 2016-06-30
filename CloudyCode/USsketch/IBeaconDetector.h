/*
 * Detect an iBeacon using a RedBear BLE Mini with this firmware:
 *    https://github.com/RedBearLab/BLE_HCI/blob/master/cc2540_hci_fw/HCI_UART_57600bps_20130502.bin.zip
 * 
 * Adapted from the code by Krishnaraj Varma found here:
 *    https://www.hackster.io/krvarma/detect-ibeacon-using-spark-core-and-ble-mini-e77542
 * 
 * Author: Giacomo Del Rio
*/

#ifndef IBEACON_DETECTOR_H_
#define IBEACON_DETECTOR_H_

#ifdef DEBUG_BUILD
#  define DEBUG(x) Serial.println(x)
#else
#  define DEBUG(x) do {} while (0)
#endif

#include <math.h>
#include <HardwareSerial.h>
#include "Arduino.h"

#define GAP_PROFILE_CENTRAL           0x08
#define KEYLEN                        16
#define DEVICE_INITIALIZED 0x600
#define DEVICE_DISCOVERY_DONE 0x601
#define DEVICE_INFORMATION 0x60D
#define BUILD_UINT16(loByte, hiByte) ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

struct BTDevice{
  public:
    uint8_t uuid[16];
    uint16_t major;
    uint16_t minor;
    int power;
    double distance;
};

class IBeaconDetector {
  public:
    IBeaconDetector(HardwareSerial& bleSerial);
    void initialize();
    bool discoverDevice(BTDevice& discoveredDevice);
  
  private:
    HardwareSerial& bleSerial;  // Serial attached to BLE mini
    uint8_t btBuffer[64];       // Bluetooth communications buffer

    static uint8_t gapCentralRoleTaskId;
    static uint8_t  gapCentralRoleIRK[KEYLEN];
    static uint8_t  gapCentralRoleSRK[KEYLEN];
    static uint32_t gapCentralRoleSignCounter;
    static uint8_t  gapCentralRoleMaxScanRes;

    void GAP_DeviceInit(uint8_t taskID, uint8_t profileRole, uint8_t maxScanResponses, 
                        uint8_t *pIRK, uint8_t *pSRK, uint32_t *pSignCounter);
    void GAP_DeviceDiscoveryRequest();
    bool processEvent(BTDevice& discoveredDevice);
};

#endif  //IBEACON_DETECTOR_H_
