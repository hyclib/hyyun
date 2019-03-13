using System;
using System.Collections.Generic;
using System.Text;

using Hy;
using Hy.Container;
using Hyyun;

namespace hyyun_cs_sTest
{
    /// <summary>
    /// 消息处理器
    /// </summary>
    class DemoMessageProcessor : IServerMessageProcessor
    {
        public UInt16 getMessageMark() { return 1; }

        // 返回false表示此连接有误, 不再处理
        public bool onMessageRecv(ref Connection conn, ref YunStream readStream)
        {
            // 收数据并打印
            string v;
            if (!readStream.getColValue(1, out v)) return false;
            Console.WriteLine(v);

            // 发送数据
            YunStream ss1 = conn.startWrite(0);
            {
                ss1.addColBegin(true, false);

                UInt16 mark = 1;
                ss1.addCol(ref mark);

                string v1 = "dfdfdfeeeeeeeeee1515121";
                ss1.addCol(ref v1);

                ss1.addColEnd();
            }
            conn.finishWrite(ref ss1);

            return true;
        }
    };

    /// <summary>
    /// 服务器
    /// </summary>
    class DemoYunServer : YunServer
    {
        int i = 0;
        string ip = "127.0.0.1";
        int port = 22009;

        public DemoYunServer(int timeout, uint threadNum) : base(timeout, threadNum, 45*1000) { }

        //-----------------------------------------------
        override public void onConnected(Connection newConnection)
        {
            i++;
            Console.WriteLine(i);
            newConnection.finishConnected();
        }

        //-----------------------------------------------
        public void go()
        {
            addMessageProcessor(new DemoMessageProcessor());
            base.go(true, ip, port);
        }
    }

}
