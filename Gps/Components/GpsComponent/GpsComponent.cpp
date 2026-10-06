// ======================================================================
// \title  GpsComponent.cpp
// \author rahulshirishkar
// \brief  cpp file for GpsComponent component implementation class
// ======================================================================

#include "Gps/Components/GpsComponent/GpsComponent.hpp"

#include <cstdlib>
#include <cstring>

#include "Fw/Log/LogString.hpp"

namespace Components {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

GpsComponent::GpsComponent(const char* const compName) : GpsComponentComponentBase(compName) {}

GpsComponent::~GpsComponent() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void GpsComponent::UartRead_handler(FwIndexType portNum,
                                    Fw::Buffer& buffer,
                                    const Drv::ByteStreamStatus& status) {
    static_cast<void>(portNum);

    if (status == Drv::ByteStreamStatus::OP_OK) {
        const U8* const data = buffer.getData();
        for (FwSizeType index = 0; index < buffer.getSize(); index++) {
            this->processByte(static_cast<char>(data[index]));
        }
    } else {
        this->reportParseError("UART receive error");
    }

    // Return the buffer so that the UART driver can reuse its memory.
    this->deallocate_out(0, buffer);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void GpsComponent::GET_UTC_TIME_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->log_ACTIVITY_HI_CurrentUtcTime(this->m_utcTime);
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// GPS parsing helpers
// ----------------------------------------------------------------------

void GpsComponent::processByte(char byte) {
    if (byte == '$') {
        this->m_sentenceLength = 0;
        this->m_sentenceBuffer[this->m_sentenceLength++] = byte;
    } else if (byte == '\n' && this->m_sentenceLength > 0) {
        this->m_sentenceBuffer[this->m_sentenceLength] = '\0';
        this->processSentence();
        this->m_sentenceLength = 0;
    } else if (byte != '\r' && this->m_sentenceLength > 0) {
        if (this->m_sentenceLength < NMEA_BUFFER_SIZE - 1) {
            this->m_sentenceBuffer[this->m_sentenceLength++] = byte;
        } else {
            this->reportParseError("GPS sentence is too long");
            this->m_sentenceLength = 0;
        }
    }
}

void GpsComponent::processSentence() {
    // Ignore all GPS messages except GGA. Both $GPGGA and $GNGGA work.
    if (this->m_sentenceLength < 6 || std::strncmp(&this->m_sentenceBuffer[3], "GGA", 3) != 0) {
        return;
    }

    char sentence[NMEA_BUFFER_SIZE];
    std::strncpy(sentence, this->m_sentenceBuffer, NMEA_BUFFER_SIZE);
    sentence[NMEA_BUFFER_SIZE - 1] = '\0';

    // Remove the optional checksum and split the comma-separated fields.
    char* checksum = std::strchr(sentence, '*');
    if (checksum != nullptr) {
        *checksum = '\0';
    }

    char* fields[15] = {};
    U32 fieldCount = 0;
    fields[fieldCount++] = sentence;
    for (char* character = sentence; *character != '\0' && fieldCount < 15; character++) {
        if (*character == ',') {
            *character = '\0';
            fields[fieldCount++] = character + 1;
        }
    }

    if (fieldCount < 10) {
        this->reportParseError(this->m_sentenceBuffer);
        return;
    }

    char* end = nullptr;
    const long fixQuality = std::strtol(fields[6], &end, 10);
    if (end == fields[6] || *end != '\0') {
        this->reportParseError(this->m_sentenceBuffer);
        return;
    }

    // Before the receiver has a fix, it may not have a UTC time yet.
    if (fields[1][0] != '\0') {
        this->m_utcTime = fields[1];
        this->tlmWrite_UTCTime(this->m_utcTime);
    }

    if (fixQuality == 0) {
        this->log_WARNING_HI_GpsNoLock();
        return;
    }

    if (fields[1][0] == '\0') {
        this->reportParseError(this->m_sentenceBuffer);
        return;
    }

    F64 latitude = 0.0;
    F64 longitude = 0.0;
    if (!this->parseCoordinate(fields[2], fields[3][0], latitude) ||
        !this->parseCoordinate(fields[4], fields[5][0], longitude)) {
        this->reportParseError(this->m_sentenceBuffer);
        return;
    }

    end = nullptr;
    const F64 altitude = std::strtod(fields[9], &end);
    if (end == fields[9] || *end != '\0') {
        this->reportParseError(this->m_sentenceBuffer);
        return;
    }

    this->tlmWrite_Latitude(latitude);
    this->tlmWrite_Longitude(longitude);
    this->tlmWrite_Altitude(altitude);
}

bool GpsComponent::parseCoordinate(const char* value, char direction, F64& coordinate) const {
    char* end = nullptr;
    const F64 rawCoordinate = std::strtod(value, &end);
    if (end == value || *end != '\0') {
        return false;
    }

    const I32 degrees = static_cast<I32>(rawCoordinate / 100.0);
    const F64 minutes = rawCoordinate - static_cast<F64>(degrees * 100);
    if (minutes < 0.0 || minutes >= 60.0) {
        return false;
    }

    coordinate = static_cast<F64>(degrees) + minutes / 60.0;
    if (direction == 'S' || direction == 'W') {
        coordinate = -coordinate;
    } else if (direction != 'N' && direction != 'E') {
        return false;
    }

    return true;
}

void GpsComponent::reportParseError(const char* message) {
    const Fw::LogStringArg errorMessage(message);
    this->log_WARNING_LO_ParseError(errorMessage);
}

}  // namespace Components
