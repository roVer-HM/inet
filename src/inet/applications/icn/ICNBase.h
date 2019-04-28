
#ifndef INET_APPLICATIONS_ICN_ICNBASE_H_
#define INET_APPLICATIONS_ICN_ICNBASE_H_

#include <memory>

#include "inet/common/INETDefs.h"

#include "inet/applications/icn/ICNTransportInterfaceCallback.h"
#include "inet/applications/icn/ICNTransportInterface.h"
#include "inet/applications/icn/ICNUdpInterface.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/applications/icn/ICNRouter.h"
#include "inet/applications/icn/ICNSubscriber.h"

namespace inet {

class ICNBase: public ICNTransportInterfaceCallback, public ApplicationBase {
public:
    /**
     * Constructor.
     */
    ICNBase();

    /**
     * Destructor.
     */
    virtual ~ICNBase() = default;

    // ICNTransportInterfaceCallback
    virtual void receiveICNPacket(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId) override;
    virtual void interfaceClosed(void) override;

protected:

    // Overrides methods from ApplicationBase
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

private:

    const std::string GATE_TO_SUBSCRIBER_APPLICATION_DATA = "subscribedPackets";

    /**
     * Stores the pointer to interface table.
     */
    IInterfaceTable* mInterfaceTableModule;

    /**
     * Stores the ICNTransportInterface
     */
    std::unique_ptr<ICNTransportInterface> mTransportInterface;

    /**
     * States if this is forwarding packages.
     */
    bool mForwarding;

    /**
     * Stores if this base has a subscriber attached to it.
     */
    bool mHasSubscriber;

    /**
     * Stores if this base has an access point attached to it.
     */
    bool mIsAccessPoint;

    /**
     * Stores if this base has a publisher attached to it.
     */
    bool mHasPublisher;

    /**
     * The router module in case this is a router otherwise this will
     * be nullptr.
     */
    ICNRouter* mICNRouterModule;

    /**
     * Helper method for handling publication packets from ICNPublisher
     * module.
     */
    void handlePublicationPacket(Packet* packet);

    /**
     * Helper method for handling subscription packets from ICNSubscriber
     * module.
     */
    void handleSubscriptionPacket(Packet* packet);

    /**
     * Helper method for handling silent subscription packets.
     */
    void handleSilentSubscriptionPacket(Packet* packet);

    /**
     * Helper method to handle broadcast publications.
     */
    void handleBroadcastPublicationPacket(Packet* packet);

    /**
     * Helper method to handle publish packets that were received from network.
     * Publish packets are forwarded to neighbours and/or the local application. This
     * will ask the routing table to get info about that.
     */
    void handlePublishPacketFromNetwork(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId, ICNName& icnName);

    /**
     * Helper method to handle subscribe packets which were received from the network.
     */
    void handleSubscribePacketFromNetwork(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId, ICNName& icnName);
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNBASE_H_ */
