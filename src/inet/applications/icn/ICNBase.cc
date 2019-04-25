#include "ICNBase.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/TimeTag_m.h"

namespace inet {

Define_Module(ICNBase);

ICNBase::ICNBase()
: mSelfMessage(nullptr)
, mInterfaceTableModule(nullptr)
, mTransportInterface(new ICNUdpInterface())
, mForwarding(false)
, mICNRouterModule(nullptr)
, mICNSubscriberModule(nullptr)
{
}

void ICNBase::receiveICNPacket(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId) {
    std::string name(icnPacket->getIcnName());

    EV_INFO << "############### HELLO I RECEIVED A PACKET ###############" << endl;

    if (icnPacket->getPacketType() == ICNPacketType::PUBLISH) {
        EV_INFO << "Forwarding packet with name '" << name << "' arrived on interface with id " << arrivalInterfaceId << "..."<< endl;

        // get the interfaces we need to forward this message to
        std::vector<int> forwardTo = mICNRouterModule->getInterestedInterfaces(name);

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
                if (interfaceId == ICNRouter::LOCAL_APPLICATION_ID) {
                    // notify local application
                    mICNSubscriberModule->receiveData(icnPacket);
                    EV_INFO << "Packet with name '" << name << "' forwarded to local app." << endl;
                } else if (mForwarding) {
                    mTransportInterface->sendICNPacket(packet->dup(), interfaceId);
                    EV_INFO << "Packet with name '" << name << "' forwarded to interface with id " << interfaceId << endl;
                }
            }

            delete packet;
        } else {
            // the list was empty that means that there is no interest for this packet here
            EV_INFO << "Packet with name '" << name << "' is not interesting for me!" << endl;
            // TODO: in case we are a forwarder we might want to tell some1 that we are not interested anymore
        }

    // in case we receive subscribe packet and we have forwarding enabled we will store the subscription
    // for later forwarding (see publish packet section above)
    } else if (icnPacket->getPacketType() == ICNPacketType::SUBSCRIBE && mForwarding) {
        bool routingTableChanged = mICNRouterModule->addSubscription(arrivalInterfaceId, name);
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
            EV_INFO << "Not forwarding the subscription becuse we already had one with the same name!" << endl;
        }

    } else if (icnPacket->getPacketType() == ICNPacketType::ADVERTISEMENT) {

        // message we received was an AP advertisement
        // tell subscriber about it
        mICNSubscriberModule->advertisementReceived();

    } else if (icnPacket->getPacketType() == ICNPacketType::UNSUBSCRIBE) {
        throw cRuntimeError("Received local request, response or unsubscribe which cant be handled yet!");
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
        mSelfMessage = new cMessage("sendTimer");
        mInterfaceTableModule = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        mForwarding = par("icnForwarding").boolValue();
        mICNRouterModule = getModuleFromPar<ICNRouter>(par("routerModule"), this);
        mICNSubscriberModule = getModuleFromPar<ICNSubscriber>(par("subscriberModule"), this);
        mTransportInterface->setCallback(this);
    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        mTransportInterface->initialize(mInterfaceTableModule, gate("socketOut"));
    }
}

void ICNBase::finish()
{
    ApplicationBase::finish();
}

void ICNBase::processStart()
{
}

void ICNBase::handleMessageWhenUp(cMessage* msg)
{

    if (msg->isSelfMessage()) {
        ASSERT(msg == mSelfMessage);
        switch (mSelfMessage->getKind()) {
            case START:
                processStart();
                break;
            default:
                throw cRuntimeError("Invalid kind %d in self message", (int)mSelfMessage->getKind());
        }
    } else if(msg->arrivedOn(PUBLISHER_GATE_NAME.c_str())) {
        // we received a message from the publisher

        // extract name of publication
        Packet* packet = check_and_cast<Packet*>(msg);
        const Ptr<const ICNPacket> icnPacket = packet->peekAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
        std::string publicationName = icnPacket->getIcnName();

        Packet* copy = packet->dup();
        std::vector<int> interestedInterfaces = mICNRouterModule->getInterestedInterfaces(publicationName);
        for (int& interfaceId: interestedInterfaces) {
            if (interfaceId != ICNRouter::LOCAL_APPLICATION_ID) {
                mTransportInterface->sendICNPacket(copy, interfaceId);
                copy = packet->dup();
            }
        }

        delete packet;

    } else if (msg->arrivedOn(SUBSCRIBER_GATE_NAME_IN.c_str())) {
        // we received a subscription from subscriber (local application)
        // we need to investigate the contents
        Packet* packet = check_and_cast<Packet*>(msg);
        const Ptr<const ICNPacket> icnPacket = packet->peekAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
        std::string subscriptionName = icnPacket->getIcnName();
        mICNRouterModule->addSubscription(ICNRouter::LOCAL_APPLICATION_ID, subscriptionName);

        Packet* copy = packet->dup();
        // next we need to spread the word about our amazing subscription on all available interfaces
        for (int interfacePosition = 0; interfacePosition < mInterfaceTableModule->getNumInterfaces(); interfacePosition++) {
            int interfaceId = mInterfaceTableModule->getInterface(interfacePosition)->getInterfaceId();
            mTransportInterface->sendICNPacket(copy, interfaceId);
            copy = packet->dup();
        }
        delete packet;

    } else if (msg->arrivedOn(ADVERTISER_GATE_NAME_IN.c_str())) {
        // received message from advertiser

        Packet* packet = check_and_cast<Packet*>(msg);

        Packet* copy = packet->dup();
        // we need to tell everyone we can reach about it
        for (int interfacePosition = 0; interfacePosition < mInterfaceTableModule->getNumInterfaces(); interfacePosition++) {
            int interfaceId = mInterfaceTableModule->getInterface(interfacePosition)->getInterfaceId();
            mTransportInterface->sendICNPacket(copy, interfaceId);
            copy = packet->dup();
        }

        delete packet;
    } else {
        mTransportInterface->processMessage(msg);
    }
}

void ICNBase::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();
}

void ICNBase::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t start = simTime();
    mSelfMessage->setKind(START);
    scheduleAt(start, mSelfMessage);
}

void ICNBase::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(mSelfMessage);
    mTransportInterface->closeSocket();
    delayActiveOperationFinish(2);
}

void ICNBase::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(mSelfMessage);
    mTransportInterface->destroySocket();
}


} /* namespace inet */
