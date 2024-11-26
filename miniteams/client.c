#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void send_message(pid_t server_pid, const char *message) {
    
    // On converti le PID en chaîne de caractères
    int pid = getpid();
    char my_pid_and_message[1000];
    sprintf(my_pid_and_message, "%d", pid);
    
    // On sépare le PID et le message par un caractère spécial
    strcat(my_pid_and_message, "|");
    strcat(my_pid_and_message, message);
    
    for (size_t i = 0; i < strlen(my_pid_and_message); i++) {
        char ch = my_pid_and_message[i]; // Récupère le caractère à envoyer
        for (int bit = 7; bit >= 0; bit--) {
            int sig = (ch & (1 << bit)) ? SIGUSR2 : SIGUSR1;
            if (kill(server_pid, sig) == -1) {
                perror("Error sending signal");
                exit(EXIT_FAILURE);
            }
            usleep(500); // Pause pour éviter d'envoyer trop vite
        }
    }

    // Envoyer un caractère de fin ('\0')
    char end_char = '\0';
    for (int bit = 7; bit >= 0; bit--) {
        int sig = (end_char & (1 << bit)) ? SIGUSR2 : SIGUSR1;
        if (kill(server_pid, sig) == -1) {
            perror("Error sending signal");
            exit(EXIT_FAILURE);
        }
        usleep(500);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_pid> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t server_pid = atoi(argv[1]); // Convertit l'argument en PID
    const char *message = argv[2];   // Récupère le message

    printf("Sending to %d.\n", server_pid);
    send_message(server_pid, message);
    printf("Done.\n");

    return 0;
}

