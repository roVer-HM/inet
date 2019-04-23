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
        hostA: WirelessHost {
            parameters:
                @display("p=345.74402,254.016");
                numLoInterfaces = 0;
        }
        hostB: <default("WirelessHost")> like INetworkNode {
            parameters:
                @display("p=432.432,375.984");
                numLoInterfaces = 0;
        }
        hostC: <default("WirelessHost")> like INetworkNode {
            parameters:
                @display("p=262.08002,375.984");
                numLoInterfaces = 0;
        }
        hostD: StandardHost {
            parameters:
                @display("p=354.816,69.552");
                numLoInterfaces = 0;
        }
        visualizer: IntegratedCanvasVisualizer {
            parameters:
                @display("p=98.8575,209.81999");
        }
    connections:
        hostA.pppg++ <--> C <--> hostD.pppg++;
}
