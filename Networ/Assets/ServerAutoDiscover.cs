using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using System.Net.Sockets;
using System.Net;
using UnityEngine.UI;

namespace Assets
{
    class ServerAutoDiscover: MonoBehaviour
    {
        [SerializeField]
        private int serverPort = 5678;
        [SerializeField]
        private int connections = 1;
        [SerializeField]
        private int udpPort;

        public Text text;

        private bool startBroadcasting;
        private UdpClient udpClient;
        private IPEndPoint remoteIpEnd;

        private Socket socket;
        private IPEndPoint iep;
        byte[] message;


        public void StartServer()
        {
            Network.InitializeServer(connections, serverPort, false);            
            StartBroadcasting();
        }

        public void Connect()
        {
            startBroadcasting = false;
            string ip = ReciveBroadcastingMessage();
            text.text = ip;
            
            Network.Connect(ip, serverPort);
        }

        private string ReciveBroadcastingMessage()
        {
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram,ProtocolType.Udp);
            iep = new IPEndPoint(IPAddress.Any, udpPort);
            socket.Bind(iep);
            EndPoint ep = iep as EndPoint;
            byte[] data = new byte[1024];
            socket.ReceiveFrom(data, ref ep);
            socket.Close();
            return ep.ToString();
        }

        private void StartBroadcasting()
        {
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);            
            iep = new IPEndPoint(IPAddress.Broadcast, udpPort);
            message = Encoding.ASCII.GetBytes(Dns.GetHostName());
            startBroadcasting = true;
        }

        private void Update()
        {
            if (startBroadcasting)
            {
                if (Network.connections.Length < connections)
                {
                    socket.SendTo(message, iep);
                    Debug.Log("send");
                }
                else
                {
                    socket.Close();
                    startBroadcasting = false;
                }
            }
        }

        

    }
}
