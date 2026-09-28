// ======================================================================
// \title  GpsComponent.cpp
// \author rahulshirishkar
// \brief  cpp file for GpsComponent component implementation class
// ======================================================================

#include "Gps/Components/GpsComponent/GpsComponent.hpp"

namespace Components {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

GpsComponent ::GpsComponent(const char* const compName) : GpsComponentComponentBase(compName) {}

GpsComponent ::~GpsComponent() {}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void GpsComponent ::TODO_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Components
