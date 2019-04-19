
#ifndef INET_APPLICATIONS_ICN_ICN_H_
#define INET_APPLICATIONS_ICN_ICN_H_

#include <memory>

#include "inet/common/INETDefs.h"

#include "inet/applications/icn/ICNTransportInterfaceCallback.h"
#include "inet/applications/icn/ICNTransportInterface.h"
#include "inet/applications/icn/ICNUdpInterface.h"
#include "inet/applications/base/ApplicationBase.h"

namespace inet {

class ICN: public ICNTransportInterfaceCallback, public ApplicationBase {
public:
    /**
     * Constructor.
     */
    ICN();

    /**
     * Destructor.
     */
    virtual ~ICN() = default;

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
    enum SelfMsgKinds { START = 1, SEND, STOP };
    std::string mDestinationContentBasedAddress;
    std::string mLocalContentBasedAddress;
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
     * Send a packet via the transport interface.
     */
    virtual void sendPacket();

    /**
     * Startup the application by initializing transport interface and
     * sending a packet if this is a publisher.
     */
    virtual void processStart();

    /**
     * Calls sendPacket and schedules another send
     * message.
     */
    virtual void processSend();
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICN_H_ */
