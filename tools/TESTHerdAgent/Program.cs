using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Text.RegularExpressions;
using Herd;
using System.Threading;

namespace TESTHerdAgent
{
    class Program
    {
        static HerdService m_herdService;
       
        static void Main(string[] args)
        {
            m_herdService = new HerdService();

            m_herdService.DoStart();

            while (true)
            {

            }
           
        }
    }
}
