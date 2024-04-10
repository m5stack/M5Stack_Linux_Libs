#ifndef __MY_HAL_H__
#define __MY_HAL_H__

typedef struct
{
    int pipeid;        // pipeline 的 id
    int m_output_type; // 输出的类型
    // 图像或者buffer的一些参数
    int n_width, n_height, n_size, n_stride;
    int d_type;
    void *p_vir;
    unsigned long long int p_phy;
    unsigned long long int n_pts;
    void *p_pipe; // pipeline_t 结构体指针
} hal_buffer_t;


typedef enum {
    DEVICE_NONE = 0,
    DEVICE_RUN
} hal_run_status_t;



#endif