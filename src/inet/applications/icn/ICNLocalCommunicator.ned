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

package inet.applications.icn;

// The local communicator has 3 jobs:
// 		- his first job is to rebroadcast every publication he receives once (flooding)
// 		- his second job is to answer to local request with cached content
// 		- if he is not associated to an access point he will send requests himself
simple ICNLocalCommunicator  {
    parameters:
        // whether to do flooding
        bool doFlooding = default(true);
        // the delay between two (if smaller or equal zero only one request will be sent)
        int requestDelay @unit(s);
        // the size of the request
        int requestSize @unit(B);
        // the name of the data to be requested
        string requestedDataName;
        // the size of a publication (response to request)
        int publicationSize @unit(B);
        
    gates:
        // subscription messages going to icn base (these are broadcasted on the local interface)
        output publications;
        
        // request going to icn base
        output requests;
        
        // messages coming in from icn base:
        // 	- publications
        // 	- requests
        input messageInterface[];
}