
#ifndef INET_APPLICATIONS_ICN_ICNTRANSPORTINTERFACE_H_
#define INET_APPLICATIONS_ICN_ICNTRANSPORTINTERFACE_H_

#include "inet/common/INETDefs.h"

#include "inet/applications/icn/ICNTransportInterfaceCallback.h"
#include "inet/applications/icn/ICNPacket_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/contract/IInterfaceTable.h"

namespace inet {

/**
 * @class ICNTransportInterface
 *
 * This stores the callback
 */
class INET_API ICNTransportInterface {

public:

    const int ICN_PORT = 5555;
    const std::string ICN_MULTICAST_ADDRESS = "224.0.0.1";
    const std::string ICN_OUTPUT_GATE_NAME = "socketOut";
    const std::string ICN_MULTICAST_MAC_ADDRESS = "01005E000001";

    /**
     * Constructor.
     */
    ICNTransportInterface(void);

    /**
     * Destructor.
     */
    virtual ~ICNTransportInterface(void);

    /**
     * Send the given packet via the given interface id.
     *
     * @param icnPacket encpsulated into packet
     * @param interfaceId the interface id which will be used to send the data
     */
    virtual void sendICNPacket(Packet* icnPacket, int interfaceId) = 0;

    /**
     * Initialize the interface.
     */
    virtual void initialize(IInterfaceTable* interfaceTableModule, cGate* gate) = 0;

    /**
     * Close the underlying socket.
     */
    virtual void closeSocket(void) = 0;

    /**
     * Destroy the underlying socket.
     */
    virtual void destroySocket(void) = 0;

    /**
     * Process an incoming message.
     */
    virtual void processMessage(cMessage* message) = 0;

    /**
     * Sets the class that will get called when data arrived
     * on the transport layer interface.
     *
     * @param callback The ICNTransportInterfaceCallback object to store.
     */
    void setCallback(ICNTransportInterfaceCallback* callback);

protected:

    /**
     * Get the callback.
     *
     * @return pointer to ICNTransportInterfaceCallback
     */
    ICNTransportInterfaceCallback* getCallback(void);

private:

    /**
     * Stores the class that will be called when a packet has arrived.
     */
    ICNTransportInterfaceCallback* mCallback;

};

} // end namespace inet

#endif /* INET_APPLICATIONS_ICN_ICNTRANSPORTINTERFACE_H_ */
