// ======================================================================
// \title  GpsComponentTestMain.cpp
// \brief  Unit test main for GpsComponent
// ======================================================================

#include "GpsComponentTester.hpp"

TEST(Nominal, ParsesValidGgaAndReportsUtc) {
    Components::GpsComponentTester tester;
    tester.testValidGga();
}

TEST(OffNominal, ReportsNoGpsLock) {
    Components::GpsComponentTester tester;
    tester.testNoLock();
}

TEST(OffNominal, ReportsNoGpsLockWithoutUtc) {
    Components::GpsComponentTester tester;
    tester.testNoLockWithoutUtc();
}

TEST(OffNominal, ReportsParseError) {
    Components::GpsComponentTester tester;
    tester.testParseError();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
