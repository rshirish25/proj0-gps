// ======================================================================
// \title  GpsComponentTester.cpp
// \brief  Tests for GpsComponent
// ======================================================================

#include "GpsComponentTester.hpp"

namespace Components {

GpsComponentTester::GpsComponentTester()
    : GpsComponentGTestBase("GpsComponentTester", GpsComponentTester::MAX_HISTORY_SIZE),
      component("GpsComponent") {
    this->initComponents();
    this->connectPorts();
}

GpsComponentTester::~GpsComponentTester() {
    this->component.deinit();
}

void GpsComponentTester::sendSentence(char* sentence, FwSizeType size) {
    Fw::Buffer buffer(reinterpret_cast<U8*>(sentence), size);
    this->invoke_to_UartRead(0, buffer, Drv::ByteStreamStatus::OP_OK);

    ASSERT_from_deallocate_SIZE(1);
    ASSERT_from_deallocate(0, buffer);
}

void GpsComponentTester::testValidGga() {
    char sentence[] = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";
    this->sendSentence(sentence, sizeof(sentence) - 1);

    ASSERT_TLM_UTCTime_SIZE(1);
    ASSERT_TLM_UTCTime(0, "123519");
    ASSERT_TLM_Latitude_SIZE(1);
    EXPECT_NEAR(this->tlmHistory_Latitude->at(0).arg, 48.1173, 0.000001);
    ASSERT_TLM_Longitude_SIZE(1);
    EXPECT_NEAR(this->tlmHistory_Longitude->at(0).arg, 11.5166667, 0.000001);
    ASSERT_TLM_Altitude_SIZE(1);
    EXPECT_NEAR(this->tlmHistory_Altitude->at(0).arg, 545.4, 0.000001);
    ASSERT_EVENTS_GpsNoLock_SIZE(0);
    ASSERT_EVENTS_ParseError_SIZE(0);

    this->sendCmd_GET_UTC_TIME(0, 42);
    this->component.doDispatch();
    ASSERT_EVENTS_CurrentUtcTime_SIZE(1);
    ASSERT_EVENTS_CurrentUtcTime(0, "123519");
    ASSERT_CMD_RESPONSE(0, 0, 42, Fw::CmdResponse::OK);
}

void GpsComponentTester::testNoLock() {
    char sentence[] = "$GNGGA,123520,,,,,0,00,99.9,,,,,,*00\r\n";
    this->sendSentence(sentence, sizeof(sentence) - 1);

    ASSERT_TLM_UTCTime_SIZE(1);
    ASSERT_TLM_UTCTime(0, "123520");
    ASSERT_EVENTS_GpsNoLock_SIZE(1);
    ASSERT_TLM_Latitude_SIZE(0);
    ASSERT_TLM_Longitude_SIZE(0);
    ASSERT_TLM_Altitude_SIZE(0);
}

void GpsComponentTester::testNoLockWithoutUtc() {
    char sentence[] = "$GNGGA,,,,,,0,00,99.99,,,,,,*56\r\n";
    this->sendSentence(sentence, sizeof(sentence) - 1);

    ASSERT_EVENTS_GpsNoLock_SIZE(1);
    ASSERT_EVENTS_ParseError_SIZE(0);
    ASSERT_TLM_UTCTime_SIZE(0);
    ASSERT_TLM_Latitude_SIZE(0);
    ASSERT_TLM_Longitude_SIZE(0);
    ASSERT_TLM_Altitude_SIZE(0);
}

void GpsComponentTester::testParseError() {
    char sentence[] = "$GPGGA,123521,bad,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*00\r\n";
    this->sendSentence(sentence, sizeof(sentence) - 1);

    ASSERT_EVENTS_ParseError_SIZE(1);
    ASSERT_TLM_Latitude_SIZE(0);
    ASSERT_TLM_Longitude_SIZE(0);
    ASSERT_TLM_Altitude_SIZE(0);
}

}  // namespace Components
