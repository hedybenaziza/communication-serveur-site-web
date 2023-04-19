#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fstream>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 4096;

int main() {
    // Cr�e un socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Erreur de cr�ation du socket" << std::endl;
        return 1;
    }

    // Pr�pare l'adresse du serveur
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // Lie le socket � l'adresse
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Erreur de liaison du socket" << std::endl;
        return 1;
    }

    // �coute les connexions entrantes
    if (listen(serverSocket, 1) < 0) {
        std::cerr << "Erreur d'�coute" << std::endl;
        return 1;
    }

    std::cout << "Serveur en �coute sur le port " << PORT << std::endl;

    while (true) { // boucle infinie pour continuer � �couter les connexions entrantes
        // Accepte une connexion entrante
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);

        if (clientSocket < 0) {
            std::cerr << "Erreur d'acceptation" << std::endl;
            continue; // retourne � la boucle while pour continuer � �couter les connexions entrantes
        }

        std::cout << "Connexion accept�e" << std::endl;

        // Re�oit les donn�es du client et les stocke dans un vecteur
        std::vector<char> receivedData;
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;

        while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
            receivedData.insert(receivedData.end(), buffer, buffer + bytesRead);
        }

        std::cout << "Donn�es re�ues (" << receivedData.size() << " octets)" << std::endl;

        // �crit la taille des donn�es re�ues dans un fichier
        std::ofstream sizeFile("size.txt");
        sizeFile << receivedData.size() << std::endl;
        sizeFile.close();

        // R�cup�re l'extension du fichier
        std::string extension(receivedData.begin(), receivedData.begin() + 3);
        std::cout << "Extension : " << extension << std::endl;

        // �crit l'extension dans un fichier
        std::ofstream extensionFile("extension.txt");
        extensionFile << extension << std::endl;
        extensionFile.close();

        // Supprime les 3 premiers octets
        receivedData.erase(receivedData.begin(), receivedData.begin() + 3);

        // �crit les donn�es re�ues dans un fichier
        if (extension == "jpg") {
            std::ofstream outputFile("output.jpg", std::ios::binary);
            outputFile.write(receivedData.data(), receivedData.size());
            outputFile.close();
            std::cout << "Donn�es �crites dans output.jpg" << std::endl;
        }
        else if (extension == "jsn") {
            std::ofstream outputFile("output.json", std::ios::binary);
            outputFile.write(receivedData.data(), receivedData.size());
            outputFile.close();
            std::cout << "Donn�es �crites dans output.json" << std::endl;
        }
        else if (extension == "ini") {
            std::ofstream outputFile("output.ini", std::ios::binary);
            outputFile.write(receivedData.data(), receivedData.size());
            outputFile.close();
            std::cout << "Donn�es �crites dans output.ini" << std::endl;
        }
        else {
            std::ofstream outputFile("output.txt");
            outputFile << "Extension non reconnu : " << extension << std::endl;
            outputFile.write(receivedData.data(), receivedData.size());
            outputFile.close();
            std::cout << "Donn�es �crites dans output.txt" << std::endl;
        }

        // Ferme le socket client
        if (shutdown(clientSocket, SHUT_RDWR) < 0) {
            std::cerr << "Erreur lors de la fermeture du socket client" << std::endl;
        }

        // R�initialise les donn�es
        receivedData.clear();

        // Attend une nouvelle connexion
        std::cout << "Attente d'une nouvelle connexion" << std::endl;
    }
}