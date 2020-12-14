:orphan:

Ethernet Preemption
===================

- Higher priority frames can interrupt transmission of lower priority frames in the ethernet MAC layer.
- This guaranties low latency for high priority frames
- After the high priority frame transmission is finished, the other fragment of the low priority frame is sent

- Actually there are two mac layers (PreemptableMac and ExpressMac)
- This needs packet streaming as well

- Compare three cases:
	- baseline: no preemption or priority queue
	- priority queue in the mac
	- preemption (actually, there are two different queues)

Goals
-----

Preemption enables higher priority frames to interrupt the transmission of lower priority frames in the Ethernet MAC layer. This guaranties low latency for time-critical high priority frames.

This showcase demonstrates preemption and examines the latency reduction due to preemption.

The Model
---------

In time-sensitive networking applications, Ethernet preemption can significantly reduce latency. When a high priority frame becomes available during the transmission of a low priority frame, the Ethernet MAC can interrupt the transmission. After sending the high priority frame, the MAC continues the transmission of the low priority frame from where it left off, sending the low priority frame in two (or more) fragments.

Preemption is a feature of the composable Ethernet model in INET. requires packet streaming so that packet transmission is represented as a stream so that it can be interrupted. The :ned:`EthernetPreemptingMacLayer` contains two MAC sub-layers, a preemptable (:ned:`EthernetStreamingMacLayer`) and an express mac layer (:ned:`EthernetFragmentingMacLayer`), each with its own queue for frames:

.. figure:: media/mac.png
   :align: center

Preemption also requires the :ned:`EthernetPreemptingPhyLayer`.

.. Preemption requires packet streaming so that packet transmission is represented as a stream so that it can be interrupted.

The simulation uses the following network:

.. figure:: media/network.png
   :align: center

It contains just two hosts connected with 100Mbps Ethernet; and also PcapRecorder to record Pcap traces.

There are three configurations, for the following three cases:

- **Default**: The baseline (not using preemption or priority queues)
- **PriorityQueue**: Use priority queue in the Ethernet MAC; should have better delay for high priority frames
- **Preemption**: Use preemption for high priority frames; should have better delay than the priority queue case, because ongoing low priority frames can be interrupted

In the general configuration, the hosts are configured to use the layered/composable ethernet model:

.. literalinclude:: ../omnetpp.ini
   :start-at: encap.typename
   :end-at: LayeredEthernetInterface
   :language: ini

Also, we want to record a PCAP trace, so we can examine the traffic in Wireshark. We enable PCAP recording, and set the PCAP recorder to dump the Ethernet PHY frames; **TODO** CRC/FCS?

.. literalinclude:: ../omnetpp.ini
   :start-at: recordPcap
   :end-at: dumpProtocols
   :language: ini

One of the hosts, ``host1`` is configured to send UDP packets to ``host2``:

.. literalinclude:: ../omnetpp.ini
   :start-at: numApps
   :end-at: app[1].io.destPort
   :language: ini

There are two :ned:`UdpApp`'s in ``host1``, one generating background traffic and the other time-sensitive traffic.
:ned:`UdpApp` is built using generic protocol components. We configure the app's ``outbound`` module to be a PacketTagger, so we can tag packets with a VLAN ID requests, to put them in different priority categories.

We set up high background traffic (96 Mbps) and lower time-sensitive traffic (9.6 Mbps); both send 1200B packets:

.. literalinclude:: ../omnetpp.ini
   :start-at: app[0].source.packetLength
   :end-at: app[1].source.productionInterval
   :language: ini

In the **Default** configuration, no preemption or priority queue is used; the configuration just limits the EthernetMac's queue to decrease the measured end-to-end delay:

.. literalinclude:: ../omnetpp.ini
   :start-at: Config Default
   :end-at: dropperClass
   :language: ini

In the **PriorityQueue** configuration, we change the queue type in the Mac layer from the default PacketQueue to PriorityQueue:

.. literalinclude:: ../omnetpp.ini
   :start-at: Config PriorityQueue
   :end-at: classifierClass
   :language: ini

The priority queue needs two internal queues, for the two traffic categories; we also limit the internal queues, disable the buffer, and configure a packet dropper function. We configure the priority queue's classifier to classify packets based on the VLAN ID request.

In the **Preemption** configuration, we replace the :ned:`EthernetMacLayer` and :ned:`EthernetPhyLayer` modules default in :ned:`LayeredEthernetInterface` with :ned:`EthernetPreemptingMacLayer` and :ned:`EthernetPreemptingPhyLayer`; the latter support Ethernet preemption:

.. literalinclude:: ../omnetpp.ini
   :start-at: Config Preemption
   :end-at: dropperClass
   :language: ini

We also limit the queue, and configure a packet dropper function.

Results
-------

Here is a video of the preemption behavior:

.. video:: media/preemption.mp4
   :width: 100%
	 :align: center

.. figure:: media/delay.png
   :align: center
