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


//Escenario 3--------------------------------------------------------------------------

/*Este escenario tenía 10 clústeres con 3 nodos en la primera capa y 8 clústeres con 6
nodos en la segunda capa. */



//Primera capa--------------------
NodeContainer c1Cluster1;
c1Cluster1.Create(3);
NodeContainer c1Cluster2;
c1Cluster2.Create(3);
NodeContainer c1Cluster3;
c1Cluster3.Create(3);
NodeContainer c1Cluster4;
c1Cluster4.Create(3);
NodeContainer c1Cluster5;
c1Cluster5.Create(3);
NodeContainer c1Cluster6;
c1Cluster6.Create(3);
NodeContainer c1Cluster7;
c1Cluster7.Create(3);
NodeContainer c1Cluster8;
c1Cluster8.Create(3);
NodeContainer c1Cluster9;
c1Cluster9.Create(3);
NodeContainer c1Cluster10;
c1Cluster10.Create(3);
//Final Primera capa--------------------
//Segunda capa--------------------
NodeContainer c2Cluster1;
c2Cluster1.Add(c1Cluster1.Get(0));
c2Cluster1.Add(c1Cluster9.Get(0));
c2Cluster1.Create(4);
NodeContainer c2Cluster2;
c2Cluster2.Add(c1Cluster2.Get(0));
c2Cluster2.Add(c1Cluster10.Get(0));
c2Cluster2.Create(4);
NodeContainer c2Cluster3;
c2Cluster3.Add(c1Cluster3.Get(0));
c2Cluster3.Create(5);
NodeContainer c2Cluster4;
c2Cluster4.Add(c1Cluster4.Get(0));
c2Cluster4.Create(5);
NodeContainer c2Cluster5;
c2Cluster5.Add(c1Cluster5.Get(0));
c2Cluster5.Create(5);
NodeContainer c2Cluster6;
c2Cluster6.Add(c1Cluster6.Get(0));
c2Cluster6.Create(5);
NodeContainer c2Cluster7;
c2Cluster7.Add(c1Cluster7.Get(0));
c2Cluster7.Create(5);
NodeContainer c2Cluster8;
c2Cluster8.Add(c1Cluster8.Get(0));
c2Cluster8.Create(5);
//Final Segunda capa--------------------
//Tercera capa--------------------------
NodeContainer c3Cluster1;
c3Cluster1.Add(c2Cluster1.Get(2));
c3Cluster1.Add(c2Cluster2.Get(2));
c3Cluster1.Add(c2Cluster3.Get(1));
c3Cluster1.Add(c2Cluster4.Get(1));
c3Cluster1.Add(c2Cluster5.Get(1));
c3Cluster1.Add(c2Cluster6.Get(1));
c3Cluster1.Add(c2Cluster7.Get(1));
c3Cluster1.Add(c2Cluster8.Get(1));
//Final Tercera capa--------------------

  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  if (verbose)
    {
      wifi.EnableLogComponents ();  // Turn on all Wifi logging
    }

  YansWifiPhyHelper wifiPhy1, wifiPhy2, wifiPhy3, wifiPhy4, wifiPhy5, wifiPhy6, wifiPhy7, wifiPhy8, wifiPhy9, wifiPhy10, wifiPhy11, wifiPhy12, wifiPhy13, wifiPhy14, wifiPhy15, wifiPhy16, wifiPhy17, wifiPhy18, wifiPhy19;
  // set it to zero; otherwise, gain will be added
   wifiPhy1.Set ("RxGain", DoubleValue (-10) );
   wifiPhy2.Set ("RxGain", DoubleValue (-10) );
   wifiPhy3.Set ("RxGain", DoubleValue (-10) );
   wifiPhy4.Set ("RxGain", DoubleValue (-10) );
   wifiPhy5.Set ("RxGain", DoubleValue (-10) );
   wifiPhy6.Set ("RxGain", DoubleValue (-10) );
   wifiPhy7.Set ("RxGain", DoubleValue (-10) );
   wifiPhy8.Set ("RxGain", DoubleValue (-10) );
   wifiPhy9.Set ("RxGain", DoubleValue (-10) );
   wifiPhy10.Set ("RxGain", DoubleValue (-10) );
   //Fin capa 1, inicio capa 2
   wifiPhy11.Set ("RxGain", DoubleValue (-10) );
   wifiPhy12.Set ("RxGain", DoubleValue (-10) );
   wifiPhy13.Set ("RxGain", DoubleValue (-10) );
   wifiPhy14.Set ("RxGain", DoubleValue (-10) );
   wifiPhy15.Set ("RxGain", DoubleValue (-10) );
   wifiPhy16.Set ("RxGain", DoubleValue (-10) );
   wifiPhy17.Set ("RxGain", DoubleValue (-10) );
   wifiPhy18.Set ("RxGain", DoubleValue (-10) );
   //FIn capa 2, inicio capa 3
   wifiPhy19.Set ("RxGain", DoubleValue (-10) );
   
   


  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiPhy1.SetChannel (wifiChannel.Create ());
  wifiPhy2.SetChannel (wifiChannel.Create ());
  wifiPhy3.SetChannel (wifiChannel.Create ());
  wifiPhy4.SetChannel (wifiChannel.Create ());
  wifiPhy5.SetChannel (wifiChannel.Create ());
  wifiPhy6.SetChannel (wifiChannel.Create ());
  wifiPhy7.SetChannel (wifiChannel.Create ());
  wifiPhy8.SetChannel (wifiChannel.Create ());
  wifiPhy9.SetChannel (wifiChannel.Create ());
  wifiPhy10.SetChannel (wifiChannel.Create ());
   //Fin capa 1, inicio capa 2
  wifiPhy11.SetChannel (wifiChannel.Create ());
  wifiPhy12.SetChannel (wifiChannel.Create ());
  wifiPhy13.SetChannel (wifiChannel.Create ());
  wifiPhy14.SetChannel (wifiChannel.Create ());
  wifiPhy15.SetChannel (wifiChannel.Create ());
  wifiPhy16.SetChannel (wifiChannel.Create ());
  wifiPhy17.SetChannel (wifiChannel.Create ());
  wifiPhy18.SetChannel (wifiChannel.Create ());
   //FIn capa 2, inicio capa 3
  wifiPhy19.SetChannel (wifiChannel.Create ());


  // Add an upper mac and disable rate control
  WifiMacHelper wifiMac;
  wifi.SetStandard (WIFI_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");

  NetDeviceContainer devices1 = wifi.Install (wifiPhy1, wifiMac, c1Cluster1);
  NetDeviceContainer devices2 = wifi.Install (wifiPhy2, wifiMac, c1Cluster2);
  NetDeviceContainer devices3 = wifi.Install (wifiPhy3, wifiMac, c1Cluster3);
  NetDeviceContainer devices4 = wifi.Install (wifiPhy4, wifiMac, c1Cluster4);
  NetDeviceContainer devices5 = wifi.Install (wifiPhy5, wifiMac, c1Cluster5);
  NetDeviceContainer devices6 = wifi.Install (wifiPhy6, wifiMac, c1Cluster6);
  NetDeviceContainer devices7 = wifi.Install (wifiPhy7, wifiMac, c1Cluster7);
  NetDeviceContainer devices8 = wifi.Install (wifiPhy8, wifiMac, c1Cluster8);
  NetDeviceContainer devices9 = wifi.Install (wifiPhy9, wifiMac, c1Cluster9);
  NetDeviceContainer devices10 = wifi.Install (wifiPhy10, wifiMac, c1Cluster10);
  //Fin capa 1, inicio capa 2
  NetDeviceContainer devices11 = wifi.Install (wifiPhy11, wifiMac, c2Cluster1);
  NetDeviceContainer devices12 = wifi.Install (wifiPhy12, wifiMac, c2Cluster2);
  NetDeviceContainer devices13 = wifi.Install (wifiPhy13, wifiMac, c2Cluster3);
  NetDeviceContainer devices14 = wifi.Install (wifiPhy14, wifiMac, c2Cluster4);
  NetDeviceContainer devices15 = wifi.Install (wifiPhy15, wifiMac, c2Cluster5);
  NetDeviceContainer devices16 = wifi.Install (wifiPhy16, wifiMac, c2Cluster6);
  NetDeviceContainer devices17 = wifi.Install (wifiPhy17, wifiMac, c2Cluster7);
  NetDeviceContainer devices18 = wifi.Install (wifiPhy18, wifiMac, c2Cluster8);
  //FIn capa 2, inicio capa 3
  NetDeviceContainer devices19 = wifi.Install (wifiPhy19, wifiMac, c3Cluster1);
 
  


  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                "MinX", DoubleValue (0.0),
                                "MinY", DoubleValue (0.0),
                                "DeltaX", DoubleValue (distance),
                                "DeltaY", DoubleValue (distance),
                                "GridWidth", UintegerValue (5),
                                "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mobility.InstallAll();
  
  //Enable OLSR
  OlsrHelper olsr;
  Ipv4StaticRoutingHelper staticRouting;

  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (olsr, 10);

  InternetStackHelper internet;
  internet.SetRoutingHelper (list); // has effect on the next Install ()
/*   internet.Install (c1Cluster1);
  internet.Install (c1Cluster2);
  internet.Install (c1Cluster3);
  internet.Install (c1Cluster4);
  internet.Install (c1Cluster5);
  internet.Install (c1Cluster6);
  internet.Install (c1Cluster7);
  internet.Install (c1Cluster8);
  internet.Install (c1Cluster9);
  internet.Install (c1Cluster10);
  //Fin capa 1, inicio capa 2
  internet.Install (c2Cluster1);
  internet.Install (c2Cluster2);
  internet.Install (c2Cluster3);
  internet.Install (c2Cluster4);
  internet.Install (c2Cluster5);
  internet.Install (c2Cluster6);
  internet.Install (c2Cluster7);
  internet.Install (c2Cluster8);
  //FIn capa 2, inicio capa 3
 */
  internet.InstallAll();
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer d1 = ipv4.Assign (devices1);
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer d2 = ipv4.Assign (devices2);
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer d3 = ipv4.Assign (devices3);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer d4 = ipv4.Assign (devices4);
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer d5 = ipv4.Assign (devices5);
  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer d6 = ipv4.Assign (devices6);
  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer d7 = ipv4.Assign (devices7);
  ipv4.SetBase ("10.1.8.0", "255.255.255.0");
  Ipv4InterfaceContainer d8 = ipv4.Assign (devices8);
  ipv4.SetBase ("10.1.9.0", "255.255.255.0");
  Ipv4InterfaceContainer d9 = ipv4.Assign (devices9);
  ipv4.SetBase ("10.1.10.0", "255.255.255.0");
  Ipv4InterfaceContainer d10 = ipv4.Assign (devices10);
  //Fin capa 1, inicio capa 2
  ipv4.SetBase ("10.1.11.0", "255.255.255.0");
  Ipv4InterfaceContainer d11 = ipv4.Assign (devices11);
  ipv4.SetBase ("10.1.12.0", "255.255.255.0");
  Ipv4InterfaceContainer d12 = ipv4.Assign (devices12);
  ipv4.SetBase ("10.1.13.0", "255.255.255.0");
  Ipv4InterfaceContainer d13 = ipv4.Assign (devices13);
  ipv4.SetBase ("10.1.14.0", "255.255.255.0");
  Ipv4InterfaceContainer d14 = ipv4.Assign (devices14);
  ipv4.SetBase ("10.1.15.0", "255.255.255.0");
  Ipv4InterfaceContainer d15 = ipv4.Assign (devices15);
  ipv4.SetBase ("10.1.16.0", "255.255.255.0");
  Ipv4InterfaceContainer d16 = ipv4.Assign (devices16);
  ipv4.SetBase ("10.1.17.0", "255.255.255.0");
  Ipv4InterfaceContainer d17 = ipv4.Assign (devices17);
  ipv4.SetBase ("10.1.18.0", "255.255.255.0");
  Ipv4InterfaceContainer d18 = ipv4.Assign (devices18);
  //Fin capa 2, inicio capa 3
  ipv4.SetBase ("10.1.19.0", "255.255.255.0");
  Ipv4InterfaceContainer d19 = ipv4.Assign (devices19);
  
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");//Fabian god tiene que acordarme
  Ptr<Socket> recvSink = Socket::CreateSocket (c1Cluster1.Get(1), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));

  Ptr<Socket> source = Socket::CreateSocket (c1Cluster8.Get(0), tid);
  InetSocketAddress remote = InetSocketAddress (d1.GetAddress (1, 0), 80);
  source->Connect (remote);

//Escenario 3--------------------------------------------------------------------------

  if (tracing == true)
    {
      AsciiTraceHelper ascii;
      wifiPhy1.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy1.EnablePcap ("wifi-simple-adhoc-grid", devices1);

      wifiPhy2.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy2.EnablePcap ("wifi-simple-adhoc-grid", devices2);

      wifiPhy3.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy3.EnablePcap ("wifi-simple-adhoc-grid", devices3);

      wifiPhy4.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy4.EnablePcap ("wifi-simple-adhoc-grid", devices4);

      wifiPhy5.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy5.EnablePcap ("wifi-simple-adhoc-grid", devices5);

      wifiPhy6.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy6.EnablePcap ("wifi-simple-adhoc-grid", devices6);

      wifiPhy7.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy7.EnablePcap ("wifi-simple-adhoc-grid", devices7);

      wifiPhy8.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy8.EnablePcap ("wifi-simple-adhoc-grid", devices8);

      wifiPhy9.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy9.EnablePcap ("wifi-simple-adhoc-grid", devices9);

      wifiPhy10.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy10.EnablePcap ("wifi-simple-adhoc-grid", devices10);

      wifiPhy11.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy11.EnablePcap ("wifi-simple-adhoc-grid", devices11);

      wifiPhy12.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy12.EnablePcap ("wifi-simple-adhoc-grid", devices12);

      wifiPhy13.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy13.EnablePcap ("wifi-simple-adhoc-grid", devices13);

      wifiPhy14.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy14.EnablePcap ("wifi-simple-adhoc-grid", devices14);

      wifiPhy15.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy15.EnablePcap ("wifi-simple-adhoc-grid", devices15);

      wifiPhy16.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy16.EnablePcap ("wifi-simple-adhoc-grid", devices16);

      wifiPhy17.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy17.EnablePcap ("wifi-simple-adhoc-grid", devices17);

      wifiPhy18.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy18.EnablePcap ("wifi-simple-adhoc-grid", devices18);

      wifiPhy19.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy19.EnablePcap ("wifi-simple-adhoc-grid", devices19);

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