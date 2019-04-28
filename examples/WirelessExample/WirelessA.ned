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

network WirelessA
{
    parameters:
        @display("bgb=650,500;bgg=100,1,grey95");
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
        radioMedium: <default("UnitDiskRadioMedium")> like IRadioMedium {
            @display("p=580,275");
        }
        hostA: ICNAccessPoint {
            parameters:
                @display("p=345.74402,254.016");
                numLoInterfaces = 0;
        }
        hostB: ICNSubscriberHost {
            parameters:
                @display("p=419.328,363.888");
                numLoInterfaces = 0;
        }
        hostC: ICNSubscriberHost {
            parameters:
                @display("p=282.24002,363.888");
                numLoInterfaces = 0;
        }
        hostD: ICNPublisherHost {
            parameters:
                @display("p=354.816,69.552");
                numLoInterfaces = 0;
                numWlanInterfaces = 0;
        }
        visualizer: IntegratedCanvasVisualizer {
            parameters:
                @display("p=98.8575,209.81999");
        }
    connections:
        hostA.pppg++ <--> C <--> hostD.pppg++;
}
