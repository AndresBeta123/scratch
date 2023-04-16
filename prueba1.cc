//Link state routing for Wired Networks, OLSR 
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/random-variable-stream.h"

 using namespace ns3;
  
 NS_LOG_COMPONENT_DEFINE ("WifiSimpleAdhocGrid");

 void ReceivePacket (Ptr<Socket> socket)
 {
   while (socket->Recv ())
     {
       NS_LOG_UNCOND ("Received one packet!");
     }
 }

 static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                              uint32_t pktCount, Time pktInterval )
 {
   if (pktCount > 0)
     {
       socket->Send (Create<Packet> (pktSize));
       Simulator::Schedule (pktInterval, &GenerateTraffic,
                            socket, pktSize,pktCount - 1, pktInterval);
     }
   else
     {
       socket->Close ();
     }
 }

//Modificamos la función GenerateTraffic() para que genere paquetes con tiempos de llegada aleatorios

/* static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
                              uint32_t pktCount, Time pktInterval)
 {
  
  
   if (pktCount > 0)
     {
       socket->Send (Create<Packet> (pktSize));
       Time tNextPacket = Seconds(randomArrival.GetValue());
       Simulator::Schedule (tNextPacket, &GenerateTraffic,
                            socket, pktSize,pktCount - 1, pktInterval );
     }
   else
     {
       socket->Close ();
     }
 } */
  
 int main (int argc, char *argv[])
 {
   std::string phyMode ("DsssRate1Mbps");
   double distance = 10;  // m
   uint32_t packetSize = 1000; // bytes
   uint32_t numPackets = 1;
   uint32_t numNodes = 6;  // by default, 5x5
   uint32_t sinkNode = 1;
   uint32_t sourceNode = 0;
   double interval = 1.0; //Seconds
   bool verbose = false;
   bool tracing = false;

  // Definimos una instancia de la clase PoissonRandomVariable y establecemos su valor medio,
  //el valor medio se establece a la inversa del intervalo inter-packet, ya que esto corresponde a la tasa de llegada media para una distribución de Poisson/*PoissonRandomVariable randomArrival;
  //randomArrival.SetAttribute ("Mean", DoubleValue (1.0 / interval));


   CommandLine cmd;
   cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
   cmd.AddValue ("distance", "distance (m)", distance);
   cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
   cmd.AddValue ("numPackets", "number of packets generated", numPackets);
   cmd.AddValue ("interval", "interval (seconds) between packets", interval);
   cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
   cmd.AddValue ("tracing", "turn on ascii and pcap tracing", tracing);
   cmd.AddValue ("numNodes", "number of nodes", numNodes);
   cmd.AddValue ("sinkNode", "Receiver node number", sinkNode);
   cmd.AddValue ("sourceNode", "Sender node number", sourceNode);
   cmd.Parse (argc, argv);
   // Convert to time object
   Time interPacketInterval = Seconds (interval);
  
   // Fix non-unicast data rate to be the same as that of unicast
   Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",
                       StringValue (phyMode));
  
  NodeContainer heads;
  heads.Create(2);
  NodeContainer Cluster1;
  Cluster1.Create(2);
  Cluster1.Add(heads.Get(0));
  NodeContainer Cluster2;
  Cluster2.Create(2);
  Cluster2.Add(heads.Get(1));
  
   
   // The below set of helpers will help us to put together the wifi NICs we want
   WifiHelper wifi;
   if (verbose)
     {
       wifi.EnableLogComponents ();  // Turn on all Wifi logging
     }
  
   YansWifiPhyHelper wifiPhy1,wifiPhy2,wifiPhy3;
   // set it to zero; otherwise, gain will be added
   wifiPhy1.Set ("RxGain", DoubleValue (-10) );
   wifiPhy2.Set ("RxGain", DoubleValue (-10) );
   wifiPhy3.Set ("RxGain", DoubleValue (-10) );

   YansWifiChannelHelper wifiChannel;
   wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
   wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
   wifiPhy1.SetChannel (wifiChannel.Create ());
   wifiPhy2.SetChannel (wifiChannel.Create ());
   wifiPhy3.SetChannel (wifiChannel.Create ());
  
   // Add an upper mac and disable rate control
   WifiMacHelper wifiMac;
   wifi.SetStandard (WIFI_STANDARD_80211b);
   wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                 "DataMode",StringValue (phyMode),
                                 "ControlMode",StringValue (phyMode));
   // Set it to adhoc mode
   wifiMac.SetType ("ns3::AdhocWifiMac");
   NetDeviceContainer devices1 = wifi.Install (wifiPhy1, wifiMac, heads);
   NetDeviceContainer devices2 = wifi.Install (wifiPhy2, wifiMac, Cluster1);
   NetDeviceContainer devices3 = wifi.Install (wifiPhy3, wifiMac, Cluster2);
  
  
   MobilityHelper mobility;
   mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue (0.0),
                                  "MinY", DoubleValue (0.0),
                                  "DeltaX", DoubleValue (distance),
                                  "DeltaY", DoubleValue (distance),
                                  "GridWidth", UintegerValue (5),
                                  "LayoutType", StringValue ("RowFirst"));
   mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobility.Install (Cluster1);
   mobility.Install (Cluster2);
   // Enable OLSR
   OlsrHelper olsr;
   Ipv4StaticRoutingHelper staticRouting;
  
   Ipv4ListRoutingHelper list;
   list.Add (staticRouting, 0);
   list.Add (olsr, 10);
  
   InternetStackHelper internet;
   internet.SetRoutingHelper (list); // has effect on the next Install ()
   internet.Install (Cluster1);
   internet.Install (Cluster2);
  
   Ipv4AddressHelper ipv4;
   NS_LOG_INFO ("Assign IP Addresses.");
   ipv4.SetBase ("10.1.1.0", "255.255.255.0");
   Ipv4InterfaceContainer d1 = ipv4.Assign (devices1);
   ipv4.SetBase ("10.1.2.0", "255.255.255.0");
   Ipv4InterfaceContainer d2 = ipv4.Assign (devices2);
   ipv4.SetBase ("10.1.3.0", "255.255.255.0");
   Ipv4InterfaceContainer d3 = ipv4.Assign (devices3);
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
   Ptr<Socket> recvSink = Socket::CreateSocket (heads.Get(1), tid);
   InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
   recvSink->Bind (local);
   recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));
  
   Ptr<Socket> source = Socket::CreateSocket (Cluster2.Get(0), tid);
   InetSocketAddress remote = InetSocketAddress (d1.GetAddress (1, 0), 80);
   source->Connect (remote);
  
   if (tracing == true)
     {
       AsciiTraceHelper ascii;
       wifiPhy1.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
       wifiPhy1.EnablePcap ("wifi-simple-adhoc-grid", devices1);
       wifiPhy2.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
       wifiPhy2.EnablePcap ("wifi-simple-adhoc-grid", devices2);
       wifiPhy3.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
       wifiPhy3.EnablePcap ("wifi-simple-adhoc-grid", devices3);
       // Trace routing tables
       Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("wifi-simple-adhoc-grid.routes", std::ios::out);
       olsr.PrintRoutingTableAllEvery (Seconds (2), routingStream);
       Ptr<OutputStreamWrapper> neighborStream = Create<OutputStreamWrapper> ("wifi-simple-adhoc-grid.neighbors", std::ios::out);
       olsr.PrintNeighborCacheAllEvery (Seconds (2), neighborStream);
  


       // To do-- enable an IP-level trace that shows forwarding events only
     } 

    

  
    //Give OLSR time to converge-- 30 seconds perhaps
    Simulator::Schedule (Seconds (10.0), &GenerateTraffic,
                        source, packetSize, numPackets, interPacketInterval);
  
   // Output what we are doing
   NS_LOG_UNCOND ("Testing from node " << sourceNode << " to " << sinkNode << " with grid distance " << distance);
  
   Simulator::Stop (Seconds (50.0));
   Simulator::Run ();
   Simulator::Destroy ();
  
   return 0;
 }