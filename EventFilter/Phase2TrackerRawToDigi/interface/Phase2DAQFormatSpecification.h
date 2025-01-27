#ifndef Phase2DAQFormatSpecification_H
#define Phase2DAQFormatSpecification_H

namespace Phase2DAQFormatSpecification
{
    static const int DTC_DAQ_HEADER = 0xFFFFFFFF;
    static const int NUMBER_OF_BITS_PER_WORD = 32;
    static const int NUMBER_OF_BYTES_PER_WORD = 4;
    
    // Channel Header Information (Payload)
    static const int L1ID_MAX_VALUE = 0x1FF;
    static const int L1ID_BITS = 9;
    static const int CIC_ERROR_BITS = 9;
    static const int N_PIXEL_CLUSTER_BITS = 7;
    static const int N_STRIP_CLUSTER_BITS = 7;

    static const int CHIP_ID_MAX_VALUE = 0x7;
    static const int CHIP_ID_BITS = 3;

    static const int SCLUSTER_ADDRESS_2S_MAX_VALUE = 0x7F;
    static const int SCLUSTER_ADDRESS_BITS_2S = 8;

    static const int SCLUSTER_ADDRESS_PS_MAX_VALUE = 0x7F;
    static const int SCLUSTER_ADDRESS_BITS_PS = 7;

    static const int WIDTH_MAX_VALUE = 0x7;
    static const int WIDTH_BITS = 3;

    static const int MIP_BITS = 1;

    static const int SS_CLUSTER_BITS = 14;
    static const int PX_CLUSTER_BITS = 17;
    static const int Z_MAX_VALUE = 0;

    static const int CMSSW_TRACKER_ID = 0;

    typedef std::bitset<32> Word32Bits;
};

#endif