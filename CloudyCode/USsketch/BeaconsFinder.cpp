/* 
 *  Implementation of BeaconsFinder.h
*/

#include "BeaconsFinder.h"

BeaconsFinder::BeaconsFinder(IBeaconDetector& detector, double range, unsigned int retainMs)
  : detector(detector) {
  this->range = range;
  this->retainMs = retainMs;
}

void BeaconsFinder::scan(){
  BTDevice bt;
  
  if( detector.discoverDevice(bt) ){
    if(bt.distance <= range){
      auto b = inRangeBeacons.begin();
      auto t = detectionTimes.begin();
      for( ; b != inRangeBeacons.end(); ++b, ++t){
        if(equal(b->uuid, b->uuid + 16, bt.uuid)){
          *t = millis();
          return;
        }
      }
      inRangeBeacons.push_back( UUID(bt.uuid) );
      detectionTimes.push_back( millis() );
    }
  }
}

const vector<UUID>& BeaconsFinder::getBeaconsInRange(){
  unsigned long actualTime = millis();
  auto b = inRangeBeacons.begin();
  auto t = detectionTimes.begin();
  for( ; b != inRangeBeacons.end(); ){
    if(actualTime - *t > retainMs){
      inRangeBeacons.erase(b);
      detectionTimes.erase(t);
    }else{
      ++b;
      ++t;
    }
  }
  return inRangeBeacons;
}
