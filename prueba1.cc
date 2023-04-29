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
using namespace std;
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

  // Primera capa
  NodeContainer c1Cluster[10];
  for(int i = 0; i < 10; i++) {
    c1Cluster[i].Create(3);
  }

  // Segunda capa
  NodeContainer c2Cluster[8];
  for(int i = 0; i < 8; i++) {
    c2Cluster[i].Create(4);
    if(i > 1) c2Cluster[i].Create(1);   // Crear uno extra para los clusters con solo 1 cabeza
    c2Cluster[i].Add(c1Cluster[i].Get(0));
  }

  c2Cluster[0].Add(c1Cluster[8].Get(0));
  c2Cluster[1].Add(c1Cluster[9].Get(0));

  // Tercera capa
  NodeContainer c3Cluster;
  for(int i = 0; i < 8; i++) {
    c3Cluster.Add(c2Cluster[i].Get(0));
  }

  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  if (verbose)
    {
      wifi.EnableLogComponents ();  // Turn on all Wifi logging
    }

  YansWifiPhyHelper wifiPhy[19]; 
  // set it to zero; otherwise, gain will be added
  for(int i = 0; i < 19; i++) {
    wifiPhy[i].Set ("RxGain", DoubleValue (-10));

  }

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");

  for(int i = 0; i < 19; i++) {
    wifiPhy[i].SetChannel (wifiChannel.Create ());
  }


  // Add an upper mac and disable rate control
  WifiMacHelper wifiMac;
  wifi.SetStandard (WIFI_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");


  NetDeviceContainer devices[19];
  // 1ra capa
  for(int i = 0; i < 10; i++) {
  devices[i] = wifi.Install (wifiPhy[i], wifiMac, c1Cluster[i]);
  }

  // 2da capa
  for(int i = 0; i < 8; i++) {
  devices[i+10] = wifi.Install (wifiPhy[i+10], wifiMac, c2Cluster[i]);
  }

  // 3ra capa
  devices[18] = wifi.Install (wifiPhy[18], wifiMac, c3Cluster);
 
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
  internet.InstallAll();

  // TODO de aqui en adelante
  NS_LOG_INFO ("Assign IP Addresses.");
  //Ipv4AddressHelper ipv4 [19];
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer d1 [19];

  for(int i = 0; i < 19; i++) {
    //Ipv4Address netAddress = Ipv4Address("10.1." + to_string(i) + ".0");
    d1[i] = ipv4.Assign(devices[i]);

  }
  
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");//Fabian god tiene que acordarme // no me acuerdo de qué
  Ptr<Socket> recvSink = Socket::CreateSocket (c1Cluster[0].Get(0), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));

  Ptr<Socket> source = Socket::CreateSocket (c1Cluster[0].Get(0), tid);
  InetSocketAddress remote = InetSocketAddress (d1[0].GetAddress (1, 0), 80);
  source->Connect (remote);

//Escenario 3--------------------------------------------------------------------------

  if (tracing == true)
    {
      AsciiTraceHelper ascii;

      for(int i = 0; i < 19; i++) {
        wifiPhy[i].EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
        wifiPhy[i].EnablePcap ("wifi-simple-adhoc-grid", devices[i]);
      }

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