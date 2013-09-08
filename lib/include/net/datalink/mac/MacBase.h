/*
 * This file is a part of the open source stm32plus library.
 * Copyright (c) 2011,2012,2013 Andy Brown <www.andybrown.me.uk>
 * Please see website for licensing terms.
 */


#pragma once


namespace stm32plus {
  namespace net {

    /**
     * Base class for MAC IO features. Designed to handle the transmit/receive
     * operations without being a template and cannot have dependencies on the PHY
     */

    class MacBase : public virtual NetworkReceiveEvents,
                    public virtual NetworkErrorEvents,
                    public virtual NetworkSendEvents,
                    public virtual NetworkNotificationEvents {

      public:
        static MacBase *_instance;

        /**
         * Error codes generated by this class
         */

        enum {
          E_PHY_WRITE_TIMEOUT=1,                //!< PHY write timeout
          E_PHY_READ_TIMEOUT,                   //!< PHY read timeout
          E_PHY_WAIT_TIMEOUT,                   //!< PHY timed out
          E_CRC,                                //!< E_CRC
          E_TOO_BIG,                            //!< Frame is too big
          E_TRANSMIT_ERROR,                     //!< failed to transmit a frame (see cause for extended error)
          E_RECEIVE,                            //!< E_RECEIVE
          E_WATCHDOG,                           //!< E_WATCHDOG
          E_LATE_COLLISION,                     //!< E_LATE_COLLISION
          E_IP_HEADER_CHECKSUM,                 //!< E_IP_HEADER_CHECKSUM
          E_OVERFLOW,                           //!< E_OVERFLOW
          E_TRUNCATED,                          //!< E_TRUNCATED
          E_PAYLOAD,                            //!< E_PAYLOAD
          E_HEADER,                             //!< E_HEADER
          E_UNSUPPORTED_802_3_FRAME_FORMAT,     //!< The frame format is unsupported (e.g. 802.3 not SNAP)
          E_BUSY,                               //!< A frame is already being sent
          E_TRANSMIT_PROCESS_STOPPED,
          E_TRANSMIT_JABBER_TIMEOUT,
          E_RECEIVE_OVERFLOW,
          E_TRANSMIT_UNDERFLOW,
          E_RECEIVE_BUFFER_UNAVAILABLE,
          E_RECEIVE_PROCESS_STOPPED,
          E_RECEIVE_WATCHDOG_TIMEOUT,
          E_FATAL_BUS_ERROR,
          E_NO_FLASH_DATA,                      ///< cannot transmit data in-place from the flash banks (hardware limitation)
          E_UNSPECIFIED                         //!< E_UNSPECIFIED
        };


        /**
         * Parameters for the MAC base class
         */

        struct Parameters {

          uint16_t mac_mtu;                 //!< default this to 1518 bytes (1500 data plus header (incl vlan option)
          MacAddress mac_address;           //!< address of this device on the LAN
          uint32_t mac_txWaitMillis;        //!< max time to wait for a pending frame to go
          uint8_t mac_receiveBufferCount;   //!< number of receive buffers
          uint8_t mac_transmitBufferCount;  //!< number of transmit buffers

          /**
           * Constructor, set up the defaults
           */

          Parameters() {

            // default MTU

            mac_mtu=1518;

            // generate a default MAC address 02-00-00-00-00-00
            // override this after construction to set your own address

            mac_address.macAddress[0]=2;      // locally-administered bit
            mac_address.macAddress[1]=0;
            mac_address.macAddress[2]=0;
            mac_address.macAddress[3]=0;
            mac_address.macAddress[4]=0;
            mac_address.macAddress[5]=0;

            // time to wait for a pending frame to go = 200ms

            mac_txWaitMillis=200;

            // default number of buffers. higher means better performance and more memory usage but you
            // need to be responsive to the network traffic. lower means less memory usage. The default
            // of 5 is the same as the ST driver default.

            mac_receiveBufferCount=5;
            mac_transmitBufferCount=5;
          }
        };


      protected:

        // receive buffers and descriptors. there's little scope to improve this over ST's
        // implementation as data arrives at the MAC unsolicited

        scoped_array<uint8_t[ETH_MAX_PACKET_SIZE]> _receiveBuffers;
        scoped_array<ETH_DMADESCTypeDef> _receiveDmaDescriptors;

        // the transmit descriptors are created ahead of time but we use no memory for buffers
        // unless we have data to go out and it's free'd once gone

        scoped_array<ETH_DMADESCTypeDef> _transmitDmaDescriptors;
        scoped_array<NetBuffer *> _transmitNetBuffers;
        int _transmitBufferIndex;

        // parameters class

        Parameters _params;

      protected:
        void processReceivedFrame(const FrameTypeDef& frame);
        bool setupEthernetFrame(const FrameTypeDef& fd,EthernetFrame& ef) const;

        bool sendBuffer(NetBuffer *nb);

        bool initialise(const Parameters& params);
        bool startup();

        void onSend(NetEventDescriptor& ned);

      public:
        MacBase();

        void handleReceiveInterrupt();
        void handleTransmitInterrupt();
        void handleErrorInterrupt(uint32_t dmaStatus);

        uint32_t getDatalinkTransmitHeaderSize() const;
        uint32_t getDatalinkMtuSize() const;
    };


    /**
     * Constructor: set up the static instance pointer needed for the
     * interrupt callbacks
     */

    inline MacBase::MacBase() {
      _instance=this;
    }


    /**
     * Get the size of the headers needed to transmit an ethernet frame
     * @return The size of 2 MAC addresses and the EtherType field. A total of 14 bytes.
     */

    inline uint32_t MacBase::getDatalinkTransmitHeaderSize() const {
      return 14;
    }


    /**
     * Get the datalink MTU size. Default is 1500 bytes.
     * @return The MTU size
     */

    inline uint32_t MacBase::getDatalinkMtuSize() const {
      return _params.mac_mtu;
    }
  }
}
