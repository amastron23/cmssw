// FEDRawDataCollectionAnalyzer.cc
// Simple analyzer to retrieve FEDRawDataCollection from DTHDAQToFEDRawDataConverter

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>

class FEDRawDataCollectionAnalyzer : public edm::one::EDAnalyzer<> {
public:
  explicit FEDRawDataCollectionAnalyzer(const edm::ParameterSet& config);
  ~FEDRawDataCollectionAnalyzer() override = default;
  
  void analyze(edm::Event const& event, edm::EventSetup const& setup) override;
  
private:
  edm::EDGetTokenT<FEDRawDataCollection> fedRawToken_;
};

FEDRawDataCollectionAnalyzer::FEDRawDataCollectionAnalyzer(const edm::ParameterSet& config)
  : fedRawToken_(consumes<FEDRawDataCollection>(config.getParameter<edm::InputTag>("fedRawDataTag"))) {
}

void FEDRawDataCollectionAnalyzer::analyze(edm::Event const& event, edm::EventSetup const& setup) {

    edm::Handle<FEDRawDataCollection> fedRawCollection;
    event.getByToken(fedRawToken_, fedRawCollection);

    if (!fedRawCollection.isValid()) {
        edm::LogWarning("FEDRawDataCollectionAnalyzer") << "FEDRawDataCollection not found in event " << event.id().event();
        return;
    }

    const FEDRawData& payload = (*fedRawCollection).FEDData(1230);
    
    if (payload.size() == 0) {
        std::cout << "No data for FED 1230 in event " << event.id().event() << std::endl;
        return;
    }
    
    const unsigned char* data = payload.data();
    size_t size = payload.size();
    
    // Print as 8-byte (64-bit) words with line numbers, one per line
    for (size_t i = 0; i < size; i += 8) {
        if (i + 7 < size) {
            uint64_t word = static_cast<uint64_t>(data[i]) |
                           (static_cast<uint64_t>(data[i+1]) << 8) |
                           (static_cast<uint64_t>(data[i+2]) << 16) |
                           (static_cast<uint64_t>(data[i+3]) << 24) |
                           (static_cast<uint64_t>(data[i+4]) << 32) |
                           (static_cast<uint64_t>(data[i+5]) << 40) |
                           (static_cast<uint64_t>(data[i+6]) << 48) |
                           (static_cast<uint64_t>(data[i+7]) << 56);
            
            // Print with line number (index/8 to get word number)
            std::cout << std::hex << std::setw(4) << std::setfill('0') << (i/8) 
                      << ": " << std::setw(16) << std::setfill('0') << word << std::endl;
        }
    }
}

DEFINE_FWK_MODULE(FEDRawDataCollectionAnalyzer);