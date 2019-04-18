//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/applications/udpapp/UdpTestApp_m.h"
#include "inet/applications/udpapp/UdpTestApp.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"

namespace inet {

Define_Module(UdpTestApp);

UdpTestApp::~UdpTestApp()
{
    cancelAndDelete(selfMsg);
}

void UdpTestApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        port = 5000;
        mDestinationContentBasedAddress = par("destinationContentBasedAddress").stdstringValue();
        mLocalContentBasedAddress = par("localContentBasedAddress").stdstringValue();
        selfMsg = new cMessage("sendTimer");
        interfaceTableModule = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
    }
}

void UdpTestApp::finish()
{
    ApplicationBase::finish();
}

void UdpTestApp::sendPacket()
{
    // -----
    // test area

    EV_DEBUG << "Found " << interfaceTableModule->getNumInterfaces() << " interfaces:" << endl;
    for (int interface = 0; interface < interfaceTableModule->getNumInterfaces(); ++interface) {
        InterfaceEntry* interfaceEntry = interfaceTableModule->getInterface(interface);
        EV_DEBUG << "Postition: " << interface << " Name: " << interfaceEntry->getInterfaceName() << " InterfaceID: " << interfaceEntry->getInterfaceId() << endl;
    }

    // -----

    // create and fill stuff into packet
    Packet *packet = new Packet("ContentBasedData");
    const auto& payload = makeShared<UdpTestAppPacket>();
    payload->setChunkLength(B(1000));
    payload->setName(mDestinationContentBasedAddress.c_str());
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    packet->insertAtBack(payload);
    // resolve address and send data
    L3Address destinationAddress;
    L3AddressResolver().tryResolve("224.0.0.1", destinationAddress);
    socket.sendTo(packet, destinationAddress, port);
}

void UdpTestApp::processStart()
{
    socket.setOutputGate(gate("socketOut"));
    // this specifies on which interface multicasts should be sent out
    // TODO: at a later point we need to do this dynamically
    socket.setMulticastOutputInterface(101);
    // this is important to prevent packets from arriving on loopback when sending
    socket.setMulticastLoop(false);
    // bind socket to port to only receive udp packets directed at my protocol
    socket.bind(port);
    L3Address address;
    L3AddressResolver().tryResolve("224.0.0.1", address);
    // need to join the multicast group to receive the packets myself
    socket.joinMulticastGroup(address);
    // when a packet is received this class is called
    socket.setCallback(this);

    bool sender = par("publisher");
    if (sender) {
        selfMsg->setKind(SEND);
        processSend();
    }

}

void UdpTestApp::processSend()
{
    sendPacket();
    simtime_t d = simTime() + par("sendInterval");
    selfMsg->setKind(SEND);
    scheduleAt(d, selfMsg);

}

void UdpTestApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        ASSERT(msg == selfMsg);
        switch (selfMsg->getKind()) {
            case START:
                processStart();
                break;
            case SEND:
                processSend();
                break;
            default:
                throw cRuntimeError("Invalid kind %d in self message", (int)selfMsg->getKind());
        }
    }
    else
        socket.processMessage(msg);
}

void UdpTestApp::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // process incoming packet
    processPacket(packet);
}

void UdpTestApp::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void UdpTestApp::socketClosed(UdpSocket *socket)
{
    startActiveOperationExtraTimeOrFinish(-1);
}

void UdpTestApp::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();
}

void UdpTestApp::processPacket(Packet *pk)
{
    auto udpTestAppHeader = pk->popAtFront<UdpTestAppPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
    std::string name(udpTestAppHeader->getName());
    if (name == mLocalContentBasedAddress) {
        EV_INFO << "########## Received data interesting for me with name: " <<  name << " ##########"<< endl;
    } else {
        EV_INFO << "########## Received data not interesting for me with name: " <<  name << " ##########"<< endl;
    }
    delete pk;
}

void UdpTestApp::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t start = simTime();
    selfMsg->setKind(START);
    scheduleAt(start, selfMsg);
}

void UdpTestApp::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.close();
    delayActiveOperationFinish(2);
}

void UdpTestApp::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.destroy();         //TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
}

} // namespace inet

