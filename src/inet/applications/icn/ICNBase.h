
#ifndef INET_APPLICATIONS_ICN_ICNBASE_H_
#define INET_APPLICATIONS_ICN_ICNBASE_H_

#include <memory>

#include "inet/common/INETDefs.h"

#include "inet/applications/icn/ICNTransportInterfaceCallback.h"
#include "inet/applications/icn/ICNTransportInterface.h"
#include "inet/applications/icn/ICNUdpInterface.h"
#include "inet/applications/base/ApplicationBase.h"
#include "inet/applications/icn/ICNRouter.h"

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
    const std::string PUBLISHER_GATE_NAME = "icnPublisherIn";
    const std::string SUBSCRIBER_GATE_NAME_IN = "icnSubscriberIn";
    enum SelfMsgKinds { START = 1, STOP };

    /**
     * Self message to trigger start and stop.
     */
    cMessage* mSelfMessage;

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
     * The router module in case this is a router otherwise this will
     * be nullptr.
     */
    ICNRouter* mICNRouterModule;

    /**
     * Startup the application by initializing transport interface and
     * sending a packet if this is a publisher.
     */
    virtual void processStart();
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNBASE_H_ */
