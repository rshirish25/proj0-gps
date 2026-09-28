// ======================================================================
// \title  GpsComponent.hpp
// \author rahulshirishkar
// \brief  hpp file for GpsComponent component implementation class
// ======================================================================

#ifndef Components_GpsComponent_HPP
#define Components_GpsComponent_HPP

#include "Gps/Components/GpsComponent/GpsComponentComponentAc.hpp"

namespace Components {

class GpsComponent final : public GpsComponentComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct GpsComponent object
    GpsComponent(const char* const compName  //!< The component name
    );

    //! Destroy GpsComponent object
    ~GpsComponent();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for UartRead
    //!
    //! Receive GPS data from the UART driver
    void UartRead_handler(FwIndexType portNum,  //!< The port number
                          Fw::Buffer& buffer,
                          const Drv::ByteStreamStatus& status) override;

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command GET_UTC_TIME
    //!
    //! Report the most recently received UTC time
    void GET_UTC_TIME_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                                 U32 cmdSeq            //!< The command sequence number
                                 ) override;
};

}  // namespace Components

#endif
