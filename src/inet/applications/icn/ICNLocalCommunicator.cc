#include "inet/applications/icn/ICNLocalCommunicator.h"

#include <omnetpp.h>

#include "inet/applications/icn/ICNPacket_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/applications/icn/MessageKinds.h"
#include "inet/applications/icn/ICNName.h"

namespace inet {

Define_Module(ICNLocalCommunicator);

ICNLocalCommunicator::ICNLocalCommunicator()
: mDoFlooding(false)
, mContentStore()
{
}

ICNLocalCommunicator::~ICNLocalCommunicator() {
}

void ICNLocalCommunicator::initialize() {
    // parse parameters
    mDoFlooding = par("doFlooding").boolValue();
}

void ICNLocalCommunicator::handleMessage(cMessage* msg) {

    if (msg->isSelfMessage()) {
        throw cRuntimeError("Local communicator does not send self messages!");
    } else {
        if (msg->getKind() == MessageKinds::RECEIVED_PUBLICATION) {
            handleReceivedPublication(msg);
        } else if (msg->getKind() == MessageKinds::REQUEST) {
            throw cRuntimeError("Request not handled yet!");
        } else {
            throw cRuntimeError("Local communicator received unhandled message kind!");
        }
    }

}

void ICNLocalCommunicator::handleReceivedPublication(cMessage* message) {
    // just make sure everything is as expected
    ASSERT(message->getKind() == MessageKinds::RECEIVED_PUBLICATION);

    // extract name
    Packet* packet = check_and_cast<Packet*>(message);
    const Ptr<const ICNPacket> icnPacket = packet->peekAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
    std::string name = icnPacket->getIcnName();

    auto search = mContentStore.find(name);

    if (search == mContentStore.end()) {
        // is not yet stored --> we need to store and rebroadcast
        // store
        mContentStore.insert(name);

        // send publication to base
        std::stringstream stringStream;
        stringStream << "FloodingPublication(" << name << ")";
        Packet* floodingPacket = new Packet(stringStream.str().c_str());
        floodingPacket->setKind(MessageKinds::PUBLICATION);
        floodingPacket->insertAtBack(icnPacket);
        send(floodingPacket, "publications");
    }
    // else: is already stored --> we already rebroadcasted

    // delete initially received message
    delete message;
}



} /* namespace inet */
