#ifndef __PTMX_CREAT_H__
#define __PTMX_CREAT_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef enum {
	PTMX_STOP = 0,
	PTMX_RUN = 1
} ptmx_status_t;

/*msg call back */
typedef void (*msg_call_back_t)(const char*, int);
/* a ptmx device structure */
struct ptmx_creat_t 
{
	int master_ptmx;
	char slave_ptmx_name[50];
	int ptmx_status;			
	pthread_t ptmx_loop_handler_d;
	msg_call_back_t call;
	int buff_size;
};
/*creat a ptmx struct ptmx_creat_t */
struct ptmx_creat_t* ptmx_creat();
/*destroy ptmx struct ptmx_creat_t */
int ptmx_destroy(struct ptmx_creat_t **p);
/*open one ptmx*/
int ptmx_open(struct ptmx_creat_t *self);

/*set msg callback fun*/
void ptmx_set_msg_callback(struct ptmx_creat_t *self, msg_call_back_t fun);
void *ptmx_loop_handler(void *par);
/*exit one ptmx*/
void ptmx_exit(struct ptmx_creat_t *self);

#ifdef __cplusplus
}
#include <string>
#include <cstring>
class ptmx
{
private:
	struct ptmx_creat_t ptm;
	int master_fd;
public:
	ptmx()
	{
		memset(&ptm, 0, sizeof(struct ptmx_creat_t));
		ptm.buff_size = 1024 * 4;
	}
	int open()
	{
		master_fd = ptmx_open(&ptm);
		return master_fd;
	}
	int get_mster_fd()
	{
		return master_fd;
	}
	void set_buff_size(int size)
	{
		ptm.buff_size = size;
	}
	void set_msg_call_back(msg_call_back_t msg)
	{
		ptm.call = msg;
	}
	std::string get_slave_ptmx_name()
	{
		return std::string(ptm.slave_ptmx_name);
	}
	void exit()
	{
		if(ptm.ptmx_status)
		{
			ptmx_exit(&ptm);
		}
	}
	~ptmx()
	{
		exit();
	}
};
#endif // __cplusplus

#endif //__PTMX_CREAT_H__