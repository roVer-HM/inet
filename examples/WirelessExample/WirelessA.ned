//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

package inet.examples.WirelessExample;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.INetworkNode;
import inet.node.inet.StandardHost;
import inet.node.inet.WirelessHost;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import ned.DatarateChannel;
import inet.node.icn.ICNAccessPoint;
import inet.node.icn.ICNPublisherHost;
import inet.node.icn.ICNSubscriberHost;
import inet.node.icn.ICNAdvancedSubscriberHost;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.physicallayer.unitdisk.UnitDiskRadioMedium;

network WirelessA
{
    parameters:
        @display("bgb=1000,1000;bgg=100,1,grey95");
    types:
        channel C extends DatarateChannel
        {
            datarate = 100Mbps;
            delay = 0.1us;
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=580,200");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=645.12,276.19202");
        }
        hostA: ICNAccessPoint {
            parameters:
                @display("p=500.00,250.00");
        }
        // 160 meters away from ap
        hostB: ICNAdvancedSubscriberHost {
            parameters:
                @display("p=340,250.00");
        }
        // to far away from access point but close enough to receive from hostB
        hostE: ICNAdvancedSubscriberHost {
            parameters:
                @display("p=290.00,250.00");
        }
        hostC: ICNAdvancedSubscriberHost {
            parameters:
                @display("p=500.00,300.00");
        }
        hostD: ICNPublisherHost {
            parameters:
                @display("p=530.208,77.616005");
                numWlanInterfaces = 0;
        }
    connections:
        hostA.pppg++ <--> C <--> hostD.pppg++;
}
