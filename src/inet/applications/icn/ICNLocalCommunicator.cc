#include "inet/applications/icn/ICNLocalCommunicator.h"

#include <omnetpp.h>

#include "inet/applications/icn/ICNPacket_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/applications/icn/MessageKinds.h"
#include "inet/applications/icn/ICNName.h"
#include "inet/common/Simsignals.h"
#include "inet/common/ModuleAccess.h"

namespace inet {

Define_Module(ICNLocalCommunicator);

ICNLocalCommunicator::ICNLocalCommunicator()
: mDoFlooding(false)
, mRequestDelay(-1)
, mRequestSize(-1)
, mRequestedDataName("/undefined")
, mPublicationSize(-1)
, mAssociatedToAP(false)
, mRequestMessage(nullptr)
, mContentStore()
{
}

ICNLocalCommunicator::~ICNLocalCommunicator() {
    cancelAndDelete(mRequestMessage);
}

void ICNLocalCommunicator::initialize() {

    // initialize class variables
    mRequestMessage = new cMessage("requestMessage");

    // parse parameters
    mDoFlooding = par("doFlooding").boolValue();
    mRequestDelay = par("requestDelay").intValue();
    mRequestSize = par("requestSize").intValue();
    mRequestedDataName = ICNName(par("requestedDataName").stdstringValue());
    mPublicationSize = par("publicationSize").intValue();

    if (mRequestSize <= 0) {
        throw cRuntimeError("Request size must be larger than zero!");
    }
    if (mPublicationSize <= 0) {
        throw cRuntimeError("Publication size must be larger than zero!");
    }


    // subscribe to signals
    cModule* host = getContainingNode(this);
    // associated to ap
    host->subscribe(l2AssociatedSignal, this);
    host->subscribe(l2BeaconLostSignal, this);

    // as we dont know if we will ever get associated to
    // an ap we need to schedule this message
    if (mRequestDelay > 0) {
        scheduleAt(simTime() + mRequestDelay, mRequestMessage);
    } else {
        scheduleAt(simTime(), mRequestMessage);
    }
}

void ICNLocalCommunicator::handleMessage(cMessage* msg) {

    if (msg->isSelfMessage()) {
        // we received mRequestMessage
        ASSERT(mRequestMessage == msg);
        handleSelfMessageRequest();
    } else {
        if (msg->getKind() == MessageKinds::RECEIVED_PUBLICATION) {
            handleReceivedPublication(msg);
        } else if (msg->getKind() == MessageKinds::REQUEST) {
            handleReceivedRequest(msg);
        } else {
            throw cRuntimeError("Local communicator received unhandled message kind!");
        }
    }

}

void ICNLocalCommunicator::handleSelfMessageRequest(void) {
    // create and fill ICNPacket
    const auto& payload = makeShared<ICNPacket>();
    payload->setChunkLength(B(mRequestSize));
    payload->setIcnName(mRequestedDataName.generateString().c_str());
    payload->setPacketType(ICNPacketType::REQUEST);
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

    // encapsulate into packet
    std::stringstream stringStream;
    stringStream << "Request(" << mRequestedDataName.generateString() << ")";
    Packet* packet = new Packet(stringStream.str().c_str());
    packet->insertAtBack(payload);

    packet->setKind(MessageKinds::REQUEST);

    // this will transport the packet to icn base
    send(packet, "requests");

    // schedule another request if the request delay is bigger than zero
    if (mRequestDelay > 0) {
        scheduleAt(simTime() + mRequestDelay, mRequestMessage);
    }

    EV_INFO << "Send request for " << mRequestedDataName.generateString() << " to icn base to send it via local interface!" << endl;

}

void ICNLocalCommunicator::handleReceivedRequest(cMessage* message) {
    // check if everything is right
    ASSERT(message->getKind() == MessageKinds::REQUEST);

    // extract name
    Packet* packet = check_and_cast<Packet*>(message);
    const Ptr<const ICNPacket> icnPacket = packet->peekAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
    std::string name = icnPacket->getIcnName();
    ICNName icnName(name);

    // check if we have stored that data...
    size_t index = 0;
    bool searchSuccessfull = findInContentStore(icnName, index);

    if (searchSuccessfull) {
        ICNName response = mContentStore[index];
        // we have it stored we can answer the request
        // create and fill ICNPacket
        const auto& payload = makeShared<ICNPacket>();
        payload->setChunkLength(B(mRequestSize));
        payload->setIcnName(response.generateString().c_str());
        payload->setPacketType(ICNPacketType::PUBLISH);
        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

        // encapsulate into packet
        std::stringstream stringStream;
        stringStream << "RequestPublication(" << response.generateString() << ")";
        Packet* packet = new Packet(stringStream.str().c_str());
        packet->insertAtBack(payload);

        packet->setKind(MessageKinds::PUBLICATION);

        // this will transport the packet to icn base
        send(packet, "publications");

        EV_INFO << "We could answer request for '" << name << "' with '" << response.generateString() << "'!" << endl;
    }

    delete message;
}

void ICNLocalCommunicator::handleReceivedPublication(cMessage* message) {
    // just make sure everything is as expected
    ASSERT(message->getKind() == MessageKinds::RECEIVED_PUBLICATION);

    // extract name
    Packet* packet = check_and_cast<Packet*>(message);
    const Ptr<const ICNPacket> icnPacket = packet->peekAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
    std::string name = icnPacket->getIcnName();
    ICNName icnName(name);

    if (addToContentStore(icnName)) {
        // something was added to the content store --> we need to rebroadcast
        // send publication to base
        if (mDoFlooding) {
            std::stringstream stringStream;
            stringStream << "FloodingPublication(" << name << ")";
            Packet* floodingPacket = new Packet(stringStream.str().c_str());
            floodingPacket->setKind(MessageKinds::PUBLICATION);
            floodingPacket->insertAtBack(icnPacket);
            send(floodingPacket, "publications");
            EV_INFO <<  "Received a publication that we have not seen before. We will rebroadcast it!" << endl;
        }

    } else {
        // else: is already stored --> we already rebroadcasted
        EV_INFO << "Receive a publication that we have already seen before. We will not rebroadcast!" << endl;
    }

    // if the publication matched the one we were asking for we should reschedule the message
    if (!mAssociatedToAP && icnName.matches(mRequestedDataName)) {
        cancelEvent(mRequestMessage);
        scheduleAt(simTime() + mRequestDelay, mRequestMessage);
    }

    // delete initially received message
    delete message;
}

void ICNLocalCommunicator::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    Enter_Method_Silent();
    printSignalBanner(signalID, obj, details);

    // find out which signal it is
    if (cComponent::getSignalName(signalID) == cComponent::getSignalName(l2AssociatedSignal)) {
        // we associated an access point
        // ther is no need to send requests anymore
        cancelEvent(mRequestMessage);
        EV_INFO << "Received signal that we are associated to an access point. Stop sending local requests." << endl;
        mAssociatedToAP = true;
    } else if (cComponent::getSignalName(signalID) == cComponent::getSignalName(l2BeaconLostSignal)) {
        // we lost the connection to an access point
        // schedule self message to send request to icn base
        if (mRequestDelay > 0) {
            scheduleAt(simTime() + mRequestDelay, mRequestMessage);
        } else {
            scheduleAt(simTime(), mRequestMessage);
        }
        EV_INFO << "Received signal that we are not associated to an access point anymore. Start sending local requests." << endl;
        mAssociatedToAP = false;

    } else {
        throw cRuntimeError("Received signal I did not subscribe to!");
    }

}

bool ICNLocalCommunicator::findInContentStore(ICNName& name, size_t& index) {
    bool result = false;
    // iterate the content store
    for (size_t position = 0; position < mContentStore.size(); position++) {
        if (mContentStore.at(position).matches(name)) {
            // only one should be found (if not the one with the highest position will be used as that one is the newest)
            index = position;
            result = true;
        }
    }
    return result;
}

bool ICNLocalCommunicator::addToContentStore(ICNName& name) {
    bool addedSomethingToContentStore = false;
    if (name.isPrefixMatched()) {
        EV_INFO << "Prefix matched names are not added to the content store!" << endl;
    } else {
        bool needToAdd = true;
        // there is two possible ways when we dont need to add the new name:
        //          - we already have something stored that it is identical (prefix matched names are not possible)
        //          - we already have something stored with a higher version number
        // in any other case we add the new name
        for (size_t index = 0; index < mContentStore.size(); index++) {
            ICNName currentName = mContentStore[index];
            // identical match
            if (name.matches(currentName)) {
                needToAdd = false;
                index = mContentStore.size();
                // both have a version and match without it
            } else if (name.hasVersion() && currentName.hasVersion() && name.matchWithoutVersion(currentName)) {
                // need to decide which one to keep
                if (name.hasHigherVersion(currentName)) {
                    // the new one has the higher version we keep that
                    mContentStore.erase(mContentStore.begin() + index);
                    index = mContentStore.size();
                } else {
                    needToAdd = false;
                }
            }
        }
        if (needToAdd) {
            mContentStore.push_back(name);
            addedSomethingToContentStore = true;
            EV_INFO << "Added " << name.generateString() << " to content store!" << endl;
        }
    }
    return addedSomethingToContentStore;
}


} /* namespace inet */
