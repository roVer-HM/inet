#include "ICN.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/TimeTag_m.h"

namespace inet {

Define_Module(ICN);

ICN::ICN()
: mDestinationContentBasedAddress("")
, mLocalContentBasedAddress("")
, mSelfMessage(nullptr)
, mInterfaceTableModule(nullptr)
, mTransportInterface(new ICNUdpInterface())
{
}

void ICN::receiveICNPacket(const inet::Ptr<const ICNPacket>& icnPacket, int arrivalInterfaceId) {
    std::string name(icnPacket->getName());
    if (name == mLocalContentBasedAddress) {
        EV_INFO << "########## Received data interesting for me with name: " <<  name << " ##########"<< endl;
    } else {
        EV_INFO << "########## Received data not interesting for me with name: " <<  name << " ##########"<< endl;
    }
}

void ICN::interfaceClosed(void) {
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(-1);
}

void ICN::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        mDestinationContentBasedAddress = par("destinationContentBasedAddress").stdstringValue();
        mLocalContentBasedAddress = par("localContentBasedAddress").stdstringValue();
        mSelfMessage = new cMessage("sendTimer");
        mInterfaceTableModule = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        mTransportInterface->setCallback(this);
    }
}

void ICN::finish()
{
    ApplicationBase::finish();
}

void ICN::sendPacket()
{
    // -----
    // test area

    EV_DEBUG << "Found " << mInterfaceTableModule->getNumInterfaces() << " interfaces:" << endl;
    for (int interface = 0; interface < mInterfaceTableModule->getNumInterfaces(); ++interface) {
        InterfaceEntry* interfaceEntry = mInterfaceTableModule->getInterface(interface);
        EV_DEBUG << "Position: " << interface << " Name: " << interfaceEntry->getInterfaceName() << " InterfaceID: " << interfaceEntry->getInterfaceId() << endl;
    }

    // -----

    // create and fill stuff into packet
    const auto& payload = makeShared<ICNPacket>();
    payload->setChunkLength(B(1000));
    payload->setName(mDestinationContentBasedAddress.c_str());
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

    // first to interface with id 101 (hardcoded wlan0):
    mTransportInterface->sendICNPacket(payload, 101);
}

void ICN::processStart()
{
    mTransportInterface->initialize(mInterfaceTableModule, gate("socketOut"));
    bool sender = par("publisher");
    if (sender) {
        mSelfMessage->setKind(SEND);
        processSend();
    }
}

void ICN::processSend()
{
    sendPacket();
    simtime_t timestamp = simTime() + par("sendInterval");
    mSelfMessage->setKind(SEND);
    scheduleAt(timestamp, mSelfMessage);

}

void ICN::handleMessageWhenUp(cMessage* msg)
{
    if (msg->isSelfMessage()) {
        ASSERT(msg == mSelfMessage);
        switch (mSelfMessage->getKind()) {
            case START:
                processStart();
                break;
            case SEND:
                processSend();
                break;
            default:
                throw cRuntimeError("Invalid kind %d in self message", (int)mSelfMessage->getKind());
        }
    }
    else
        mTransportInterface->processMessage(msg);
}

void ICN::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();
}

void ICN::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t start = simTime();
    mSelfMessage->setKind(START);
    scheduleAt(start, mSelfMessage);
}

void ICN::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(mSelfMessage);
    mTransportInterface->closeSocket();
    delayActiveOperationFinish(2);
}

void ICN::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(mSelfMessage);
    mTransportInterface->destroySocket();
}


} /* namespace inet */
