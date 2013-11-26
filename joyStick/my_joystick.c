#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <linux/joystick.h>
#include "listop.h"
#include "RobotSocket.h"

#if 1
#define LOG_DBG(fmt, ...)  fprintf(stdout, fmt, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...)
#endif

#define LOG_ERR(fmt, ...)  fprintf(stderr, fmt, ##__VA_ARGS__)


typedef struct _joy_stick_ctx {
    struct list_head list;
    int i4_js_fd;
    unsigned int i4_op_block;
} JOYSTICK_CTX_T;

LIST_HEAD(_t_js_ctx_head);
/*==>  struct list_head _t_js_ctx_head = {&_t_js_ctx_head, &_t_js_ctx_head};*/

int joystick_open(char* cp_js_dev_name, int i4_block)
{
    int i4_open_flags = O_RDONLY;
    JOYSTICK_CTX_T*  pt_joystick_ctx = NULL;

    if (!cp_js_dev_name) {
        LOG_ERR("[%s] js device name is NULL\n", __func__);
        return -1;
    }

    pt_joystick_ctx = (JOYSTICK_CTX_T*)calloc(sizeof(JOYSTICK_CTX_T), 1);
    if (!pt_joystick_ctx) {
        LOG_ERR("[%s] no memory!!\n", __func__);
        return -1;
    }

    pt_joystick_ctx->i4_op_block = i4_block ? 1 : 0;

    if (pt_joystick_ctx->i4_op_block == 0) {
        i4_open_flags |= O_NONBLOCK;
    }

    pt_joystick_ctx->i4_js_fd = open(cp_js_dev_name, i4_open_flags);
    if (pt_joystick_ctx->i4_js_fd < 0) {
        LOG_ERR("[%s] open device %s error\n", __func__, cp_js_dev_name);
        free(pt_joystick_ctx);
        return -1;
    }

    list_add_tail(&pt_joystick_ctx->list, &_t_js_ctx_head);

    return pt_joystick_ctx->i4_js_fd;
}

int joystick_close(int i4_fd)
{
    struct list_head* pt_entry;
    struct list_head* pt_next;
    JOYSTICK_CTX_T* pt_node;

    if (list_empty(&_t_js_ctx_head)) {
       LOG_ERR("[%s] device not opened\n", __func__);
        return -1;
    }

    list_for_each_safe(pt_entry, pt_next, &_t_js_ctx_head) {
        pt_node = list_entry(pt_entry, JOYSTICK_CTX_T, list);
        if (pt_node->i4_js_fd == i4_fd) {
            list_del_init(&pt_node->list);
            free(pt_node);
            return close(i4_fd);
        }
    }

    LOG_ERR("[%s] i4_fd=%d invalid\n", __func__, i4_fd);
    return -1;
}

int joystick_read_one_event(int i4_fd, struct js_event* tp_jse)
{
    int i4_rd_bytes;


    i4_rd_bytes = read(i4_fd, tp_jse, sizeof(struct js_event));

    if (i4_rd_bytes == -1) {
        if (errno == EAGAIN) { 
            return 0;
        }
        else {
            return -1;
        }
    }

    return i4_rd_bytes;
}

int joystick_read_ready(int i4_fd)
{
    int i4_block = 2;
    struct list_head* pt_entry;
    JOYSTICK_CTX_T* pt_node;

    if (list_empty(&_t_js_ctx_head)) {
        LOG_ERR("[%s] device not opened\n", __func__);
        return -1;
    }

    list_for_each(pt_entry, &_t_js_ctx_head) {
        pt_node = list_entry(pt_entry, JOYSTICK_CTX_T, list);
        if (pt_node->i4_js_fd == i4_fd) {
            i4_block = pt_node->i4_op_block;
            break;
        }
    }
    if (i4_block == 2) {
        LOG_ERR("[%s] i4_fd=%d invalid\n", __func__, i4_fd);
        return 0;
    }
    else if (i4_block == 1) {
        fd_set readfd;
        int i4_ret = 0;
//        struct timeval timeout = {0, 0};
        struct timeval timeout = {0, 500000};
        FD_ZERO(&readfd);
        FD_SET(i4_fd, &readfd);

        i4_ret = select(i4_fd + 1, &readfd, NULL, NULL, &timeout);

        if (i4_ret > 0 && FD_ISSET(i4_fd, &readfd)) {
            return 1;
        }
        else {
            return 0;
        }

    }

    return 1; 
}


void debug_list(void)
{
    if (! list_empty(&_t_js_ctx_head)) {
        struct list_head* pt_entry;
        JOYSTICK_CTX_T* pt_node;

        list_for_each(pt_entry, &_t_js_ctx_head) {
            pt_node = list_entry(pt_entry, JOYSTICK_CTX_T, list);
            LOG_DBG("fd:%d--block:%d\n", pt_node->i4_js_fd, pt_node->i4_op_block);
        }
    }
    else {
        LOG_DBG("-----------> EMPTY NOW\n");
    }
}

#if 1
typedef struct _axes_t {
    int x;
    int y;
} AXES_T;
/* Power by Renzhenwen 2013-01-9*/
int main(int argc, char* argv[])
{
    int fd, rc;
    int op_times = 0;
    char number_of_axes = 0;
    char number_of_btns = 0;
    char js_name_str[128];
    unsigned int buttons_state = 0;
    AXES_T* tp_axes = NULL;
    int i, print_init_stat = 1/*0*/;  //��־Ϊ�Ƿ��ӡ��ʼ��״̬
		int len;

    struct js_event jse;

RobotSocket *clientSocket = new RobotSocket("192.168.1.51", 7777, RobotSocket::TCP);
	//RobotSocket *clientSocket = new RobotSocket("127.0.0.1", 9002, RobotSocket::TCP);
	pause();
	#define LEN 100
	unsigned char buf[LEN];//11�ֽڵĿ��ƽṹ
	buf[0] = 0xfb;buf[1] = 0x03;buf[2] = 0x01;buf[3] = 0x33;
	buf[4] = 0x00;buf[5] = 0x00;buf[6] = 0x02;buf[7] = 0x00;
	buf[8] = 0x01;buf[9] = 0x01;buf[10] = 0xfe;
    fd = joystick_open("/dev/input/js0", 1); 
    if (fd < 0) {
        LOG_ERR("open failed.\n");
        exit(1);
    }

    rc = ioctl(fd, JSIOCGAXES, &number_of_axes);
    if (rc != -1) {
        LOG_DBG("number_of_axes:%d\n", number_of_axes);
        if (number_of_axes > 0) {
            tp_axes = (AXES_T*)calloc(sizeof(AXES_T), 1);
        }
    }

    rc = ioctl(fd, JSIOCGBUTTONS, &number_of_btns);
    if (rc != -1) {
        LOG_DBG("number_of_btns:%d\n", number_of_btns);
    }

    if (ioctl(fd, JSIOCGNAME(sizeof(js_name_str)), js_name_str) < 0) {
        LOG_DBG(js_name_str, "Unknown", sizeof(js_name_str));
    }

    LOG_DBG("joystick Name: %s\n", js_name_str);

	bool flag = false;
    for (;;) 
	{
        if (joystick_read_ready(fd)) {
			/*if(sta == -1 )  //power by zhenwenRen
			{
				if(flag) {
					clientSocket->write(buf, 12);
        			select(1, NULL, NULL, NULL, &timeout);
				}
				else
					select(1, NULL, NULL, NULL, &timeout_long);
				continue;
			}*/
            rc = joystick_read_one_event(fd, &jse);
            if (rc > 0) {
                if ((jse.type & JS_EVENT_INIT) == JS_EVENT_INIT) {
                    if ((jse.type & ~JS_EVENT_INIT) == JS_EVENT_BUTTON) {
                        if (jse.value) {
                            buttons_state |= (1 << jse.number);
                        }
                        else {
                            buttons_state &= ~(1 << jse.number);
                        }
                    }
                    else if ((jse.type & ~JS_EVENT_INIT) == JS_EVENT_AXIS) {
                        if (tp_axes) {
                            if ((jse.number & 1) == 0) {
                                tp_axes[jse.number / 2].x = jse.value;
                            }
                            else {
                                tp_axes[jse.number / 2].y = jse.value;
                            }
                        }

                    }
                }/* Power by Renzhenwen 2013-01-9*/
                else {
                   // op_times++;
                    if (print_init_stat == 0) {
                        for (i = 0; i < number_of_btns; i++) {
                            LOG_DBG("joystick init state: button %d is %s.\n", i, ((buttons_state & (1 << i)) == (1 << i)) ? "DOWN" : "UP");
                        }

                        if (tp_axes)
                            for (i = 0; i < number_of_axes; i++) {
                                LOG_DBG("joystick init state: axes %d is x=%d  y=%d.\n", i, tp_axes[i].x, tp_axes[i].y);
                            }
                        print_init_stat = 1;
                    }

                    if (jse.type  == JS_EVENT_BUTTON) {
                        if (jse.value) {
                            buttons_state |= (1 << jse.number);
                        }
                        else {
                            buttons_state &= ~(1 << jse.number);
                        }
						
							if(0 == jse.number)
							{
								if(((buttons_state & (1 << jse.number)) == (1 << jse.number)))
								{
									buf[4] = 0x01;buf[5] = 0x08;buf[6] = 0x02;buf[7] = 0x01;buf[8]=0xfe;
									len = 11;
								}
								else
								{/*	
									flag = false;
									buf[1] = 0x01;buf[2] = 0x01;buf[3] = 0x33;
									buf[4] = 0x08;buf[5] = 0x08;buf[6] = 0x00;buf[7] = 0x02;buf[8]=0xfe;
									len = 9;*/
								}
							}
							else if(1 == jse.number)
							{
								if(((buttons_state & (1 << jse.number)) == (1 << jse.number)))
								{
									flag = true; 
									buf[4] = 0x01;buf[5] = 0x01;buf[6] = 0x02;buf[7] = 0x01;buf[8] = 0x01;buf[9]=0xfe;
									len = 11;
								}
								else
								{
									flag = false;
									buf[4] = 0x01;buf[5] = 0x01;buf[6] = 0x02;buf[7] = 0x01;buf[8] = 0x02;buf[9]=0xfe;
									len = 11;
								}
							}
							else if(2 == jse.number)
							{
								if(((buttons_state & (1 << jse.number)) == (1 << jse.number)))
								{
									flag = true; 
									buf[4] = 0x01;buf[5] = 0x02;buf[6] = 0x02;buf[7] = 0x03; buf[8] = 0x03;buf[9] = 0x01;buf[10]=0xfe;
									len = 11;
								}
								else
								{	
									flag = false;
									buf[4] = 0x01;buf[5] = 0x02;buf[6] = 0x02;buf[7] = 0x03; buf[8] = 0x03;buf[9] = 0x01;buf[10]=0xfe;
									len =11;
								}
							}
							else if(3 == jse.number)
							{
								if(((buttons_state & (1 << jse.number)) == (1 << jse.number)))
								{
									flag = true; 
									buf[4] = 0x01;buf[5] = 0x02;buf[6] = 0x02;buf[7] = 0x01;buf[8]=0xfe;
									len = 11;
								}
								else
								{	
									flag = false;
									buf[4] = 0x01;buf[5] = 0x02;buf[6] = 0x02;buf[7] = 0x02;buf[8]=0xfe;
									len = 11;
								}
							}
							else if((4 == jse.number)||(6 == jse.number))
							{

								if((buttons_state & (1 << jse.number)) == (1 << jse.number)){
								flag = false;
								buf[4] = 0x0c;buf[5]=0x08;buf[6]=0x02;buf[7]=0x00;buf[8]=0x0e;
								len = 11;
								}
								else
								{
									flag = false;
								 	continue;
								}
							}
							else
							{
								flag = false;
								buf[4] = 0x0C;buf[5] = 0x00;buf[6] = 0x02;buf[7] = 0x00;buf[8]=0x0e;
								len = 11;
							}
							clientSocket->write(buf, len);
                       // LOG_DBG("joystick state: button %d is %s.\n", jse.number+1, ((buttons_state & (1 << jse.number)) == (1 << jse.number)) ? "DOWN" : "UP");
             }/* Power by Renzhenwen 2013-01-9*/
                    else if (jse.type == JS_EVENT_AXIS) {
                        if (tp_axes) {
                            if ((jse.number & 1) == 0) {
                                tp_axes[jse.number / 2].x = jse.value;
                            }
                            else {
                                tp_axes[jse.number / 2].y = jse.value;
                            }
                         //   LOG_DBG("joystick state: axes %d is x=%d  y=%d.\n", jse.number / 2, tp_axes[jse.number / 2].x, tp_axes[jse.number / 2].y);
                        }
                        else {
                           // LOG_DBG("joystick state: axes %d is %s=%d.\n", jse.number / 2, ((jse.number & 1) == 0) ? "x" : "y", jse.value);
                        }
                    }
                }

                //if (op_times >= 18) {
                    //break;
                //}
            }
        }
		else
		{
			if(flag)
			clientSocket->write(buf, len);
		}
        usleep(1000);
    }

	printf("Closing.............\n");
    joystick_close(fd);

    if (tp_axes) {
        free(tp_axes);
    }

	clientSocket->close();
    return 0;
}
#endif

