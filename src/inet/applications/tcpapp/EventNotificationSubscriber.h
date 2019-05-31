

#ifndef __INET_TCPBASICCLIENTAPPADAPTED_H
#define __INET_TCPBASICCLIENTAPPADAPTED_H

#include "inet/common/INETDefs.h"

#include "inet/applications/tcpapp/TcpAppBase.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/NodeStatus.h"

namespace inet {

/**
 * An example request-reply based client application.
 */
class INET_API EventNotificationSubscriber : public TcpAppBase, public cListener {

private:
    enum messageKinds {
        CONNECT = 1,
        SEND = 2,
        CLOSE = 3
    };

    std::set<std::string> contentStore;
protected:
    cMessage* selfMessage = nullptr;
    std::string subscriptionName;
    int expectedBytesResponse;
    // this is for concatenating packets in socketDataArrived() method
    Packet* tempPacket;
    int bytesReceivedSinceReset;

    /**
     * Override receive signal from cListener.
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;

    virtual void sendSubscription();

    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleTimer(cMessage *msg) override;

    virtual void socketEstablished(TcpSocket *socket) override;
    virtual void socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent) override;
    virtual void socketClosed(TcpSocket *socket) override;
    virtual void socketFailure(TcpSocket *socket, int code) override;

    // functiion from operational base
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void close() override;

public:
    EventNotificationSubscriber() {}
    virtual ~EventNotificationSubscriber();
};

} // namespace inet

#endif // ifndef __INET_TCPBASICCLIENTAPPADAPTED_H

