#define _GNU_SOURCE

#include <stdio.h>  
#include <stdlib.h>
#include <fcntl.h>       // open
#include <unistd.h>      // close, fstat, access
#include <aio.h>         // aio_read, aio_write
#include <sys/stat.h>    // fstat
#include <sys/statvfs.h> // statvfs
#include <string.h>      // strerror
#include <time.h>        // clock_gettime
#include <errno.h>       // errno
#include <signal.h>      // sigval_t

// Structure pour gérer les opérations asynchrones
typedef struct async_op {
    struct aiocb aio;          // Structure pour les opérations asynchrones
    void* data_buffer;         // Buffer pour les données (aligné)
    int is_write;              // 1 pour écriture, 0 pour lecture
} async_op;

// Gestionnaire de fin d'opération asynchrone
void handle_completion(sigval_t signal) {
    async_op* operation = (async_op*)signal.sival_ptr;

    if (operation->is_write) {
        ssize_t bytes_written = aio_return(&operation->aio);
        if (bytes_written == -1) {
            fprintf(stderr, "Write operation failed: %s T_T\n", strerror(errno));
        }
    } else {
        ssize_t bytes_read = aio_return(&operation->aio);
        if (bytes_read == -1) {
            fprintf(stderr, "Read operation failed: %s T_T\n", strerror(errno));
        }
    }
}


// Créer un fichier et le remplir de données aléatoires
void generate_random_file(const char* filename, size_t size) {
    int file_descriptor = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (file_descriptor == -1) {
        fprintf(stderr, "Failed to create the file: %s T_T\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char* buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed T_T\n");
        close(file_descriptor);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; ++i) {
        buffer[i] = 'A' + rand() % ('Z' - 'A' + 1); // Lettres majuscules aléatoires
    }

    size_t total_written = 0;
    while (total_written < size) {
        ssize_t bytes_written = write(file_descriptor, buffer + total_written, size - total_written);
        if (bytes_written == -1) {
            fprintf(stderr, "Failed to write to file: %s T_T\n", strerror(errno));
            free(buffer);
            close(file_descriptor);
            exit(EXIT_FAILURE);
        }
        total_written += bytes_written;
    }

    free(buffer);
    close(file_descriptor);
    printf("File created and filled with random data: %s\n", filename);
}

// Copie asynchrone principale
void perform_async_copy(const char* input_file, const char* output_file, size_t block_size, int num_ops) {
    // Vérifier si le fichier source existe
    if (access(input_file, F_OK) == -1) {
        printf("Source file does not exist. Creating and filling with random data...\n");
        size_t default_size = 64 * 1024 * 1024; // 64 Mo par défaut
        generate_random_file(input_file, default_size);
    }

    // Ouvrir le fichier source en lecture
    int read_fd = open(input_file, O_RDONLY);
    if (read_fd == -1) {
        fprintf(stderr, "Failed to open source file: %s T_T\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Ouvrir le fichier de destination en écriture
    int write_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (write_fd == -1) {
        close(read_fd);
        fprintf(stderr, "Failed to open destination file: %s T_T\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Obtenir la taille du fichier source
    struct stat file_info;
    if (fstat(read_fd, &file_info) == -1) {
        close(read_fd);
        close(write_fd);
        fprintf(stderr, "Failed to get file size: %s T_T\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    off_t file_size = file_info.st_size;
    off_t remaining_bytes = file_size;

    // Initialiser le pool d'opérations asynchrones
    async_op operations[num_ops];
    memset(operations, 0, sizeof(async_op) * num_ops);

    for (int i = 0; i < num_ops; ++i) {
        if (posix_memalign(&operations[i].data_buffer, block_size, block_size) != 0) {
            fprintf(stderr, "Failed to allocate aligned buffer T_T\n");
            exit(EXIT_FAILURE);
        }
    }

    /*
        Cette fonction alligne un block de taille block_size, pour un nombre d'operations  
    */

    // Positions pour lecture/écriture
    off_t read_pos = 0;
    off_t write_pos = 0;

    // Mesurer le temps d'exécution
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Boucle de copie asynchrone
    while (remaining_bytes > 0) {  //Erreur
        // Lancer les opérations de lecture
        for (int i = 0; i < num_ops /*&& remaining_bytes > 0*/; ++i) { //bizarre
            async_op* op = &operations[i];
            op->is_write = 0;
            op->aio.aio_fildes = read_fd;
            op->aio.aio_buf = op->data_buffer;
            op->aio.aio_nbytes = remaining_bytes < block_size ? remaining_bytes : block_size;
            op->aio.aio_offset = read_pos;
            op->aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
            op->aio.aio_sigevent.sigev_notify_function = handle_completion;
            op->aio.aio_sigevent.sigev_value.sival_ptr = op;

            if (aio_read(&op->aio) == -1) {
                fprintf(stderr, "Failed to initiate read operation: %s T_T\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            read_pos += op->aio.aio_nbytes;
            remaining_bytes -= op->aio.aio_nbytes;
        }

        // Attendre la fin des lectures
        for (int i = 0; i < num_ops; ++i) {
            while (aio_error(&operations[i].aio) == EINPROGRESS) {
                // Attendre
            }

            ssize_t bytes_read = aio_return(&operations[i].aio);
            if (bytes_read == -1) {
                fprintf(stderr, "Read operation failed: %s T_T\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            // Lancer l'écriture
            operations[i].is_write = 1;
            operations[i].aio.aio_fildes = write_fd;
            operations[i].aio.aio_offset = write_pos;

            if (aio_write(&operations[i].aio) == -1) {
                fprintf(stderr, "Failed to initiate write operation: %s T_T\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            write_pos += bytes_read;
        }

        // Attendre la fin des écritures
        for (int i = 0; i < num_ops; ++i) {
            while (aio_error(&operations[i].aio) == EINPROGRESS) {
                // Attendre
            }

            ssize_t bytes_written = aio_return(&operations[i].aio);
            if (bytes_written == -1) {
                fprintf(stderr, "Write operation failed: %s T_T\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }

    // Libérer les buffers
    for (int i = 0; i < num_ops; ++i) {
        free(operations[i].data_buffer);
    }

    close(read_fd);
    close(write_fd);

    // Calculer le temps total
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double duration = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                      (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;

    printf("File copied successfully.\n");
    //printf("Cluster size: %zu bytes\n", cluster_size);
    printf("Block size: %zu bytes \n", block_size);
    printf("Number of concurrent operations: %d\n", num_ops);
    printf("Total time taken: %.2f ms\n", duration);
}

int main(int argc, char** argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file> <cluster_multiplier> <num_operations>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* source_file = argv[1];
    const char* dest_file = argv[2];
    size_t block_size = strtoul(argv[3], NULL, 10);
    int num_operations = atoi(argv[4]);
    int write_fd = open(dest_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (write_fd == -1) 
    {
        fprintf(stderr, "Failed to open or create destination file '%s': %s T_T\n", dest_file, strerror(errno));
        exit(EXIT_FAILURE);
    }
    

    perform_async_copy(source_file, dest_file, block_size, num_operations);

    return EXIT_SUCCESS;
}

/*
    Le fichier out a des lignes supplementaires.
*/