#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <cstring>


#define PORT 8080

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    // Création du socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Erreur de création du socket" << std::endl;
        return -1;
    }

    // Initialisation de l'adresse du serveur
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Attachement du socket au port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "Erreur de bind" << std::endl;
        return -1;
    }

    // Ecoute des connexions entrantes
    if (listen(server_fd, 3) < 0)
    {
        std::cerr << "Erreur de listen" << std::endl;
        return -1;
    }

    // Acceptation de la connexion entrante
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
    {
        std::cerr << "Erreur d'accept" << std::endl;
        return -1;
    }

    // Lecture de la taille de l'image envoyée par le client
    int image_size;
    if (read(new_socket, &image_size, sizeof(image_size)) < 0)
    {
        std::cerr << "Erreur de lecture de la taille de l'image" << std::endl;
        return -1;
    }

    // Allocation de mémoire pour l'image
    char* image_data = new char[image_size];

    // Lecture de l'image envoyée par le client
    if (read(new_socket, image_data, image_size) < 0)
    {
        std::cerr << "Erreur de lecture de l'image" << std::endl;
        return -1;
    }

    // Ecriture de l'image sur le disque
    std::ofstream output_file("image.jpg", std::ios::binary);
    output_file.write(image_data, image_size);
    output_file.close();

    // Fermeture de la connexion
    close(new_socket);
    close(server_fd);

    return 0;
}