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
        bool icnForwarding = default(false);
        string interfaceTableModule;   // The path to the InterfaceTable module
        string routerModule;
        string subscriberModule;
        @display("i=block/app");
        @lifecycleSupport;

    gates:
        input socketIn @labels(UdpControlInfo/up);
        output socketOut @labels(UdpControlInfo/down);
        // gate for messages from publisher module
        input icnPublisherIn;
        // gate for messages that contain an icnSubscription
        input icnSubscriberIn;
        // gate for messages that contain an advertisement
        input icnAdvertiserIn;
}

