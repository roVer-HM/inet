Ethernet Cut-Through Switching
==============================

Goals
-----

Cut-through switching can reduce switching delay of Ethernet frames by immediately forwarding an Ethernet frame after the header is received and the switch knows which outgoing interface to send the frame on (as opposed to store-and-forward switching, in which the whole frame is received and then forwarded).

This showcase demonstrates cut-through switching, and compares it to store-and-forward switching in terms of delay.

| INET version: ``4.3``
| Source files location: `inet/showcases/ethernet/cutthrough <https://github.com/inet-framework/inet-showcases/tree/master/ethernet/cutthrough>`__

The Model
---------

Cut-through switching reduces switching delay, but skips the FCS check, as the FCS is at the end of the Ethernet frame; the FCS check is performed in at destination host. The delay reduction is more substantial if the packet goes through multiple switches (as one packet transmission duration can be saved at each switch).

Cut-through switching makes use of intra-node  packet streaming in INET's modular Ethernet model.
Packet streaming is required because the frame needs to be processed as a stream (as opposed to as a whole packet) in order for the switch to be able to start forwarding it before the whole packet is received.

.. note:: The default is store-and-forward behavior in hosts such as :ned:`StandardHost`.

The example simulation contains two :ned:`StandardHost` nodes connected by two :ned:`EthernetSwitch`' nodes (all connections are 1 Gbps):

.. image:: media/Network.png
   :align: center
   :width: 100%

In the simulation, host1 sends 1000-Byte UDP packets to host2, with a mean arrival time of 100ms, and X ms jitter. There are two configurations in omnetpp.ini, ``StoreAndForward`` and ``Cutthrough`` (only differing in the use of cut-through switching).

The encapsulation type :ned:`OmittedEthernetEncapsulation` is there for backward compatibility purposes;
it's a dummy encapsulation module required by the new layered Ethernet model so that it is a drop-in replacement for the old one).

Also, the link speed is specified:

.. literalinclude:: ../omnetpp.ini
   :start-at: bitrate
   :end-at: bitrate
   :language: ini

Here are the two configurations:

.. literalinclude:: ../omnetpp.ini
   :start-at: StoreAndForward
   :end-at: phyLayer
   :language: ini

The cut-through interface in the switches support packet streaming by default; the :ned:`EthernetStreamingPhyLayer` in the hosts support packet streaming as well.

Results
-------

The following videos store-and-forward and cut-through behavior in Qtenv, respectively:

.. video:: media/storeandforward.mp4
   :width: 100%
   :align: center

.. video:: media/cutthrough1.mp4
   :width: 100%
   :align: center

The following sequence chart excerpt shows a packet sent from host1 to host2 via the switches, for store-and-forward and cut-through, respectively (the timeline is linear):

.. image:: media/storeandforwardseq2.png
   :align: center
   :width: 100%

.. image:: media/seqchart2.png
   :align: center
   :width: 100%

We compared the end-to-end delay of the UDP packets in the case of store-and-forward switching vs cut-through switching:

.. image:: media/delay.png
   :align: center
   :width: 100%

We can verify that result analytically. In case of store-and-forward, the end-to-end duration is ``3 * (transmission time + propagation time)``, around 25.296 ms. In the case of cut-through, the duration is ``1 * transmission time + 3 propagation time + 2 * cut-through delay``, around 8.432 ms.

Sources: :download:`omnetpp.ini <../omnetpp.ini>`, :download:`EthernetCutthroughShowcase.ned <../EthernetCutthroughShowcase.ned>`

Discussion
----------

Use `this <https://github.com/inet-framework/inet-showcases/issues/TODO>`__ page in the GitHub issue tracker for commenting on this showcase.

.. 1054B; 8.432us; 25.296+propagation time

  (1000 + 8 + 20 + 18 + 8) * 8 / 1E+9 * 3 / 1E-6
  (1000 + 8 + 20 + 18 + 8) * 8 / 1E+9 / 1E-6 + 22 / 1E+9 / 1E-6 * 2
