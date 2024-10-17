#pragma once
//在Windows上实现socket编程，需要引入Windows的socket库
#include <WinSock2.h>
//预编译，让编译器初始化一个静态库，这个静态库提供了很多API接口
#pragma comment(lib,"ws2_32.lib")

//接下来定义需要的函数
//初始化WinSock2
bool init_socket();
//释放和清理，关闭网络库，释放系统资源
bool close_socket();
//创建服务端socket对象，用来实现服务端的API
SOCKET serverSocket();
//创建客户端socket对象，用来实现客户端的API
SOCKET clientSocket(const char* ip); //ip为服务器IP地址

