using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NetMQ;
using NetMQ.Sockets;

namespace ServerSide
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var server = new ResponseSocket("@tcp://localhost:5556"))
            {
                string msg;
                do
                {
                    msg = server.ReceiveFrameString();
                    Console.WriteLine($"From Client: {msg}");

                    // Send a response back from the server
                    server.SendFrame(msg);
                } while (msg != "done");
            }
        }
    }
}
