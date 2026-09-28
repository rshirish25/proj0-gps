module Components {

    @ UART GPS receiver and parser
    active component GpsComponent {

        @ Receive GPS data from the UART driver
        sync input port UartRead: Drv.ByteStreamData

        @ Return processed UART buffers
        output port deallocate: Fw.BufferSend

        @ Report the most recently received UTC time
        async command GET_UTC_TIME opcode 0

        @ GPS telemetry
        telemetry UTCTime: string size 32
        telemetry Latitude: F64
        telemetry Longitude: F64
        telemetry Altitude: F64

        @ Report that the GPS does not have a position lock
        event GpsNoLock \
            severity warning high \
            format "GPS does not have a lock"

        @ Report a malformed GPS message
        event ParseError(sentence: string size 128) \
            severity warning low \
            format "Error parsing GPS string: {}"

        @ Report the current GPS UTC time
        event CurrentUtcTime(utcTime: string size 32) \
            severity activity high \
            format "Current UTC time: {}"

        @ Port for requesting the current F Prime time
        time get port timeCaller

        import Fw.Command
        import Fw.Event
        import Fw.Channel
    }
}