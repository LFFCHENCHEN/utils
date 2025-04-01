#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_EVENTS 10
#define BUF_SIZE 4096

// 定义回调函数类型
typedef void (*io_callback)(int fd, void *arg);

// 事件处理器结构体
struct event_handler
{
    int fd;
    io_callback callback;
    void *arg;
    struct epoll_event ev;
};

// 全局事件循环控制
static int epoll_fd;
static struct epoll_event events[MAX_EVENTS];

// 初始化事件循环
void event_loop_init()
{
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
}

// 设置非阻塞IO
static void set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 注册事件处理器
void register_handler(int fd, uint32_t events_mask, io_callback cb, void *arg)
{
    struct event_handler *handler = malloc(sizeof(struct event_handler));
    handler->fd = fd;
    handler->callback = cb;
    handler->arg = arg;

    set_nonblocking(fd);
    handler->ev.events = events_mask | EPOLLET; // 默认使用边缘触发
    handler->ev.data.ptr = handler;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &handler->ev) == -1)
    {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

// 事件处理循环
void event_loop_run()
{
    while (1)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            if (errno == EINTR)
                continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; ++i)
        {
            struct event_handler *h = events[i].data.ptr;
            h->callback(h->fd, h->arg); // 触发回调
        }
    }
}

/******************** 使用示例 ********************/
void stdin_callback(int fd, void *arg)
{
    char buf[BUF_SIZE];
    while (1)
    {
        ssize_t count = read(fd, buf, sizeof(buf));
        if (count > 0)
        {
            write(STDOUT_FILENO, "RECV: ", 6);
            write(STDOUT_FILENO, buf, count);
        }
        else if (count == -1 && errno == EAGAIN)
        {
            break;
        }
        else
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
    }
}

int main()
{
    event_loop_init();
    register_handler(STDIN_FILENO, EPOLLIN, stdin_callback, NULL);
    event_loop_run();
    return 0;
}
