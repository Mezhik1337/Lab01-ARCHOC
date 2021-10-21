#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#include <winsock2.h>
#include <vector>
#include <string>
#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <ctime>
using namespace std;



void recv_file(SOCKET* sock) {
    char file_size_str[16];
    char file_name[32];

    recv(*sock, file_size_str, 16, 0);
    int file_size = atoi(file_size_str);
    char* bytes = new char[file_size];

    recv(*sock, file_name, 32, 0);

    fstream file;
    file.open(file_name, ios_base::out | ios_base::binary);

    cout << "size " << file_size << endl;
    cout << "name " << file_name << endl;

    if (file.is_open()) {
        recv(*sock, bytes, file_size, 0);
        cout << "data: " << bytes << endl;

        file.write(bytes, file_size);
        cout << "Save!" << endl;
    }
    else
        cout << "Error file open" << endl;

    delete[] bytes;
    file.close();


}

vector<SOCKET> connections;
int connections_counter = -1;
string sent = " sent: ";
string received = " received: ";
void log(string message, string action)
{
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);                         // текущая дата, выраженная в секундах
    timeinfo = localtime(&rawtime);        // текущая дата, представленная в нормальной форме

    ofstream out;
    out.open("C:\\Users\\38067\\Desktop\\Археос\\logfile.txt", ios::app);

    if (out.is_open())
    {
        out << asctime(timeinfo) << "Server" << action << message << endl;
    }
    out.close();
}


void SendInfo(int connection_marker)
{
    while (true)
    {
        int size_of_command;
        recv(connections[connection_marker], (char*)&size_of_command, sizeof(size_of_command), NULL);

        char* command = new char[size_of_command + 1];
        command[size_of_command] = '\0';
        if (recv(connections[connection_marker], command, size_of_command, NULL) < 0)
        {
            closesocket(connections[connection_marker]);
            break;
        }
        string command_reinitialize = command;
        log(command_reinitialize, received);
        transform(begin(command_reinitialize), end(command_reinitialize), begin(command_reinitialize), tolower);
        string short_command = command_reinitialize.substr(0, size_of_command - 2);
        if (command_reinitialize == "who")
        {
            string info = "Client and server created by Mezhybovskiy Yura(file transfer)from K-27 group for variant 2";
            int size_of_info = size(info);
            send(connections[connection_marker], (char*)&size_of_info, sizeof(size_of_info), NULL);
            send(connections[connection_marker], info.c_str(), size_of_info, NULL);
            log(info, sent);
        }
        else if (command_reinitialize == "help")
        {
            string info = "info recieved";
            int size_of_info = size(info);
            send(connections[connection_marker], (char*)&size_of_info, sizeof(size_of_info), NULL);
            send(connections[connection_marker], info.c_str(), size_of_info, NULL);
            log(info, sent);
        }
        else
        {
            string error_text = "Error: unknown command";
            int size_of_error_text = size(error_text);
            send(connections[connection_marker], (char*)&size_of_error_text, sizeof(size_of_error_text), NULL);
            send(connections[connection_marker], error_text.c_str(), size_of_error_text, NULL);
            log(error_text, sent);
        }
        delete[] command;
    }

}

int main()
{
    setlocale(LC_ALL, "Russian");

    WSAData wsadata;
    WORD DLLVersion = MAKEWORD(2, 1); //запрашиваемая версия библиотеки винсок
    if (WSAStartup(DLLVersion, &wsadata) != 0)
    {
        cout << "WSAStartup couldn't upload winsock version" << endl;
        log("WSAStartup couldn't upload winsock version", " error: ");
        exit(1);
    }

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1027);
    addr.sin_family = AF_INET; //константа для интернет протоколов

    SOCKET Listen = socket(AF_INET, SOCK_STREAM, NULL);// coкстрим это протокол устанавливающий соединение
    if (bind(Listen, (SOCKADDR*)&addr, sizeof(addr)) < 0)//привязка адресса сокету
    {
        cout << "Couldn't bind an addr" << endl;
        log("Couldn't bind an addr", " error: ");
        exit(1);
    }

    listen(Listen, SOMAXCONN);/*прослушиваем порт, сомаксконн это константа максимального значения
                                клиентов, стоящих в очереди*/

    while (true)
    {
        int addrsize = sizeof(addr);
        SOCKET connection = accept(Listen, (SOCKADDR*)&addr, &addrsize);
        if (connection == 0)
        {
            cout << "Couldn't connect with client" << endl;
            log("Couldn't connect with client", " error: ");
            return 1;
        }
        else
        {
            connections.push_back(connection);
            connections_counter++;

            cout << "Client connection accepted" << endl;
            log("Client connection accepted", " uploaded successful: ");
            char msg[256];
            recv(connection, msg, sizeof(msg), NULL);
            else
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendInfo, (LPVOID)(connections_counter), NULL, NULL);
        }
    }
    return 0;
}