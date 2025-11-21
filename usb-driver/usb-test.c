#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int main()
{
    int ret , fd = open("/dev/desd3", O_RDWR);
    char buf[32];
    printf(" open() returned: %d\n",fd);
    ret = write(fd, "", 1);
    if( ret < 0)
        perror("write() failed\n");
    else
        printf("write() returned: %d\n", ret);
    ret = read(fd, buf, 32);
    if(ret <0 )
        perror("read() failed\n");
    else
        printf("read() returned: %d\n",ret);
    close(fd);
    return 0;

}