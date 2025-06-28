#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdint.h>
#include <inttypes.h>
#define CLUSTER_SIZE 4096

//recommandated structure:
struct aio_operation {
    struct aiocb aio;
    char *buffer;
    int write_operation;
    void* next_operation;
};

//opening and creation of file-open

/*
    open(read_filename, O_RDONLY | O_NONBLOCK, 0666)
    open(write_filename, O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0666)
*/

//получение размера файла - через функцию fstat

//вызов асинхронного чтения - aio_read
//вызов асинхронной записи - aio_write
//проверка прочитанных байтов - aio_return
//ожидание асинхронных операций - aio_suspend
//функция завершения (сами пишете, шаблон ниже)

void perform_async_copy(char* source_file, char* destination_file, size_t block_size, int num_operation){
    int read_fd = open(source_file, O_RDONLY | O_NONBLOCK, 0666)
    
}

void aio_completion_handler(sigval_t sigval) {
    struct aio_operation *aio_op = (struct aio_operation *)sigval.sival_ptr;
    
    if (aio_op->write_operation) {
    // операция записи
    } else {
    //операция чтения
    }
}

//Можно (лайфхак) для того, чтобы поймать конец копирования, использовать функции raise / sleep

int main(int argc, char** argv){
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <source_file> <destination_file> <cluster_multiplier> <num_operations> \n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char* source_file = argv[1];
    const char* dest_file = argv[2];
    size_t cluster_multiplier = (size_t)(atoi(argv[3]));
    size_t block_size = cluster_multiplier * CLUSTER_SIZE;
    int num_operation = atoi(argv[4]);

    perfom_async_copy(source_file, dest_file, block_size, num_operation);
}