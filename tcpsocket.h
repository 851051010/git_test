#pragma once
//��Windows��ʵ��socket��̣���Ҫ����Windows��socket��
#include <WinSock2.h>
//Ԥ���룬�ñ�������ʼ��һ����̬�⣬�����̬���ṩ�˺ܶ�API�ӿ�
#pragma comment(lib,"ws2_32.lib")

//������������Ҫ�ĺ���
//��ʼ��WinSock2
bool init_socket();
//�ͷź������ر�����⣬�ͷ�ϵͳ��Դ
bool close_socket();
//���������socket��������ʵ�ַ���˵�API
SOCKET serverSocket();
//�����ͻ���socket��������ʵ�ֿͻ��˵�API
SOCKET clientSocket(const char* ip); //ipΪ������IP��ַ

