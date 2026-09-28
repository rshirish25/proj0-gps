// ======================================================================
// \title  GpsComponent.hpp
// \author rahulshirishkar
// \brief  hpp file for GpsComponent component implementation class
// ======================================================================

#ifndef Components_GpsComponent_HPP
#define Components_GpsComponent_HPP

#include "Fw/Types/String.hpp"
#include "Gps/Components/GpsComponent/GpsComponentComponentAc.hpp"

namespace Components {

class GpsComponent final : public GpsComponentComponentBase {
  public:
    GpsComponent(const char* const compName);
    ~GpsComponent();

  private:
    // Handle data received from the UART driver
    void UartRead_handler(FwIndexType portNum,
                          Fw::Buffer& buffer,
                          const Drv::ByteStreamStatus& status) override;

    // Report the most recently received GPS UTC time
    void GET_UTC_TIME_cmdHandler(FwOpcodeType opCode,
                                 U32 cmdSeq) override;

    // GPS parsing helpers
    void processByte(char byte);
    void processSentence();
    bool parseCoordinate(const char* value,
                         char direction,
                         F64& coordinate) const;
    void reportParseError(const char* message);

    static constexpr U32 NMEA_BUFFER_SIZE = 128;

    char m_sentenceBuffer[NMEA_BUFFER_SIZE] = {};
    U32 m_sentenceLength = 0;
    Fw::String m_utcTime{"No GPS time received"};
};

}  // namespace Components

#endif
