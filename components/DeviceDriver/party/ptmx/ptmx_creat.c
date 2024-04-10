#include "ptmx_creat.h"
#include <string.h>
#if defined(__linux__) || defined(__GLIBC__) || defined(__GNU__)
#define _GNU_SOURCE /* GNU glibc grantpt() prototypes */
#endif

struct ptmx_creat_t* ptmx_creat()
{
	struct ptmx_creat_t* p = (struct ptmx_creat_t*)malloc(sizeof(struct ptmx_creat_t));
	memset(p, 0, sizeof(struct ptmx_creat_t));
}

int ptmx_destroy(struct ptmx_creat_t **p)
{
	if((p == NULL) || (*p == NULL)) return -1;
	if((*p)->ptmx_status == PTMX_RUN)
	{
		ptmx_exit(*p);
	}
	free(*p);
	*p = NULL;
	return 0;
}


int ptmx_open(struct ptmx_creat_t *self)
{
	char slavename[50];
	self->master_ptmx = open("/dev/ptmx", O_RDWR | O_NOCTTY);
	if(self->master_ptmx <= 0)
	{
		return self->master_ptmx;
	}
	grantpt(self->master_ptmx);
	unlockpt(self->master_ptmx);
	ptsname_r(self->master_ptmx, slavename, 50);
	strcpy(self->slave_ptmx_name, slavename);
	self->ptmx_status = PTMX_RUN;
	pthread_create(&self->ptmx_loop_handler_d, NULL, ptmx_loop_handler, self);
	return self->master_ptmx;
}

void ptmx_set_msg_callback(struct ptmx_creat_t *self, msg_call_back_t fun)
{
	self->call = fun;
}

void *ptmx_loop_handler(void *par)
{
	struct ptmx_creat_t* ptmx = (struct ptmx_creat_t*)par;

	char *buff = (char*)malloc(ptmx->buff_size);
	do
	{
		int size = read(ptmx->master_ptmx, buff, ptmx->buff_size);
		if (size > 0)
		{
			if (ptmx->ptmx_status != PTMX_STOP && ptmx->call != NULL)
			{
				buff[size] = '\0';
				ptmx->call(buff, size);
			}
		}
		else
		{
			usleep(100000);
		}
	} while (ptmx->ptmx_status);
	free(buff);
	return NULL;
}

void ptmx_exit(struct ptmx_creat_t *self)
{
	self->ptmx_status = PTMX_STOP;
	char tmp_buff[128];
	sprintf(tmp_buff, "echo \"exit\" >> %s", self->slave_ptmx_name);
	system(tmp_buff);
	pthread_join(self->ptmx_loop_handler_d, NULL);
	close(self->master_ptmx);
}

#if defined(__linux__) || defined(__GLIBC__) || defined(__GNU__)
#undef name _GNU_SOURCE /* GNU glibc grantpt() prototypes */
#endif
