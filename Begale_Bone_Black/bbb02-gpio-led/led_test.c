#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd;
    char buf[8] = "";
    if(argc != 2)
    {
        printf("invalid command line args.\n");
        printf("syntax: %s ON|OFF|GET \n",argv[0]);
        _exit(1);
    }

    fd = open("/dev/myled", O_RDWR);
    if(fd < 0)
    {
        perror("failed to open device");
        _exit(1);
    }

    if (strcmp(argv[1], "ON") == 0)
        write(fd, "1", 2);
    else if (strcmp(argv[1], "OFF") == 0)
        write(fd, "0", 2);
    else if(strcmp(argv[1], "GET") == 0)
    {
        read(fd, buf, sizeof(buf));
        printf("%s\n",buf); //1 or 0
    }
    else
        printf("syntax: %s ON|OFF|GET\n", argv[0]);
    close(fd);
    return 0;

}