

#include "inet/applications/tcpapp/EventNotificationSubscriber.h"
#include "inet/applications/tcpapp/EventNotificationMsg_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TimeTag_m.h"
#include "inet/transportlayer/tcp_common/TcpHeader_m.h"


namespace inet {

Define_Module(EventNotificationSubscriber);

EventNotificationSubscriber::~EventNotificationSubscriber()
{
    cancelAndDelete(selfMessage);
}

void EventNotificationSubscriber::initialize(int stage)
{
    TcpAppBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {

        subscriptionName = par("subscriptionName").stdstringValue();
        expectedBytesResponse = par("expectedBytesResponse").intValue();
        selfMessage = new cMessage("timer");

        //cModule* host = getContainingNode(this);
        // when we are asssociated we connect -> when
        //host->subscribe(l2AssociatedSignal, this);
        //host->subscribe(l2BeaconLostSignal, this);
    }
}

void EventNotificationSubscriber::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    Enter_Method_Silent();
    printSignalBanner(signalID, obj, details);

    if (cComponent::getSignalName(signalID) == cComponent::getSignalName(l2AssociatedSignal)) {
        EV_INFO << "Received association signal. Opening connection and sending subscription..." << endl;
        selfMessage->setKind(messageKinds::CONNECT);
        // give arp and dhcp some time to get addresses then subscribe
        scheduleAt(simTime() + 0.5, selfMessage);
    } else if (cComponent::getSignalName(signalID) == cComponent::getSignalName(l2BeaconLostSignal)) {
        // we lost the connection to an access point
        // close the socket
        //selfMessage->setKind(messageKinds::CLOSE);
        //scheduleAt(simTime(), selfMessage);
    } else {
        throw cRuntimeError("Received signal I did not subscribe to!");
    }

}

void EventNotificationSubscriber::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t startTime = par("startTime").intValue();

    if (startTime < simtime_t::ZERO) {
        throw cRuntimeError("Invalid start time");
    }
    selfMessage->setKind(messageKinds::CONNECT);
    scheduleAt(startTime, selfMessage);
}

void EventNotificationSubscriber::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMessage);
    if (socket.getState() == TcpSocket::CONNECTED || socket.getState() == TcpSocket::CONNECTING || socket.getState() == TcpSocket::PEER_CLOSED)
        close();
}

void EventNotificationSubscriber::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMessage);
    if (operation->getRootModule() != getContainingNode(this))
        socket.destroy();
}

void EventNotificationSubscriber::sendSubscription()
{
    long requestLength = par("requestLength");
    if (requestLength < 1)
        throw cRuntimeError("Request length must be bigger than one. Recommended: 1000Bytes");

    std::stringstream stringStream;
    stringStream << "ClassicSubscription(" << subscriptionName << ")";

    const auto& payload = makeShared<EventNotificationMsg>();
    Packet* packet = new Packet(stringStream.str().c_str());
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setChunkLength(B(requestLength));
    payload->setIcnComparisonName(subscriptionName.c_str());
    payload->setType(PacketType::SUBSCRIBE);
    packet->insertAtBack(payload);

    EV_INFO << "sending request with " << requestLength << " and name " << subscriptionName << std::endl;

    // tell base class to send it
    sendPacket(packet);
}

void EventNotificationSubscriber::handleTimer(cMessage *msg)
{
    switch (msg->getKind()) {
        case messageKinds::CONNECT:
            connect();
            break;

        case messageKinds::SEND:
            sendSubscription();
            break;

        case messageKinds::CLOSE:
            close();
            break;

        default:
            throw cRuntimeError("Invalid timer msg: kind=%d", msg->getKind());
    }
}

void EventNotificationSubscriber::socketEstablished(TcpSocket *socket)
{
    TcpAppBase::socketEstablished(socket);

    // connection is established
    // --> we will send the subscription now!
    selfMessage->setKind(messageKinds::SEND);
    scheduleAt(simTime(), selfMessage);
}

void EventNotificationSubscriber::socketDataArrived(TcpSocket *socket, Packet *msg, bool urgent)
{

    // the packet should be complete now
    // when data arrived on the socket we need to check that its not a duplicate (prob not necessary)
    // and record a scalar of the arrival time
    const Ptr<const EventNotificationMsg> eventNotificationPacket = msg->peekAtFront<EventNotificationMsg>(b(-1), Chunk::PF_ALLOW_INCORRECT);
    std::string name = eventNotificationPacket->getIcnComparisonName();
    auto result = contentStore.insert(name);
    // if the bool value is set to true we have added a value
    if (result.second) {
       EV_INFO << "Added " << name << " to local content store!" << std::endl;
       std::stringstream stringStream;
       stringStream << "DataArrived(" << name << ")";
       recordScalar(stringStream.str().c_str(), simTime());
    }

    // do this last as this will delete the packet :O
    TcpAppBase::socketDataArrived(socket, msg, urgent);

}

void EventNotificationSubscriber::close()
{
    TcpAppBase::close();
    cancelEvent(selfMessage);
}

void EventNotificationSubscriber::socketClosed(TcpSocket *socket)
{
    TcpAppBase::socketClosed(socket);
}

void EventNotificationSubscriber::socketFailure(TcpSocket *socket, int code)
{
    TcpAppBase::socketFailure(socket, code);

    selfMessage->setKind(messageKinds::CONNECT);
    // lets wait 5 seconds until we attempt to reconnect
    scheduleAt(simTime() + 5, selfMessage);
}

} // namespace inet

