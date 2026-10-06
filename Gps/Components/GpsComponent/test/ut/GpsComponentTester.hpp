// ======================================================================
// \title  GpsComponentTester.hpp
// \brief  Test harness for GpsComponent
// ======================================================================

#ifndef Components_GpsComponentTester_HPP
#define Components_GpsComponentTester_HPP

#include "Gps/Components/GpsComponent/GpsComponent.hpp"
#include "Gps/Components/GpsComponent/GpsComponentGTestBase.hpp"

namespace Components {

class GpsComponentTester final : public GpsComponentGTestBase {
  public:
    static const FwSizeType MAX_HISTORY_SIZE = 10;
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

    GpsComponentTester();
    ~GpsComponentTester();

    void testValidGga();
    void testNoLock();
    void testNoLockWithoutUtc();
    void testParseError();

  private:
    void connectPorts();
    void initComponents();
    void sendSentence(char* sentence, FwSizeType size);

    GpsComponent component;
};

}  // namespace Components

#endif
