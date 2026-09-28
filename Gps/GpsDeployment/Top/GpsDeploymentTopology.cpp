// ======================================================================
// \title  GpsDeploymentTopology.cpp
// \brief cpp file containing the topology instantiation code
//
// ======================================================================
// Provides access to autocoded functions
#include <Gps/GpsDeployment/Top/GpsDeploymentTopologyAc.hpp>
// Note: Uncomment when using Svc:TlmPacketizer
//#include <Gps/GpsDeployment/Top/GpsDeploymentPacketsAc.hpp>

// Necessary project-specified types
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/MallocAllocator.hpp>

// Public functions for use in main program are namespaced with deployment module Gps
// This is also the namespace where the topology components are instantiated by FPP.
namespace Gps {

// Instantiate a malloc allocator for cmdSeq buffer allocation
Fw::MallocAllocator mallocator;

// GPS UART configuration
constexpr FwSizeType GPS_BUFFER_SIZE = 32 * 1024;
constexpr U16 GPS_BUFFER_COUNT = 10;
constexpr const char* GPS_UART_DEVICE = "/dev/ttyUSB0";
bool uartOpened = false;

// Rate group timing: base clock interval and divisors are coupled to rate group names
const Fw::TimeInterval rateGroupInterval(1, 0);  // 1Hz base clock
Svc::RateGroupDriver::DividerSet rateGroupDivisorsSet{{{1, 0}, {2, 0}, {4, 0}}};
// Divisors: 1Hz, 0.5Hz, 0.25Hz

// Context tokens for rate group members (unused, set to zero)
Svc::ActiveRateGroup::ContextArray rateGroup_1HzContext(0);
Svc::ActiveRateGroup::ContextArray rateGroup_0_5HzContext(0);
Svc::ActiveRateGroup::ContextArray rateGroup_0_25HzContext(0);

enum TopologyConstants {
    COMM_PRIORITY = 34,
    UART_PRIORITY = 38,
};

/**
 * \brief configure/setup components in project-specific way
 *
 * This is a *helper* function which configures/sets up each component requiring project specific input. This includes
 * allocating resources, passing-in arguments, etc. This function may be inlined into the topology setup function if
 * desired, but is extracted here for clarity.
 */
void configureTopology() {
    // Rate group driver needs a divisor list
    rateGroupDriver.configure(rateGroupDivisorsSet);

    // Rate groups require context arrays.
    rateGroup_1Hz.configure(rateGroup_1HzContext);
    rateGroup_0_5Hz.configure(rateGroup_0_5HzContext);
    rateGroup_0_25Hz.configure(rateGroup_0_25HzContext);

    // Command sequencer needs to allocate memory to hold contents of command sequences
    cmdSeq.allocateBuffer(0, mallocator, 5 * 1024);

    // PrmDb file name must be supplied by the using topology
    FileHandling::prmDb.configure("PrmDb.dat");

    // Allocate buffers used to receive GPS data.
    Svc::BufferManager::BufferBins gpsBufferBins{};
    gpsBufferBins.bins[0].bufferSize = GPS_BUFFER_SIZE;
    gpsBufferBins.bins[0].numBuffers = GPS_BUFFER_COUNT;
    gpsBufferManager.setup(300, 0, mallocator, gpsBufferBins);

    // Open the GPS UART connection.
    uartOpened = serialDriver.open(GPS_UART_DEVICE, Drv::LinuxUartDriver::BAUD_9600,
                                   Drv::LinuxUartDriver::NO_FLOW, Drv::LinuxUartDriver::PARITY_NONE,
                                   GPS_BUFFER_SIZE);

    if (uartOpened) {
        Fw::Logger::log("[INFO] GPS UART opened successfully\n");
    } else {
        Fw::Logger::log("[ERROR] GPS UART failed to open\n");
    }
}

void setupTopology(const TopologyState& state) {
    // Autocoded initialization. Function provided by autocoder.
    initComponents(state);
    // Autocoded id setup. Function provided by autocoder.
    setBaseIds();
    // Autocoded connection wiring. Function provided by autocoder.
    connectComponents();
    // Autocoded command registration. Function provided by autocoder.
    regCommands();
    // Autocoded configuration. Function provided by autocoder.
    configComponents(state);
    if (state.hostname != nullptr && state.port != 0) {
        comDriver.configure(state.hostname, state.port);
    }
    // Project-specific component configuration. Function provided above. May be inlined, if desired.
    configureTopology();
    // Autocoded parameter read from file. Function provided by autocoder.
    readParameters();
    // Autocoded parameter loading. Function provided by autocoder.
    loadParameters();
    // Autocoded task kick-off (active components). Function provided by autocoder.
    startTasks(state);
    if (uartOpened) {
        serialDriver.start(UART_PRIORITY, Default::STACK_SIZE);
    }
    // Initialize socket communication if and only if there is a valid specification
    if (state.hostname != nullptr && state.port != 0) {
        Os::TaskString name("ReceiveTask");
        // Uplink is configured for receive so a socket task is started
        comDriver.start(name, COMM_PRIORITY, Default::STACK_SIZE);
    }
}

void startRateGroups() {
    // Blocks until stopRateGroups() is called (e.g. from signal handler)
    timer.startTimer(rateGroupInterval);
}

void stopRateGroups() {
    timer.quit();
}

void teardownTopology(const TopologyState& state) {
    if (uartOpened) {
        serialDriver.quitReadThread();
        (void)serialDriver.join();
    }

    gpsBufferManager.cleanup();

    // Autocoded (active component) task clean-up. Functions provided by topology autocoder.
    stopTasks(state);
    freeThreads(state);

    // Other task clean-up.
    comDriver.stop();
    (void)comDriver.join();

    // Resource deallocation
    cmdSeq.deallocateBuffer(mallocator);

    tearDownComponents(state);
    deinitComponents(state);
}
};  // namespace Gps
