
#include "inet/applications/tcpapp/EventNotificationPublisher.h"

#include "inet/applications/common/SocketTag_m.h"
#include "inet/applications/tcpapp/EventNotificationMsg_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/tcp/TcpCommand_m.h"

namespace inet {

Define_Module(EventNotificationPublisher);

void EventNotificationPublisher::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        //statistics
        msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

        WATCH(msgsRcvd);
        WATCH(msgsSent);
        WATCH(bytesRcvd);
        WATCH(bytesSent);
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {
        const char *localAddress = par("localAddress");
        int localPort = par("localPort");
        publicationName = par("publicationName").stdstringValue();
        publicationSize = par("dataSize");
        simtime_t startTime = par("startTime").intValue();
        if (startTime < SimTime::ZERO) {
            throw cRuntimeError("Invalid start time!");
        }
        delay = par("delay");
        repeat = par("repeat");
        selfMessage = new cMessage("timer");
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localAddress[0] ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
        socket.listen();

        cModule *node = findContainingNode(this);
        NodeStatus *nodeStatus = node ? check_and_cast_nullable<NodeStatus *>(node->getSubmodule("status")) : nullptr;
        bool isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        if (!isOperational)
            throw cRuntimeError("This module doesn't support starting in node DOWN state");

        // schedule the first self-message
        if (startTime <= simTime()) {
            // immediately
            scheduleAt(simTime(), selfMessage);
        } else {
            // later
            scheduleAt(startTime, selfMessage);
        }
    }
}

void EventNotificationPublisher::sendPublications()
{
    // first create the packet
    std::stringstream stringStream;
    stringStream << publicationName << "/" << simTime();
    std::string name = stringStream.str();
    stringStream.str("");
    stringStream << "ClassicPublication(" << name << ")";
    Packet* publicationPacket = new Packet(stringStream.str().c_str());

    // set settings for the packet that are the same for all connection ids
    publicationPacket->setKind(TCP_C_SEND);
    publicationPacket->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::tcp);
    const auto& payload = makeShared<EventNotificationMsg>();
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setChunkLength(B(publicationSize));
    payload->setIcnComparisonName(name.c_str());
    payload->setType(PacketType::PUBLISH);
    publicationPacket->insertAtBack(payload);

    for (auto connId: subscribedClients) {
        publicationPacket->addTagIfAbsent<SocketReq>()->setSocketId(connId);
        send(publicationPacket->dup(), "socketOut");
        msgsSent++;
        bytesSent += publicationPacket->getByteLength();
        EV_INFO << "Sending subscription packet with name " << name << " to TCP. Connection id is: " << connId << std::endl;
    }

    delete publicationPacket;
}

void EventNotificationPublisher::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        sendPublications();
        if (repeat) {
            scheduleAt(simTime() + delay, msg);
        }
    }
    else if (msg->getKind() == TCP_I_PEER_CLOSED) {
        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        int connId = check_and_cast<Indication *>(msg)->getTag<SocketInd>()->getSocketId();
        delete msg;
        auto request = new Request("close", TCP_C_CLOSE);
        request->addTagIfAbsent<SocketReq>()->setSocketId(connId);
        scheduleAt(simTime(), request);

        // remove from subscribed clients
        size_t numberOfElementsRemoved = subscribedClients.erase(connId);
        if (numberOfElementsRemoved != 1) {
            throw cRuntimeError("A connection got closed that was never opened!");
        }
    }
    else if (msg->getKind() == TCP_I_DATA || msg->getKind() == TCP_I_URGENT_DATA) {

        // cast to packet
        Packet *packet = check_and_cast<Packet *>(msg);
        // extract data
        const Ptr<const EventNotificationMsg> eventNotificationPacket = packet->peekAtFront<EventNotificationMsg>(b(-1), Chunk::PF_ALLOW_INCORRECT);
        // we expect a subscription
        if (eventNotificationPacket->getType() == PacketType::SUBSCRIBE) {
            // add to subscribers
            int connId = packet->getTag<SocketInd>()->getSocketId();
            subscribedClients.insert(connId);
            // this is all wee need to do
        } else {
            EV_INFO << "Received non subscription packet type. Discarding packet..." << std::endl;
        }

        delete msg;

    }
    else if (msg->getKind() == TCP_I_AVAILABLE)
        socket.processMessage(msg);
    else {
        // some indication -- ignore
        EV_WARN << "drop msg: " << msg->getName() << ", kind:" << msg->getKind() << "(" << cEnum::get("inet::TcpStatusInd")->getStringFor(msg->getKind()) << ")\n";
        delete msg;
    }
}

void EventNotificationPublisher::refreshDisplay() const
{
    char buf[64];
    sprintf(buf, "rcvd: %ld pks %ld bytes\nsent: %ld pks %ld bytes", msgsRcvd, bytesRcvd, msgsSent, bytesSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void EventNotificationPublisher::finish()
{
    EV_INFO << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV_INFO << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";
}

} // namespace inet

