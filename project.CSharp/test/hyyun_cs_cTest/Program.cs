using System;
using System.Collections.Generic;
using System.Text;

using Hy;
using Hy.Container;
using Hyyun;

namespace hyyun_cs_cTest
{
    /// <summary>
    /// 消息处理者
    /// </summary>
    class DemoMessageProcessor : IClientMessageProcessor
    {
        public ushort getMessageMark() { return 1; }
        public void onMessageRecv(ref YunClient conn, ref YunStream readStream)
        {
            string str;
            if (!readStream.getColValue(1, out str))    return;
            System.Console.WriteLine(str);
        }
    }

    /// <summary>
    /// 程序
    /// </summary>
    class Program
    {
        static void Main1(string[] args)
        {
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
                    YunClient client = new YunClient(-1, false, false);
                    client.addMessageProcessor(new DemoMessageProcessor());

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
                        string v2 = "hello world!"; msg.addCol(ref v2);
                    }
                    msg.addColEnd();

                    if (!client.sendMessage(ref msg))
                    {
                        // 发送失败
                        throw new Exception("发送失败");
                    }

                } // for

                //-----------------------------------------------
                // 断开连接
                //client.disconnect();
            }
            catch (Exception ex)
            {

            }
        }
    }
}
