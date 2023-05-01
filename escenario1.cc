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
std::string phyMode ("DsssRate1Mbps");
uint32_t packetSize = 1000; // bytes
uint32_t numPackets = 1;
uint32_t numNodes = 6;  // by default, 5x5
uint32_t sinkNode = 1;
uint32_t sourceNode = 0;
int capacidadCap2=1;
int numClustersCap1 = 4; // Escenario 1
int numClustersCap2 = 2;
int numNodesCap1 = 6;
int numTotalClusters = numClustersCap1+numClustersCap2+1;
double interval = 1.0; //Seconds
bool verbose = false;
bool tracing = false;

static double geometric_truncated(int x, double p, int k)
{
    return pow(1 - p, x - 1) * p / (1 - pow(1 - p, k));
}

void createOnOff(NodeContainer sourceNodeContainer, uint32_t sourceClusterIndex,  uint32_t x, NodeContainer sinkNodeContainer, 
                 uint32_t sinkClusterIndex, uint32_t y, Ipv4InterfaceContainer sinkInterface, 
                 double dataRate, uint32_t packetSize){

  Ptr<ExponentialRandomVariable> expTime = CreateObject<ExponentialRandomVariable>();

  expTime->SetAttribute("Mean", DoubleValue(2.0));

  PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(sinkInterface.GetAddress(x, 0), 10)));
  ApplicationContainer app = sink.Install(sinkNodeContainer.Get(x));

  OnOffHelper onOffHelper("ns3::UdpSocketFactory", Address(InetSocketAddress(sinkInterface.GetAddress(x, 0), 10)));
  onOffHelper.SetAttribute("OnTime", PointerValue(expTime));
  onOffHelper.SetAttribute("OffTime", PointerValue(expTime));
  onOffHelper.SetAttribute("DataRate", StringValue(to_string(static_cast<int>((dataRate*(static_cast<double>(capacidadCap2)/numClustersCap1) * 1000)))+"Kbps"));
  onOffHelper.SetAttribute("PacketSize", UintegerValue(packetSize));

  app = onOffHelper.Install(sourceNodeContainer.Get(y));
  

  cout << "este: " + to_string(static_cast<int>((dataRate*(static_cast<double>(capacidadCap2)/numClustersCap1) * 1000))) << endl;
  // cout << to_string(capacidadCap2) + "-" << to_string(numClustersCap1) + "-" << to_string(static_cast<double>(capacidadCap2)/numClustersCap1) << to_string((dataRate*(capacidadCap2/numClustersCap1))) << endl;
  cout <<to_string(expTime->GetValue())+ ","+ to_string(expTime->GetValue()) << endl;
  app.Start(Seconds(30.0));
  app.Stop(Seconds(60.0)); 
}

void createOnOffRandom(NodeContainer sourceNodeContainer, NodeContainer sinkNodeContainer, Ipv4InterfaceContainer sinkInterface, double dataRate, uint32_t packetSize){

  double min = 0.0;
  double max = 5.0;
  
  Ptr<UniformRandomVariable> xUniform = CreateObject<UniformRandomVariable> ();
  xUniform->SetAttribute ("Min", DoubleValue (min));
  xUniform->SetAttribute ("Max", DoubleValue (max));

  Ptr<UniformRandomVariable> yUniform = CreateObject<UniformRandomVariable> ();
  yUniform->SetAttribute ("Min", DoubleValue (min));
  yUniform->SetAttribute ("Max", DoubleValue (max));

  uint32_t x = xUniform->GetValue();
  uint32_t y = yUniform->GetValue();

  PacketSinkHelper sink("ns3::UdpSocketFactory", Address(InetSocketAddress(sinkInterface.GetAddress(x, 0), 10)));
  ApplicationContainer app = sink.Install(sinkNodeContainer.Get(x));

  OnOffHelper onOffHelper("ns3::UdpSocketFactory", Address(InetSocketAddress(sinkInterface.GetAddress(x, 0), 10)));
  onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=4]"));
  onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
  onOffHelper.SetAttribute("DataRate", StringValue(to_string(dataRate)+"Mbps"));
  onOffHelper.SetAttribute("PacketSize", UintegerValue(packetSize));

  app = onOffHelper.Install(sourceNodeContainer.Get(y));

  NS_LOG_UNCOND("--->"+ to_string(x) + " || " +  to_string(y) + "\n");

  app.Start(Seconds(30.0));
  app.Stop(Seconds(40.0));
}

int main (int argc, char *argv[])
{

// Definimos una instancia de la clase PoissonRandomVariable y establecemos su valor medio,
//el valor medio se establece a la inversa del intervalo inter-packet, ya que esto corresponde a la tasa de llegada media para una distribución de Poisson/*PoissonRandomVariable randomArrival;
//randomArrival.SetAttribute ("Mean", DoubleValue (1.0 / interval));


  CommandLine cmd;
  cmd.Parse (argc, argv);


  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",
                      StringValue (phyMode));


  //Escenario 1--------------------------------------------------------------------------

  /*Este escenario tenía 4 clústeres con 6 nodos en la primera capa y 2 clústeres con 2
  nodos en la segunda capa. */

  // Primera capa
  NodeContainer c1Cluster[numClustersCap1];
  for(int i = 0; i < numClustersCap1; i++) {
    c1Cluster[i].Create(numNodesCap1);
  }

  // Segunda capa
  NodeContainer c2Cluster[numClustersCap2];
  for(int i = 0; i < numClustersCap2; i++) {
    c2Cluster[i].Add(c1Cluster[i].Get(0));
    c2Cluster[i].Add(c1Cluster[i+2].Get(0));
  }

  // Tercera capa
  NodeContainer c3Cluster;
  for(int i = 0; i < numClustersCap2; i++) {
    c3Cluster.Add(c2Cluster[i].Get(0));
  }

  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  if (verbose)
    {
      wifi.EnableLogComponents ();  // Turn on all Wifi logging
    }

  YansWifiPhyHelper wifiPhy[numTotalClusters]; 
  // set it to zero; otherwise, gain will be added
  for(int i = 0; i < numTotalClusters; i++) {
    wifiPhy[i].Set ("RxGain", DoubleValue (-10));

  }

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");

  for(int i = 0; i < numTotalClusters; i++) {
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

  NetDeviceContainer devices[numTotalClusters];
  // 1ra capa
  for(int i = 0; i < numClustersCap1; i++) {
  devices[i] = wifi.Install (wifiPhy[i], wifiMac, c1Cluster[i]);
  }

  // 2da capa
  for(int i = 0; i < numClustersCap2; i++) {
  devices[i+numClustersCap1] = wifi.Install (wifiPhy[i+numClustersCap1], wifiMac, c2Cluster[i]);
  }

  // 3ra capa
  devices[numTotalClusters-1] = wifi.Install (wifiPhy[numTotalClusters-1], wifiMac, c3Cluster);

  // Movilidad de los nodos
  MobilityHelper mobilityLayer;

  ObjectFactory pos;
  pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
  pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"));
  pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=100.0]"));
  Ptr<PositionAllocator> posAlloc = pos.Create()->GetObject<PositionAllocator>();

  mobilityLayer.SetMobilityModel("ns3::RandomWaypointMobilityModel", 
                          "Speed", StringValue ("ns3::UniformRandomVariable[Min=0|Max=1]"),
                          "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"),
                          "PositionAllocator", PointerValue(posAlloc));
  mobilityLayer.SetPositionAllocator (posAlloc);

  mobilityLayer.InstallAll();
  
  //Enable OLSR
  OlsrHelper olsr;
  Ipv4StaticRoutingHelper staticRouting;

  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (olsr, 10);

  InternetStackHelper internet;
  internet.SetRoutingHelper (list); // has effect on the next Install ()
  internet.InstallAll();

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;

  Ipv4InterfaceContainer d1 [numTotalClusters];
  for(int i = 0; i < numTotalClusters; i++) {
    string a = ("10.1." + to_string(i) + ".0");
    ipv4.SetBase(a.c_str(), "255.255.255.0");
    d1[i] = ipv4.Assign(devices[i]);
  }
  
  //Escenario 1--------------------------------------------------------------------------

  // PRUEBA DE ENVIO BILATERAL
  // createOnOff(c1Cluster[1], 1,  0, c1Cluster[0], 0,  1, d1[0], 0.5, 1024);
  // createOnOff(c1Cluster[0], 0, 1, c1Cluster[1], 1, 0, d1[1], 0.5, 1024);

  Ptr<UniformRandomVariable> clusterUniform = CreateObject<UniformRandomVariable> ();
  Ptr<UniformRandomVariable> nodeUniform = CreateObject<UniformRandomVariable> ();

  clusterUniform->SetAttribute ("Min", DoubleValue (0));
  clusterUniform->SetAttribute ("Max", DoubleValue (numClustersCap1));

  nodeUniform->SetAttribute ("Min", DoubleValue (0));
  nodeUniform->SetAttribute ("Max", DoubleValue (numNodesCap1));

  for (int i = 0; i < numClustersCap1; i++)
  {
    for (int j = 1; j < numNodesCap1; j++){

      uint32_t sinkCluster = clusterUniform->GetValue();
      uint32_t sinkNode = nodeUniform->GetValue();

      double dataRate = geometric_truncated(j, 0.7, numNodesCap1);
      createOnOff(c1Cluster[i], i,  j, c1Cluster[sinkCluster], sinkCluster,  sinkNode, d1[i], dataRate, 1024); 
    }
  }
  

  // NO FUNCIONA QUE UN NODO ENVIE DOS VECES AL MISMO TIEMPO A DOS NODOS DIFERENTES
  // createOnOff(c1Cluster[1], 0, c1Cluster[0], 1, d1[0], 0.5, 1024);
  // createOnOff(c1Cluster[1], 0, c1Cluster[0], 2, d1[0], 0.5, 1024);

  AsciiTraceHelper ascii;
  wifiPhy[0].EnableAsciiAll(ascii.CreateFileStream("ando.tr"));
  Simulator::Stop (Seconds (60.0));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}