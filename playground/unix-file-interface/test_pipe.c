#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    int pid;
    int i = 0;
    int pipefds[2];
    char buf[30];

    memset(buf, 0, 30);
    // create pipe
    if (pipe(pipefds) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if(fork())
    {
        close(pipefds[0]);
        //write to pipe
        printf("writing to file descriptor #%d\n", pipefds[1]);
        write(pipefds[1], "CS323", 6);
        close(pipefds[1]);

        wait(NULL);
    }
    else
    {
        close(pipefds[1]);
        //read from pipe
        printf("reading from file descriptor #%d\n", pipefds[0]);
        while(i < 6)
        {
            if(read(pipefds[0], buf+i, 1))
                i++;
        }
        printf("read \"%s\"\n", buf);
        close(pipefds[0]);

        exit(EXIT_SUCCESS);
    }
    return 0;
}
