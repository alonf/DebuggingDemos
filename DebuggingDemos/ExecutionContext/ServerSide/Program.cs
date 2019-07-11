using System;
using NetMQ;
using NetMQ.Sockets;

namespace ServerSide
{
    class Program
    {
        static void Main()
        {
            using (var server = new ResponseSocket("@tcp://localhost:7722"))
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
