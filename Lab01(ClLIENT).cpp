#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#include <winsock2.h>
#include <string>
#include <fstream>
#include <ctime>
#include <vector>
#include<EXPERIMENTAL/filesystem>
using namespace std;


void send_file(SOCKET* sock, const string& file_name) {
    fstream file;
    file.open(file_name, ios_base::in | ios_base::binary);

    if (file.is_open()) {
        int file_size = experimental::filesystem::file_size(file_name) + 1;

        char* bytes = new char[file_size];

        file.read(bytes, file_size);

        cout << "size " << file_size << endl;
        cout << "name " << file_name << endl;

        send(*sock, to_string(file_size).c_str(), 61, 0);
        send(*sock, file_name.c_str(), 32, 0);
        send(*sock, bytes, file_size, 0);

        delete[] bytes;
    }
    else
        cout << "Error file open" << endl;
    file.close();
}
SOCKET Connect;
string sent = " sent: ";
string received = " received: ";
void log(string message, string action)
{
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);                         // current date using seconds
    timeinfo = localtime(&rawtime);        // current date in normal form

    ofstream out;
    out.open("C:\\Users\\38067\\Desktop\\Археос\\logfile.txt", ios::app);

    if (out.is_open())
    {
        out << asctime(timeinfo) << "Client" << action << message << endl;
    }
    out.close();
}

void GetInfo()
{
    int size_of_info;
    recv(Connect, (char*)&size_of_info, sizeof(size_of_info), NULL);
    char* info = new char[size_of_info + 1];
    info[size_of_info] = '\0';
    recv(Connect, info, size_of_info, NULL);
    string info_reinitialize = info;
    log(info, received);
    cout << info << endl;
    string short_str = info_reinitialize.substr(0, 4);
    int num = 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");

    WSAData wsadata;
    WORD DLLVersion = MAKEWORD(2, 1);

    if (WSAStartup(DLLVersion, &wsadata) != 0)
    {
        cout << "WSAStartup couldn't upload winsock version" << endl;
        log("WSAStartup couldn't upload winsock version", " error: ");
        exit(1);
    }

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1027);
    addr.sin_family = AF_INET;

    Connect = socket(AF_INET, SOCK_STREAM, NULL);

    if (connect(Connect, (SOCKADDR*)&addr, sizeof(addr)) != 0)
    {
        cout << "Connection failed" << endl;
        log("Connection failed", " error: ");
        return 1;
    }
    else
    {
        cout << "Client connected" << endl;
        log("Client connected", " upload successful: ");

        while (true)
        {
            cout << "Wait for command input:" << endl;
            log("Wait for command input:", " state: ");
            string command;
            cin >> command;
            if (command == "transfer") 
            {
                cout << "Please input file name" << endl;
                string name;
                cin >> name;
                send_file(&Connect, name);
            }
            if (command == "correct") {
                cout << "Enter number of line:";
                int number;
                cin >> number;
                while (number < 1 || number>5) {
                    cout << "Wrong input! Enter number from 1 to 5\n";
                    cin >> number;
                }
                command = command + " " + to_string(number);
            }
            int size_of_command = size(command);
            send(Connect, (char*)&size_of_command, sizeof(size_of_command), NULL);
            send(Connect, command.c_str(), size_of_command, NULL);
            log(command, sent);
            GetInfo();
            if (command == "help") {
                cout << "who - for getting some details about author\n";
                cout << "transfer - for transfer your file to server\n";
            }
        }
    }
    return 0;
}