using System;
using System.Collections.Generic;
using System.Text;

using Hy;
using Hy.Container;
using Hyyun;

namespace hyyun_cs_cTest_noUseMsgCenter
{
    /// <summary>
    /// 程序
    /// </summary>
    class Program
    {
        static void Main(string[] args)
        {
            YunClient client1 = new YunClient(-1, true, true);
            client1.Dispose();
            GC.Collect();
            GC.WaitForPendingFinalizers();

            string ip = "127.0.0.1";
            int port = 22009;

            //----------------------------------------
            // 消息发送
            try
            {
                for (uint i = 0; i < 1000; ++i)
                {
                    //-----------------------------------------------
                    // 创建客户端
                    //YunClient client(Protocol_Tcp); // 目前只支持 Protocol_Tcp
                    YunClient client = new YunClient(-1, true, false);

                    //-----------------------------------------------
                    // 连接服务器
                    if (!client.connect(ip, port))
                    {
                        // 连接失败
                        throw new Exception("连接失败");
                    }

                    //-----------------------------------------------
                    // 发送数据
                    YunStream msg = new YunStream();
                    msg.addColBegin(false, true);
                    {
                        UInt16 mark = 1; msg.addCol(ref mark);
                        string v2 = "中华231231"; msg.addCol(ref v2);
                    }
                    msg.addColEnd();

                    if (!client.sendMessage(ref msg))
                    {
                        // 发送失败
                        throw new Exception("发送失败");
                    }

                    //-----------------------------------------------
                    // 接收数据
                    YunStream readMsg = client.getMessage();
                    if (!readMsg.isLegalYun())
                    {
                        throw new Exception("接收超时");
                    }

                    string v3;
                    if (!readMsg.getColValue(1, out v3)) return;
                    Console.WriteLine(v3);

                    //-----------------------------------------------
                    // 断开连接
                    //client.disconnect();

                    client.disconnect();
                    GC.Collect();
                    GC.WaitForPendingFinalizers();


                } // for

            }
            catch (Exception ex)
            {

            }
        }
    }
}
