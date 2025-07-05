/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * 
 * Compilation: "gcc -o get_temp main.c"
 * Launch: "./get_temp"
 * 
 * @version 0.1
 * @date 2025-07-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/select.h>

#define BAUD_RATE B9600
#define CMD_STR "get_temp\n"

int try_open_port(const char *dev_path) {
    int fd = open(dev_path, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) return -1;

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

int find_usb_serial_port(char *out_path, size_t out_size) {
    DIR *dir = opendir("/dev");
    if (!dir) {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "ttyUSB", 6) == 0) {
            snprintf(out_path, out_size, "/dev/%s", entry->d_name);
            closedir(dir);
            return 0;
        }
    }

    closedir(dir);
    return -1;
}

int main() {
    char port_path[256];

    if (find_usb_serial_port(port_path, sizeof(port_path)) != 0) {
        fprintf(stderr, "UART порт не найден (ttyUSB*)\n");
        return 1;
    }

    int fd = try_open_port(port_path);
    if (fd == -1) {
        perror("Не удалось открыть порт");
        return 1;
    }

    // Отправка команды "get_temp\n"
    ssize_t written = write(fd, CMD_STR, strlen(CMD_STR));
    if (written <= 0) {
        perror("Ошибка записи команды");
        close(fd);
        return 1;
    }
    sleep(1);   // Pause to allow the device to process the command
    // Ожидание ответа
    fd_set read_fds;
    struct timeval timeout;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int ready = select(fd + 1, &read_fds, NULL, NULL, &timeout);
    if (ready > 0 && FD_ISSET(fd, &read_fds)) {
        char buf[128] = {0};
        int n = read(fd, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            printf("%s", buf);  // выводим ответ без добавления лишнего \n
        } else {
            fprintf(stderr, "Ошибка чтения ответа\n");
        }
    } else {
        fprintf(stderr, "Нет ответа от устройства (таймаут)\n");
    }

    close(fd);
    return 0;
}
