

#ifndef __INET_TCPGENERICSRVAPP_H
#define __INET_TCPGENERICSRVAPP_H

#include "inet/common/lifecycle/LifecycleUnsupported.h"
#include "inet/common/packet/ChunkQueue.h"
#include "inet/transportlayer/contract/tcp/TcpSocket.h"

namespace inet {

/**
 * EventNotificationPublisher
 *
 * This is the server of the event notification system. It will send messages to connected
 * clients.
 */
class INET_API EventNotificationPublisher : public cSimpleModule, public LifecycleUnsupported
{
  protected:
    TcpSocket socket;

    long msgsRcvd;
    long msgsSent;
    long bytesRcvd;
    long bytesSent;

    std::set<int> subscribedClients;
    cMessage* selfMessage;
    std::string publicationName;
    int publicationSize;
    int delay;
    bool repeat;
    bool hasSentPublication;
    std::string latestSentPublication;

  protected:
    virtual void sendPublications();
    void sendPublication(std::string name, int connId);

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;
};

} // namespace inet

#endif // ifndef __INET_TCPGENERICSRVAPP_H

