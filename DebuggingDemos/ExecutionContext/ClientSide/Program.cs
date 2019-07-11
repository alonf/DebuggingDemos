using System;
using System.Diagnostics;
using System.Threading.Tasks;
using NetMQ;
using NetMQ.Sockets;

namespace ClientSide
{
    class Program
    {
        static void Main()
        {
            Console.WriteLine("Starting client side.");
            SendMessages();
        }

        private static void SendMessages()
        {
            object zeroMqLock = new object();

            using (var client = new RequestSocket(">tcp://localhost:7722")) // connect
            {
                Parallel.For(1, 100, i =>
                {
                    // ReSharper disable once AccessToDisposedClosure
                    SendAMessage(zeroMqLock, client, i);
                    Task.Delay(100);
                });
                client.SendFrame($"done");
            }
        }

        private static void SendAMessage(object zeroMqLock, RequestSocket client, int i)
        {
            string msg;
            lock (zeroMqLock)
            {
                client.SendFrame($"Msg {i}");
                msg = client.ReceiveFrameString();
            }
            if (i == 20)
                Debugger.Break();

            Console.WriteLine($"From Server: {msg}");
        }
    }
}
