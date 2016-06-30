/* 
 *  Implementation of IBeaconDetector.h
*/

#include "IBeaconDetector.h"

uint8_t IBeaconDetector::gapCentralRoleTaskId = 0;
uint8_t  IBeaconDetector::gapCentralRoleIRK[KEYLEN] = {0};
uint8_t  IBeaconDetector::gapCentralRoleSRK[KEYLEN] = {0};
uint32_t IBeaconDetector::gapCentralRoleSignCounter = 1;
uint8_t  IBeaconDetector::gapCentralRoleMaxScanRes = 5;

IBeaconDetector::IBeaconDetector(HardwareSerial& bleSerial)
  : bleSerial(bleSerial)
{
}

void IBeaconDetector::initialize(){
  GAP_DeviceInit(gapCentralRoleTaskId, GAP_PROFILE_CENTRAL, 
                 gapCentralRoleMaxScanRes, gapCentralRoleIRK, 
                 gapCentralRoleSRK, &gapCentralRoleSignCounter);
  DEBUG("BT Initialized");
}

bool IBeaconDetector::discoverDevice(BTDevice& discoveredDevice){
  if(bleSerial.available()){
    return processEvent(discoveredDevice);
  }
}

bool IBeaconDetector::processEvent(BTDevice& discoveredDevice){
  uint8_t type, event_code, data_len, status1;
  uint16_t event;

  DEBUG("BT Start reading");
  
  type = bleSerial.read();
  delay(100);  // Really needed?
  event_code = bleSerial.read();
  data_len = bleSerial.read();

  //DEBUG("BT Data dump");
  for( int i = 0; i < data_len; i++ ){
    btBuffer[i] = bleSerial.read();
    //DEBUG(btBuffer[i]);
  }

  event = BUILD_UINT16(btBuffer[0], btBuffer[1]);
  status1 = btBuffer[2];
  
  switch(event){
    case DEVICE_INITIALIZED:
      GAP_DeviceDiscoveryRequest();
      DEBUG("BT Started discovery");
      return false;
        
    case DEVICE_DISCOVERY_DONE:
      GAP_DeviceDiscoveryRequest();
      DEBUG("BT Completed discovery");
      return false;
      
    case DEVICE_INFORMATION:{
      DEBUG("BT Get device info");
      
      // Get RSSI and Measured Power
      int rssi = btBuffer[11];
      int txpower = btBuffer[42];
            
      // Calculate Distance. Based on the algorithm from:
      //   http://stackoverflow.com/questions/20416218/understanding-ibeacon-distancing
      double distance = 0.0;
      double ratio = (256 - rssi) * 1.0 / (256 - txpower);
      if(ratio < 1.0)
        distance = pow(ratio, 10);
      else
        distance = (0.89976)*pow(ratio,7.7095) + 0.111;
                
      // Copy infos on output parameter
      memcpy(discoveredDevice.uuid, &(btBuffer[22]), 16);
      discoveredDevice.major = BUILD_UINT16(btBuffer[39], btBuffer[38]);
      discoveredDevice.minor = BUILD_UINT16(btBuffer[41], btBuffer[40]);
      discoveredDevice.power = btBuffer[42];
      discoveredDevice.distance = distance;

      return true;
    }
    default:
      return false;
  }

  return false;
}

void IBeaconDetector::GAP_DeviceInit(uint8_t taskID, uint8_t profileRole, uint8_t maxScanResponses, 
                                     uint8_t *pIRK, uint8_t *pSRK, uint32_t *pSignCounter){
    uint8_t len = 0;
    
    btBuffer[len++] = 0x01;                  // -Type    : 0x01 (Command)
    btBuffer[len++] = 0x00;                  // -Opcode  : 0xFE00 (GAP_DeviceInit)
    btBuffer[len++] = 0xFE;
  
    btBuffer[len++] = 0x26;                  // -Data Length
    btBuffer[len++] = profileRole;           //  Profile Role
    btBuffer[len++] = maxScanResponses;      //  MaxScanRsps
    memcpy(&btBuffer[len], pIRK, 16);        //  IRK
    len += 16;
    memcpy(&btBuffer[len], pSRK, 16);        //  SRK
    len += 16;
    memcpy(&btBuffer[len], pSignCounter, 4); //  SignCounter
    len += 4;

    bleSerial.write(btBuffer, len);
}

void IBeaconDetector::GAP_DeviceDiscoveryRequest(){
  uint8_t len = 0;
  
  btBuffer[len++] = 0x01;                 // -Type    : 0x01 (Command)
  btBuffer[len++] = 0x04;                 // -Opcode  : 0xFE04 (GAP_DeviceDiscoveryRequest)
  btBuffer[len++] = 0xFE;
      
  btBuffer[len++] = 0x03;                 // -Data Length
  btBuffer[len++] = 0x03;                 //  Mode
  btBuffer[len++] = 0x00;                 //  ActiveScan (set to 0x01 to get also active devices)
  btBuffer[len++] = 0x00;                 //  WhiteList

  bleSerial.write(btBuffer, len);
}

