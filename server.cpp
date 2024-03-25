#include <iostream>
#include <winsock2.h>
#include <fstream> // For file operations
#include <sstream> // For string manipulation
#include <direct.h> // For directory creation
#include <filesystem> // For directory listing
#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;

void sendResponse(SOCKET clientSocket, const std::string& response) {
    send(clientSocket, response.c_str(), response.length(), 0);
}

std::string receiveRequest(SOCKET clientSocket) {
    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed" << std::endl;
        return "";
    } else if (bytesReceived == 0) {
        std::cerr << "Connection closed by client" << std::endl;
        return "";
    } else {
        return std::string(buffer);
    }
}

void readFile(SOCKET clientSocket, const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        sendResponse(clientSocket, fileContents);
    } else {
        std::string errorMsg = "File not found: " + filename;
        sendResponse(clientSocket, errorMsg);
    }
}

void createFile(SOCKET clientSocket, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file.close();
        sendResponse(clientSocket, "File created successfully");
    } else {
        std::cerr << "Unable to create file: " << filename << std::endl;
        sendResponse(clientSocket, "Error creating file");
    }
}

void writeFile(SOCKET clientSocket, const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
        sendResponse(clientSocket, "File saved successfully");
    } else {
        std::cerr << "Unable to create file: " << filename << std::endl;
        sendResponse(clientSocket, "Error saving file");
    }
}

void createDirectory(SOCKET clientSocket, const std::string& dirname) {
    if (_mkdir(dirname.c_str()) == 0) {
        sendResponse(clientSocket, "Directory created successfully");
    } else {
        std::cerr << "Unable to create directory: " << dirname << std::endl;
        sendResponse(clientSocket, "Error creating directory");
    }
}

void listDirectory(SOCKET clientSocket) {
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile("*.*", &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                sendResponse(clientSocket, findData.cFileName);
            }
        } while (FindNextFile(hFind, &findData) != 0);
        FindClose(hFind);
    } else {
        sendResponse(clientSocket, "Error listing directory");
    }
}

void handleClient(SOCKET clientSocket) {
    while (true) {
        std::string request = receiveRequest(clientSocket);
        if (request.empty()) {
            break; // Connection closed by client
        }
        std::istringstream iss(request);
        std::string command;
        iss >> command;

        if (command == "GET") {
            std::string filename;
            iss >> filename;
            readFile(clientSocket, filename);
        } else if (command == "CREATE") {
            std::string filename;
            iss >> filename;
            createFile(clientSocket, filename);
        } else if (command == "WRITE") {
            std::string filename, content;
            iss >> filename;
            std::getline(iss, content);
            writeFile(clientSocket, filename, content);
        } else if (command == "MKDIR") {
            std::string dirname;
            iss >> dirname;
            createDirectory(clientSocket, dirname);
        } else if (command == "LIST") {
            listDirectory(clientSocket);
        } else if (command == "EXIT") {
            break; // Client wants to close the connection
        } else {
            sendResponse(clientSocket, "Invalid command");
        }
    }

    closesocket(clientSocket);
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    // Create socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Accept connections
    SOCKET clientSocket;
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    while ((clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET) {
        std::cout << "Client connected" << std::endl;
        handleClient(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
