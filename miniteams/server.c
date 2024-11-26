#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>


// Variable globale pour accumuler les bits reçus
int received_bits[] = {2, 2, 2, 2, 2, 2, 2, 2};

void signal_handler(int sig) {

    int i = 0;
    
    // On remplit la variable global selon le signal reçu
    for (i = 0; i < 8; i++) {
    	if (received_bits[i] == 2) {
    		if (sig == SIGUSR1) {
    			received_bits[i] = 0;
    		} else {
    			received_bits[i] = 1;
    		}
    		break;	
    	}
    }
}

int main() {
    printf("Miniteams starting…\n");
    printf("My PID is %d\n", getpid());
    printf("Waiting for new messages...\n");
    

// Ouvrir le fichier pour écrire l'historique
    FILE *log_file = NULL;
    log_file = fopen("conversations.log", "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }


    // Configurer les signaux
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    // Boucle principale pour attendre les signaux
    char message_time[50];
    char current_char = 0;
    char message[1000] = "";
    int is_char_full = 0;
    int i = 0;
    int k = 0;
    while (1) {
    
    	// On vérifie si tous les bits du caractère ont été reçu
    	for (i = 0; i < 8; i++) {
    		if (received_bits[i] != 2) {
    			if (i == 7) {
    				is_char_full = 1;
    			}
    				
    		} else {
    			i = 8;
    		}
    	}
    
    
        // Quand 8 bits sont reçus, afficher le caractère
	    if (is_char_full) {
	    
	    	for (i = 0; i < 8; i++) {
	    		current_char <<= 1; // Ajouter un bit au caractère courant
			if (received_bits[i] == 1) {
				current_char |= 1; // Ajoute un bit à 1 si signal SIGUSR2
			}
			
	    	}   
	    
		if (current_char == '\0') { // Si le caractère est nul, on finit le message
		
		    printf("[From client ");
		    
		    // Le début du message correspond au PID du client
		    i = 0;
		    while (message[i] != 124) { // Tant que le caractère '|' n'apparait pas il s'agit du PID
		    	putchar(message[i]);
		    	i++;
		    }
		    i++; // On saute le caractère '|'
		    
		    printf("] ");
		
		    // On récupère l'heure locale pour la mettre au début de la ligne dans le fichier log
		    time_t t = time(NULL);
    		    struct tm *current_time = localtime(&t);

    		    strftime(message_time, 50, "[%H:%M:%S] ", current_time );
		
		    fprintf(log_file, "%s ", message_time);
		    
		    // On affiche le message dans le terminal tout en le sauvegardant dans le fichier log
		    while (message[i] != '\0') {
		    	putchar(message[i]);
		    	fprintf(log_file, "%c", message[i]);
		    	i++;
		    }
		    
		    printf("\n");
		    fprintf(log_file, "\n");
		    fflush(log_file);       // Forcer l'écriture dans le fichier

		    // On vide la string message
		    for (k = strlen(message) - 1; k >= 0; k--) {
	    		message[k] = 0;
			
	    	    }
		    
		} else { // Tant que le caractère n'est pas nul on le garde dans une string
		    message[strlen(message)] = current_char;
		}
		// Réinitialisation pour le prochain caractère
		current_char = 0;
		received_bits[0] = 2;
		received_bits[1] = 2;
		received_bits[2] = 2;
		received_bits[3] = 2;
		received_bits[4] = 2;
		received_bits[5] = 2;
		received_bits[6] = 2;
		received_bits[7] = 2;
		is_char_full = 0;
	    }
	    
    
        pause(); // Attendre un signal
        
        
        
        
        
    }

    return 0;
}

