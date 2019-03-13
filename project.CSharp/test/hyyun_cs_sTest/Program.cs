using System;
using System.Collections.Generic;
using System.Text;

namespace hyyun_cs_sTest
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                DemoYunServer server = new DemoYunServer(-1, 8);
                server.go();
            }
            catch (System.Exception ex)
            {
            	
            }
        }
    }
}
