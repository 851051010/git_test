#include "tcpsocket.h"
#include <iostream>
using namespace std;

int main()
{
    //1.初始化socket环境
    init_socket();

    //2.创建客户端的socket对象，同时完成绑定
    SOCKET clientsocker = clientSocket("192.168.124.2");

    //3.准备缓存空间
    char buf[512] = { '\0' }; //一个512字节大小的缓存空间
    //客户端先发后收，死循环保证聊天一直进行
    while (true)
    {
        //先清空缓存
        for (int i = 0; i < strlen(buf); i++)
        {
            buf[i] = '\0';
        }
        //给服务端发送消息
        cout << "给服务器发送消息:" << endl;
        cin.getline(buf, 511);
        if (send(clientsocker, buf, 511, 0) == SOCKET_ERROR)
        {
            cout << "客户端发送消息失败，错误码：" << WSAGetLastError() << endl;
            break;
        }

        //收消息，先清空缓存
        for (int i = 0; i < strlen(buf); i++)
        {
            buf[i] = '\0';
        }
        if (recv(clientsocker, buf, 511, 0) > 0)
        {
            cout << "服务端：" << buf << endl;
        }
        else
        {
            cout << "客户端接收消息失败，错误码：" << WSAGetLastError() << endl;
            break;
        }
    }

    //退出循环的时候，关闭socket
    //首先清理socket对象
    closesocket(clientsocker);
    //其次再关闭socket的系统资源
    close_socket();

    return 0;
}
