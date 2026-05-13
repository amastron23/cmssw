#ifndef EventFilter_Phase2TrackerRawToDigi_ChannelsOffset_H
#define EventFilter_Phase2TrackerRawToDigi_ChannelsOffset_H

// Class to store the payload offsets of the various channels in the FedRawData collection
// as from the current outer tracker data format

#include "EventFilter/Phase2TrackerRawToDigi/interface/Phase2DAQFormatSpecification.h"
#include "EventFilter/Phase2TrackerRawToDigi/interface/Phase2TrackerSpecifications.h"

using namespace Phase2TrackerSpecifications;
using namespace Phase2DAQFormatSpecification;

class ChannelsOffset {
public:
  std::vector<uint32_t> values_;
  std::vector<uint16_t> offsetMap_{std::vector<uint16_t>(CICs_PER_SLINK, 0)};

  void setValue(std::vector<uint32_t>& newValues) {
    values_ = newValues;
    fillOffsetMap();
  }

  void printValues() const {
    for (size_t i = 0; i < values_.size(); ++i) {
      std::cout << "ChannelsOffset[" << i << "]: " << values_[i] << "   " << std::bitset<N_BITS_PER_WORD>(values_[i])
                << std::endl;
    }
  }
  void printValue(size_t i) const {
    std::cout << "ChannelsOffset[" << i << "]: " << values_[i] << "   " << std::bitset<N_BITS_PER_WORD>(values_[i])
              << std::endl;
  }

  // this is for the 64b version of the offset
  void fillOffsetMap64() {
    // channel 0 offset is always 0 
    offsetMap_[0] = static_cast<uint16_t>(0);  
    for (size_t i = 1; i < CICs_PER_SLINK ; ++i) {
      offsetMap_[i] = static_cast<uint16_t>((values_[i-1]) & 0xFF) - 1 ;
    }
  }

  // this was for the 32b version of the offset
void fillOffsetMap() {
    // // Print the raw values first
    // for (size_t i = 0; i < values_.size(); i++) {
    //     std::cout << std::hex << std::setw(4) << std::setfill('0') << i 
    //               << ": " << std::setw(8) << std::setfill('0') << values_[i] << std::endl;
    // }
    
    // offsetMap_[0] is always 0
    offsetMap_[0] = 0;
    
    // Extract offsets from the 32-bit words
    // Each 32-bit word contains two 16-bit offsets (lower and upper)
    int offsetIndex = 1;  // start from index 1 since index 0 is already set
    
    for (size_t i = 0; i < values_.size(); i++) {
        // Extract lower 16 bits (bits 0-15)
        uint16_t lower = static_cast<uint16_t>(values_[i] & 0xFFFF);
        // Extract upper 16 bits (bits 16-31)
        uint16_t upper = static_cast<uint16_t>(values_[i] >> 16);
        
        // Assign lower to next offset
        if (offsetIndex < CICs_PER_SLINK) {
            offsetMap_[offsetIndex++] = upper;
        }
        
        // Assign upper to next offset
        if (offsetIndex < CICs_PER_SLINK) {
            offsetMap_[offsetIndex++] = lower;
        }
        
        // // Print for debugging
        // std::cout << "Word " << std::dec << i 
        //           << ": lower = 0x" << std::hex << std::setw(4) << std::setfill('0') << lower
        //           << ", upper = 0x" << std::hex << std::setw(4) << std::setfill('0') << upper 
        //           << std::endl;
    }
    
    // // Print the resulting offset map
    // std::cout << "\nOffset Map:" << std::endl;
    // for (size_t i = 0; i < offsetMap_.size(); i++) {
    //     std::cout << "offsetMap[" << std::dec << i << "] = 0x" 
    //               << std::hex << std::setw(4) << std::setfill('0') << offsetMap_[i] << std::endl;
    // }
}

  uint16_t getOffsetForChannel(unsigned int iChannel) {
    if (iChannel >= CICs_PER_SLINK) {
      throw cms::Exception("ChannelsOffset") << " iChannel " << iChannel << " too high";
    }
    return offsetMap_[iChannel];
  }

  void printMap() const {
    for (size_t i = 0; i < offsetMap_.size(); ++i) {
      std::cout << "offsetMap[" << i << "]: " << offsetMap_[i] 
                << "   " << std::bitset<16>(offsetMap_[i])
                << std::endl;
    }
  }
};

#endif