/* Prograam code originally taken from:
   http://man7.org/linux/man-pages/man7/inotify.7.html */

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>

#include "filemanager.h"
#include "strings.h"
#include "mm.h"

static RecoveryDatabase_t rd;

/* Read all available inotify events from the file descriptor 'fd'.
   wd is the table of watch descriptors for the directories in argv.
   argc is the length of wd and argv.
   argv is the list of watched directories.
   Entry 0 of wd and argv is unused. */

static void handle_events(int fd, int *wd)
{
    /* Some systems cannot read integer variables if they are not
       properly aligned. On other systems, incorrect alignment may
       decrease performance. Hence, the buffer used for reading from
       the inotify file descriptor should have the same alignment as
       struct inotify_event. */

    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    size_t i;
    ssize_t len;
    char *ptr;
    char *path, *tmp;

    /* Loop while events can be read from inotify file descriptor. */

    for (;;)
    {

        /* Read some events. */

        len = read(fd, buf, sizeof buf);
        if (len == -1 && errno != EAGAIN)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* If the nonblocking read() found no events to read, then
           it returns -1 with errno set to EAGAIN. In that case,
           we exit the loop. */

        if (len <= 0)
            break;

        /* Loop over all events in the buffer */

        for (ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len)
        {

            event = (const struct inotify_event *)ptr;

            /* Print event type */

            for (i = 0; i < rd.dirs->count; ++i)
                if (wd[i] == event->wd)
                    break;

            tmp = SCat((char *)rd.dirs->storage[i], "/");
            path = SCat(tmp, event->name);
            Mfree(tmp);

            if (event->mask & IN_OPEN)
            {
                printf("IN_OPEN: ");
            }
            if (event->mask & IN_CLOSE_NOWRITE)
            {
                printf("IN_CLOSE_NOWRITE: ");
            }
            if (event->mask & IN_CLOSE_WRITE)
            {
                printf("IN_CLOSE_WRITE: ");
                FMCheckFile(&rd, path);
            }
            if (event->mask & IN_MOVED_FROM)
            {
                printf("IN_MOVED_FROM: ");
                FMCheckFile(&rd, path);
            }
            if (event->mask & IN_MOVED_TO)
            {
                printf("IN_MOVED_TO: ");
                FMCheckFile(&rd, path);
            }
            if (event->mask & IN_DELETE)
            {
                printf("IN_DELETE: ");
                FMCheckFile(&rd, path);
            }
            if (event->mask & IN_CREATE)
            {
                printf("IN_CREATE: ");
                FMCheckFile(&rd, path);
            }
            if (event->mask & IN_DELETE_SELF)
            {
                printf("IN_DELETE_SELF: ");
                FMCheckFile(&rd, path);
            }
            Mfree(path);

            /* Print the name of the watched directory */

            printf("%s/", (char *)rd.dirs->storage[i]);

            /* Print the name of the file */

            if (event->len)
                printf("%s", event->name);

            /* Print type of filesystem object */

            if (event->mask & IN_ISDIR)
                printf(" [directory]\n");
            else
                printf(" [file]\n");
        }
    }
}

int main(int argc, char *argv[])
{
    char buf;
    int fd, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];

    if (argc != 1)
    {
        printf("Usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Press ENTER key to terminate.\n");

    /* Create the file descriptor for accessing the inotify API */

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1)
    {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }

    /**/

    memset(&rd, 0, sizeof(rd));
    FMCreateDatabase(&rd);

    /* Allocate memory for watch descriptors */

    wd = calloc(rd.dirs->count, sizeof(int));
    if (wd == NULL)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Mark directories for events
       - file was opened
       - file was closed */

    for (size_t i = 0; i < rd.dirs->count; i++)
    {
        char *test = (char *)(rd.dirs->storage[i]);
        printf("%s\n", test);
        wd[i] = inotify_add_watch(fd, (char *)(rd.dirs->storage[i]), IN_OPEN | IN_CLOSE | IN_MOVE | IN_DELETE | IN_CREATE | IN_DELETE_SELF);
        if (wd[i] == -1)
        {
            fprintf(stderr, "Cannot watch '%s'\n", (char *)(rd.dirs->storage[i]));
            perror("inotify_add_watch");
            exit(EXIT_FAILURE);
        }
    }

    /* Prepare for polling */

    nfds = 2;

    /* Console input */

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    /* Inotify input */

    fds[1].fd = fd;
    fds[1].events = POLLIN;

    /* Wait for events and/or terminal input */

    printf("Listening for events.\n");
    while (1)
    {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1)
        {
            if (errno == EINTR)
                continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (poll_num > 0)
        {

            if (fds[0].revents & POLLIN)
            {

                /* Console input is available. Empty stdin and quit */

                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN)
            {

                /* Inotify events are available */

                handle_events(fd, wd);
            }
        }
    }

    printf("Listening for events stopped.\n");

    /* Close inotify file descriptor */

    close(fd);

    free(wd);
    exit(EXIT_SUCCESS);
}
