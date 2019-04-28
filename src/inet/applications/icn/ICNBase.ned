//
// Copyright (C) 2004 Andras Varga
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
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

package inet.applications.icn;

import inet.applications.contract.IApp;

simple ICNBase
{
    parameters:
        bool icnForwarding;
        string interfaceTableModule;   // The path to the InterfaceTable module
        string routerModule;
        bool hasSubscriber;
        bool hasPublisher;
        @display("i=block/app");
        @lifecycleSupport;

    gates:
        input socketIn @labels(UdpControlInfo/up);
        output socketOut @labels(UdpControlInfo/down);
        // gate for all messages coming from other modules which sit below ICN
        input messageInterface[];
        
        // a gate to deliver icn packets that this node subscribed to
        output subscribedPackets;
        
        // a gate to notify modules about received heartbeats
        output receivedHeartbeats;
        
        // a gate to notify modules about received nearby subscriptions ( for future use)
        //output receivedSubscriptions;
        
}

