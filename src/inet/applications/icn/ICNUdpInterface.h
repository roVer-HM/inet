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

#ifndef INET_APPLICATIONS_ICN_ICNUdpInterface_H_
#define INET_APPLICATIONS_ICN_ICNUdpInterface_H_

#include <vector>

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/applications/icn/ICNTransportInterface.h"

namespace inet {

class INET_API ICNUdpInterface : public UdpSocket::ICallback, public ICNTransportInterface
{
public:
    /**
     * Constructor.
     */
    ICNUdpInterface() = default;

    /**
     * Destructor.
     */
    ~ICNUdpInterface() = default;


    // ICNTransportInterface
    void sendICNPacket(Packet* icnPacket, int interfaceId) override;
    void initialize(IInterfaceTable* interfaceTableModule, cGate* gate) override;
    void processMessage(cMessage* message) override;
    virtual void closeSocket(void) override;
    virtual void destroySocket(void) override;

  protected:
    /**
     * Find the source interface for the given packet. In case
     * there is no InterfaceInd tag or the specified interface
     * does not exist this will return a nullptr.
     *
     * @param packet The packet to examine.
     *
     * @return InterfaceEntry* (nullptr if not found)
     */
    const InterfaceEntry* getSourceInterface(Packet* packet);


    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;


private:

    /**
    * The transport socket that this interface will use.
    */
    UdpSocket mSocket;


    IInterfaceTable* mInterfaceTableModule = nullptr;

    void processPacket(Packet* packet);

};

} // namespace inet

#endif // ifndef INET_APPLICATIONS_ICN_ICNUdpInterface_H_

