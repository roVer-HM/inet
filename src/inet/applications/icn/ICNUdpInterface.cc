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

#include "inet/applications/icn/ICNUdpInterface.h"

#include "inet/applications/icn/ICNPacket_m.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/linklayer/common/InterfaceTag_m.h"

namespace inet {

void ICNUdpInterface::initialize(IInterfaceTable* interfaceTableModule, cGate* gate) {
    mInterfaceTableModule = interfaceTableModule;

    mSocket.setOutputGate(gate);
    // this is important to prevent packets from arriving on loopback when sending
    mSocket.setMulticastLoop(false);
    // bind socket to port to only receive udp packets directed at my protocol
    mSocket.bind(ICN_PORT);
    L3Address address;
    L3AddressResolver().tryResolve(ICN_MULTICAST_ADDRESS.c_str(), address);
    // need to join the multicast group to receive the packets myself
    mSocket.joinMulticastGroup(address);
    // when a packet is received this class is called
    mSocket.setCallback(this);

}

void ICNUdpInterface::processMessage(cMessage* message) {
    mSocket.processMessage(message);
}

void ICNUdpInterface::closeSocket(void) {
    mSocket.close();
}

void ICNUdpInterface::destroySocket(void) {
    mSocket.destroy();
}

void ICNUdpInterface::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // process incoming packet
    processPacket(packet);
}

void ICNUdpInterface::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void ICNUdpInterface::socketClosed(UdpSocket *socket)
{
    getCallback()->interfaceClosed();
}

const InterfaceEntry* ICNUdpInterface::getSourceInterface(Packet* packet)
{
    auto tag = packet->findTag<InterfaceInd>();
    return tag != nullptr ? mInterfaceTableModule->getInterfaceById(tag->getInterfaceId()) : nullptr;
}

void ICNUdpInterface::processPacket(Packet* packet)
{
    // get the interface id on that the packet arrived on
    const InterfaceEntry* interface = getSourceInterface(packet);

    if (interface == nullptr) {
        EV_DEBUG << "We received a packet on an interface that does not exist!" << endl;
    } else {

        EV_DEBUG << "A packet arrived on interface with id " << interface->getInterfaceId() << " and name " << interface->getInterfaceName() << endl;

        // interface found: we can now investigate the actual content of the packet
        const Ptr<const ICNPacket> icnPacketHeader = packet->popAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);

        getCallback()->receiveICNPacket(icnPacketHeader, interface->getInterfaceId());
    }

    delete packet;
}

void ICNUdpInterface::sendICNPacket(Packet* icnPacket, int interfaceId) {
    // resolve address and send data
    L3Address destinationAddress;
    L3AddressResolver().tryResolve(ICN_MULTICAST_ADDRESS.c_str(), destinationAddress);

    mSocket.setMulticastOutputInterface(interfaceId);
    mSocket.sendTo(icnPacket, destinationAddress, ICN_PORT);
}

} // namespace inet

