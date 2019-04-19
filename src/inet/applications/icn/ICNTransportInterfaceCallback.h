
#ifndef INET_APPLICATIONS_ICN_ICNTRANSPORTINTERFACECALLBACK_H_
#define INET_APPLICATIONS_ICN_ICNTRANSPORTINTERFACECALLBACK_H_

#include "inet/common/INETDefs.h"

#include "inet/applications/icn/ICNPacket_m.h"

namespace inet {

/**
 * @class ICNTransportInterfaceCallback
 *
 * Interface that defines the method that is called when
 * a packet is received from transport layer. The class
 * that own the ICNTransportInterface should implement
 * this interface.
 */
class INET_API ICNTransportInterfaceCallback {

public:

    /**
     * Constructor.
     */
    ICNTransportInterfaceCallback(void) = default;

    /**
     * Destructor.
     */
    ~ICNTransportInterfaceCallback(void) = default;

    /**
     * Receive an icn packet and the interface it arrived on.
     *
     * @param icnPacket The icn packet.
     * @param arrivalInterfaceId the interface the packet arrived on.
     */
    virtual void receiveICNPacket(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId) = 0;

    /**
     * Notifies when the interface was closed and is unusable.
     */
    virtual void interfaceClosed(void) = 0;

};

} // end namespace inet

#endif /* INET_APPLICATIONS_ICN_ICNTRANSPORTINTERFACECALLBACK_H_ */
