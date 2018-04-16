using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace DebugEvents
{
    class Program
    {
        static void Main(string[] args)
        {
            System.Diagnostics.Debug.WriteLine("Started!");
            int zero = 0;
            System.Diagnostics.Debugger.Break();
            System.Diagnostics.Debug.WriteLine("Between breakpoints");
            System.Diagnostics.Debugger.Break();

            Parallel.Invoke(
                ()=> System.Diagnostics.Debug.WriteLine("Hello "),
                () => System.Diagnostics.Debug.WriteLine("World!!!"));

            try
            {
                Console.WriteLine(42/zero);
            }
            catch (Exception exception)
            {
                Console.WriteLine(exception);
            }

            System.Diagnostics.Debug.WriteLine("Done.");
        }
    }
}
