#include <iostream>
#include <winsock2.h>
#include <fstream> // For file operations
#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;

void sendRequest(SOCKET clientSocket, const std::string& request) {
    send(clientSocket, request.c_str(), request.length(), 0);
}

std::string receiveResponse(SOCKET clientSocket) {
    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed" << std::endl;
        return "";
    } else if (bytesReceived == 0) {
        std::cerr << "Connection closed by server" << std::endl;
        return "";
    } else {
        return std::string(buffer);
    }
}

int main() {
    std::string serverIP;
     std::string adress;
    std::cout << "Enter PC name to access files: ";
    std::cin >> adress;
    if (adress == "mypc")
    {
        /* code */
serverIP = "127.0.0.1";
    }else
    {
        serverIP = "127.0.0.1";
    }
    

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    // Create socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Connect to server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str()); // Use user-provided IP address
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Client loop
    while (true) {
        std::cout << "Options:" << std::endl;
        std::cout << "1. Read file" << std::endl;
        std::cout << "2. Create file" << std::endl;
        std::cout << "3. Write to file" << std::endl;
        std::cout << "4. Create directory" << std::endl;
        std::cout << "5. List directory" << std::endl;
        std::cout << "6. Exit" << std::endl;

        int choice;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            std::string filename;
            std::cout << "Enter filename to read: ";
            std::cin >> filename;
            std::string getRequest = "GET " + filename;
            sendRequest(clientSocket, getRequest);
            std::string response = receiveResponse(clientSocket);
            std::cout << "Response from server: " << response << std::endl;
        } else if (choice == 2) {
            std::string filename;
            std::cout << "Enter filename to create: ";
            std::cin >> filename;
            std::string createRequest = "CREATE " + filename;
            sendRequest(clientSocket, createRequest);
            std::string response = receiveResponse(clientSocket);
            std::cout << "Response from server: " << response << std::endl;
        } else if (choice == 3) {
            std::string filename, content;
            std::cout << "Enter filename to write: ";
            std::cin >> filename;
            std::cout << "Enter content: ";
            std::cin.ignore(); // Ignore newline character
            std::getline(std::cin, content);
            std::string writeRequest = "WRITE " + filename + " " + content;
            sendRequest(clientSocket, writeRequest);
            std::string response = receiveResponse(clientSocket); // Receive acknowledgment from the server
            std::cout << "File saved successfully" << std::endl;
        } else if (choice == 4) {
            std::string dirname;
            std::cout << "Enter directory name to create: ";
            std::cin >> dirname;
            std::string mkdirRequest = "MKDIR " + dirname;
            sendRequest(clientSocket, mkdirRequest);
            std::string response = receiveResponse(clientSocket);
            std::cout << "Response from server: " << response << std::endl;
        } else if (choice == 5) {
            std::string listRequest = "LIST";
            sendRequest(clientSocket, listRequest);
            std::string response = receiveResponse(clientSocket);
            std::cout << "Directory contents:\n" << response << std::endl;
        } else if (choice == 6) {
            std::string exitCommand = "EXIT";
            sendRequest(clientSocket, exitCommand);
            // No need to wait for a response since the server won't send any after receiving the exit command
            break;
        } else {
            std::cout << "Invalid choice" << std::endl;
        }
    }

    // Close the connection
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
