#include "ICNBase.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/TimeTag_m.h"

namespace inet {

Define_Module(ICNBase);

ICNBase::ICNBase()
: mLocalContentBasedAddress("")
, mSelfMessage(nullptr)
, mInterfaceTableModule(nullptr)
, mTransportInterface(new ICNUdpInterface())
{
}

void ICNBase::receiveICNPacket(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId) {
    std::string name(icnPacket->getName());
    if (name == mLocalContentBasedAddress) {
        EV_INFO << "########## Received data interesting for me with name: " <<  name << " ##########"<< endl;
    } else {
        EV_INFO << "########## Received data not interesting for me with name: " <<  name << " ##########"<< endl;
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
        mLocalContentBasedAddress = par("localContentBasedAddress").stdstringValue();
        mSelfMessage = new cMessage("sendTimer");
        mInterfaceTableModule = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        mTransportInterface->setCallback(this);
    }
}

void ICNBase::finish()
{
    ApplicationBase::finish();
}

void ICNBase::processStart()
{
    mTransportInterface->initialize(mInterfaceTableModule, gate("socketOut"));
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
        mTransportInterface->sendICNPacket(check_and_cast<Packet*>(msg), 101);
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
