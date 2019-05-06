#include "inet/applications/icn/ICNBase.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/TimeTag_m.h"
#include "inet/applications/icn/MessageKinds.h"
#include "inet/applications/icn/ICNName.h"

namespace inet {

Define_Module(ICNBase);

ICNBase::ICNBase()
: mInterfaceTableModule(nullptr)
, mTransportInterface(new ICNUdpInterface())
, mForwarding(false)
, mHasSubscriber(false)
, mHasPublisher(false)
, mHasLocalCommunicator(false)
, mICNRouterModule(nullptr)
, mInfrastructureCommunicationInterfaceId(ICNRouter::UNDEFINED_ID)
, mLocalCommunicationInterfaceId(ICNRouter::UNDEFINED_ID)
, mContentMap()
{
}

void ICNBase::receiveICNPacket(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId) {
    ICNName icnName(icnPacket->getIcnName());

    if (arrivalInterfaceId != mLocalCommunicationInterfaceId) {
        EV_INFO << "Processing packet with name '" << icnName.generateString() << "' arrived on infrastructure communication interface " << "..." << endl;
        // find the appropriate handler for each type of message
        if (icnPacket->getPacketType() == ICNPacketType::PUBLISH) {
            handlePublishPacketFromNetwork(icnPacket, arrivalInterfaceId, icnName);
        } else if (icnPacket->getPacketType() == ICNPacketType::SUBSCRIBE) {
            // delegate...
            handleSubscribePacketFromNetwork(icnPacket, arrivalInterfaceId, icnName);
        }  else if (icnPacket->getPacketType() == ICNPacketType::UNSUBSCRIBE) {
            throw cRuntimeError("Received local request, response or unsubscribe which cant be handled yet!");
        }
    } else {
        EV_INFO << "Processing packet with name '" << icnName.generateString() << "' arrived on local communication interface " << "..." << endl;
        // find the appropriate handler for each type of message
        if (icnPacket->getPacketType() == ICNPacketType::PUBLISH) {
            // delegate...
            handlePublishPacketFromNetwork(icnPacket, arrivalInterfaceId, icnName);
        } else {
            throw cRuntimeError("Received packet type on local interface that is not handled yet!");
        }
    }



}

void ICNBase::handleSubscribePacketFromNetwork(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId, ICNName& icnName) {
    // in case we receive subscribe packet and we have forwarding enabled we will store the subscription
    // for later forwarding
    if (mForwarding) {
        bool routingTableChanged = mICNRouterModule->addSubscription(arrivalInterfaceId, icnName);
        // if we havent seen such a subscription yet we will forward it to all our interfaces
        if (routingTableChanged) {
            // encapsulate into packet
            Packet* packet = new Packet("ICNForwardedSubscriptionData");
            const auto& icnPacketDuplicate = icnPacket->dupShared();
            packet->insertAtBack(icnPacketDuplicate);

            Packet* copy = packet->dup();
            for (int interfacePosition = 0; interfacePosition < mInterfaceTableModule->getNumInterfaces(); interfacePosition++) {
                int interfaceId = mInterfaceTableModule->getInterface(interfacePosition)->getInterfaceId();
                if (interfaceId != arrivalInterfaceId) {
                    mTransportInterface->sendICNPacket(copy, interfaceId);
                    copy = packet->dup();
                    EV_INFO << "Forwarded subscription to interface with id: " << interfaceId << endl;
                } else {
                    EV_INFO << "Not forwarding subscription to interface with id " << interfaceId << " becuse its the interface the packet arrived on!" << endl;
                }
            }
            delete packet;
        } else {
            EV_INFO << "Not forwarding the subscription because we already had one with the same name!" << endl;
        }
    } else {
        EV_INFO << "Forwarding is not enabled on this node!" << endl;
    }
}

void ICNBase::handlePublishPacketFromNetwork(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId, ICNName& icnName) {

    auto search = mContentMap.find(icnName.generateString());
    bool seenBefore = search != mContentMap.end();

    // get the interfaces we need to forward this message to
    std::vector<int> forwardTo = mICNRouterModule->getInterestedInterfaces(icnName);

    if (!forwardTo.empty()) {
        // iterate whole vector and remove elements equal to arrivalInterfaceId
        for (auto iterator = forwardTo.begin(); iterator != forwardTo.end(); ) {
            if (*iterator == arrivalInterfaceId) {
                iterator = forwardTo.erase(iterator);
            } else {
                iterator++;
            }
        }

        // encapsulate into packet
        Packet* packet = new Packet("ICNForwardedPublisherData");
        packet->insertAtBack(icnPacket);

        for (int& interfaceId: forwardTo) {
            if (interfaceId == ICNRouter::LOCAL_APPLICATION_ID && !seenBefore) {
                Packet* duplicate = packet->dup();
                // notify local application
                duplicate->setKind(MessageKinds::SUBSCRIPTION_RESULT);
                send(duplicate, GATE_TO_SUBSCRIBER_APPLICATION_DATA.c_str());
                EV_INFO << "Packet with name '" << icnName.generateString() << "' forwarded to local app." << endl;
                mContentMap[icnName.generateString()] = false;
                if (mHasLocalCommunicator) {
                    mContentMap[icnName.generateString()] = true;
                    EV_INFO << "Packet with name " << icnName.generateString() << " will be broadcasted on local interface!" << endl;
                    Packet* localPacket = new Packet("ICNLocalForwardedPublisherData");
                    localPacket->insertAtBack(icnPacket);
                    mTransportInterface->sendICNPacket(localPacket, mLocalCommunicationInterfaceId);
                }
            } else if (mForwarding) {
                mTransportInterface->sendICNPacket(packet->dup(), interfaceId);
                EV_INFO << "Packet with name '" << icnName.generateString() << "' forwarded to interface with id " << interfaceId << endl;
            }
        }

        delete packet;
    } else {
        // the list was empty that means that there is no interest for this packet here
        EV_INFO << "Packet with name '" << icnName.generateString() << "' is not interesting for a local app or for forwarding!" << endl;
    }
}

void ICNBase::interfaceClosed(void) {
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(-1);
}

void ICNBase::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        mInterfaceTableModule = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        mForwarding = par("icnForwarding").boolValue();
        mHasSubscriber = par("hasSubscriber").boolValue();
        mHasPublisher = par("hasPublisher").boolValue();
        mHasLocalCommunicator = par("hasLocalCommunicator").boolValue();
        mICNRouterModule = getModuleFromPar<ICNRouter>(par("routerModule"), this);
        mTransportInterface->setCallback(this);
    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        mTransportInterface->initialize(mInterfaceTableModule, gate("socketOut"));

        for (int interfacePosition = 0; interfacePosition < mInterfaceTableModule->getNumInterfaces(); interfacePosition++) {
            int interfaceId = mInterfaceTableModule->getInterface(interfacePosition)->getInterfaceId();
            std::string interfaceName(mInterfaceTableModule->getInterface(interfacePosition)->getInterfaceName());
            // static: the first wlan interface on all hosts will be used for infrastructure communication
            // the second one will be used for local communication
            // if there is no second wlan interface mLocalCommunicationInterfaceId will stay undefined
            if (interfaceName == "wlan0") {
                mInfrastructureCommunicationInterfaceId = interfaceId;
            } else if (interfaceName == "wlan1") {
                mLocalCommunicationInterfaceId = interfaceId;
            }
        }
    }
}

void ICNBase::finish()
{
    ApplicationBase::finish();
}

void ICNBase::handleMessageWhenUp(cMessage* msg) {
    if (msg->isSelfMessage()) {
        throw cRuntimeError("ICNBase does not send self Messages!");
    } else {
        if(msg->getKind() == MessageKinds::PUBLICATION) {
            ASSERT(mHasPublisher);
            // we received a message from the publisher
            Packet* packet = check_and_cast<Packet*>(msg);
            handlePublicationPacket(packet);
            delete packet;
        // this is not used anymore
        } else if (msg->getKind() == MessageKinds::BROADCAST_PUBLICATION) {
            // this is the same as regular publication with the difference
            // that there will be no check if some1 is interested in my data
            Packet* packet = check_and_cast<Packet*>(msg);
            handleBroadcastPublicationPacket(packet);
            delete packet;
        } else if (msg->getKind() == MessageKinds::SUBSCRIPTION) {
            ASSERT(mHasSubscriber);
            // we received a subscription from subscriber (local application)
            Packet* packet = check_and_cast<Packet*>(msg);
            handleSubscriptionPacket(packet);
            delete packet;
        // this is not used anymore
        } else if (msg->getKind() == MessageKinds::SILENT_SUBSCRIPTION) {
            ASSERT(mHasSubscriber);
            // this is the same as a subscription with the difference that it is
            // only registered in the routing module but not sent to other interfaces
            Packet* packet = check_and_cast<Packet*>(msg);
            bool infrastructure = msg->arrivedOn("messageInterfaceInfrastructure");
            handleSilentSubscriptionPacket(packet, infrastructure);
            delete packet;
        } else {
            mTransportInterface->processMessage(msg);
        }
    }
}

void ICNBase::handleBroadcastPublicationPacket(Packet* packet) {
    EV_INFO << "Received request to broadcast a publication!" << endl;
    Packet* copy = packet->dup();

    // sent to all interfaces...
    for (int position = 0; position < mInterfaceTableModule->getNumInterfaces(); position++) {
        mTransportInterface->sendICNPacket(copy, mInterfaceTableModule->getInterface(position)->getInterfaceId());
        copy = packet->dup();
    }
    delete copy;
}

void ICNBase::handleSilentSubscriptionPacket(Packet* packet, bool infrastructure) {
    // we need to investigate the contents
    const Ptr<const ICNPacket> icnPacket = packet->peekAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
    std::string subscriptionName = icnPacket->getIcnName();
    // transform to ICNName
    ICNName icnName(subscriptionName);
    if (infrastructure) {
        mICNRouterModule->addSubscription(ICNRouter::LOCAL_APPLICATION_ID, icnName);
    } else {
        mICNRouterModule->addSubscription(ICNRouter::LOCAL_COMMUNICATOR_ID, icnName);
    }


}

void ICNBase::handlePublicationPacket(Packet* packet) {
    // extract name of publication
    const Ptr<const ICNPacket> icnPacket = packet->peekAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
    std::string publicationName = icnPacket->getIcnName();
    // transform to ICNName
    ICNName icnName(publicationName);

    Packet* copy = packet->dup();
    std::vector<int> interestedInterfaces = mICNRouterModule->getInterestedInterfaces(icnName);
    for (int& interfaceId: interestedInterfaces) {
        if (interfaceId != ICNRouter::LOCAL_APPLICATION_ID) {
            mTransportInterface->sendICNPacket(copy, interfaceId);
            copy = packet->dup();
        }
    }
    delete copy;
}

void ICNBase::handleSubscriptionPacket(Packet* packet) {
    // we need to investigate the contents
    const Ptr<const ICNPacket> icnPacket = packet->peekAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
    std::string subscriptionName = icnPacket->getIcnName();
    // transform to ICNName
    ICNName icnName(subscriptionName);
    mICNRouterModule->addSubscription(ICNRouter::LOCAL_APPLICATION_ID, icnName);

    Packet* copy = packet->dup();
    // next we need to spread the word about our amazing subscription on all available interfaces
    for (int interfacePosition = 0; interfacePosition < mInterfaceTableModule->getNumInterfaces(); interfacePosition++) {
        int interfaceId = mInterfaceTableModule->getInterface(interfacePosition)->getInterfaceId();
        if (interfaceId == mLocalCommunicationInterfaceId) {
            EV_INFO << "Not announcing my subscription on the local communication interface!" << endl;
        } else {
            mTransportInterface->sendICNPacket(copy, interfaceId);
            EV_INFO <<  "Forwarded the subscription to interface with id " << interfaceId << "." << endl;
            copy = packet->dup();
        }
    }
    delete copy;
}

void ICNBase::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();
}

void ICNBase::handleStartOperation(LifecycleOperation *operation)
{
}

void ICNBase::handleStopOperation(LifecycleOperation *operation)
{
    mTransportInterface->closeSocket();
    delayActiveOperationFinish(2);
}

void ICNBase::handleCrashOperation(LifecycleOperation *operation)
{
    mTransportInterface->destroySocket();
}


} /* namespace inet */
