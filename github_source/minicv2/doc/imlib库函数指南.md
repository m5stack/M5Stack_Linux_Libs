- 常用函数列表
~~~ c
// 库的初始化函数
void imlib_init_all();
void imlib_deinit_all();

// 创建一个堆区图片
image_t* imlib_image_create(int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels, bool is_data_alloc);
// 创建一个临时的栈内存图片
image_t* imlib_image_create_fb_buff(int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels, bool is_data_alloc);
// 释放栈内存图片
void imlib_image_destroy_fb_buff(image_t **obj);
// 释放堆内存图片
void imlib_image_destroy(image_t **obj);
// 初始化一张图片
void imlib_image_init(image_t *ptr, int w, int h, pixformat_t pixfmt, uint32_t size, void *pixels);
// 复制一张图片
void image_copy(image_t *dst, image_t *src);
// 获取图片的大小
size_t image_size(image_t *ptr);


// 转换图片的格式，兼容截图的功能
void imlib_pixfmt_to(image_t *dst, image_t *src, rectangle_t *roi_i);

// 图片的缩放
void imlib_image_resize(image_t *dst, image_t *src, int hist);

// 获取图像的某个像素
int imlib_get_pixel(image_t *img, int x, int y);
//快速获取图像的某个像素，以行为单位
int imlib_get_pixel_fast(image_t *img, const void *row_ptr, int x);
//设置图片的某个像素
void imlib_set_pixel(image_t *img, int x, int y, int p);
// 在图像上画线
void imlib_draw_line(image_t *img, int x0, int y0, int x1, int y1, int c, int thickness);
// 在图像上画框
void imlib_draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c, int thickness, bool fill);
// 在图像上画圆
void imlib_draw_circle(image_t *img, int cx, int cy, int r, int c, int thickness, bool fill);
// 在图像上画椭圆
void imlib_draw_ellipse(image_t *img, int cx, int cy, int rx, int ry, int rotation, int c, int thickness, bool fill);
// 在图像上写字
void imlib_draw_string(image_t *img, int x_off, int y_off, const char *str, int c, float scale, int x_spacing, int y_spacing, bool mono_space,
                       int char_rotation, bool char_hmirror, bool char_vflip, int string_rotation, bool string_hmirror, bool string_hflip);
// 快速的贴图
void imlib_draw_image_fast(image_t *img, image_t *other, int x_off, int y_off, float x_scale, float y_scale, float alpha, image_t *mask);
// 在图像上画十字标
void imlib_draw_cross(image_t *img, int x, int y, int c, int size, int thickness);






~~~