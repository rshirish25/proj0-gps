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
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command TODO
    //!
    //! TODO
    void TODO_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                         U32 cmdSeq            //!< The command sequence number
                         ) override;
};

}  // namespace Components

#endif
