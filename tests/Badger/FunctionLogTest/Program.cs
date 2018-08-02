using Badger.Data;
using System;

namespace FunctionLogTest
{
    class Program
    {
        static void Main(string[] args)
        {

            FunctionLog functionLog= new FunctionLog("../../test.simion.log.functions");

            foreach(Function f in functionLog.Functions)
            {
                Console.WriteLine("Function #" + f.Id + " (" + f.Name + "): " + f.Samples.Count + " samples)");
            }
        }
    }
}
