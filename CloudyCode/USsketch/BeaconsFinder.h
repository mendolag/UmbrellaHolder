/*
 * Maintain a list of Beacons that are located within a given range. 
 * Every detected beacon stays in the list for a given amount
 * of time, then it will be removed if not detected anymore.
 * 
 * Author: Giacomo Del Rio
*/

#ifndef BEACONS_FINDER_H_
#define BEACONS_FINDER_H_

#ifdef DEBUG_BUILD
#  define DEBUG(x) Serial.println(x)
#else
#  define DEBUG(x) do {} while (0)
#endif

#include <vector>
#include "USUtils.h"
#include "IBeaconDetector.h"

using namespace std;

class BeaconsFinder {
  public:
    BeaconsFinder(IBeaconDetector& detector, double range, unsigned int retainMs);
    void scan();
    const vector<UUID>& getBeaconsInRange();
  
  private:
    IBeaconDetector& detector;
    double range;
    unsigned int retainMs;
    vector<UUID> inRangeBeacons;
    vector<unsigned long> detectionTimes;
};

#endif  //BEACONS_FINDER_H_
