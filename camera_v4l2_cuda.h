/*
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <queue>
#include "opencv_consumer_interface.h"

#define V4L2_BUFFERS_NUM    4

#define INFO(fmt, ...) \
    if (ctx->enable_verbose) \
        printf("INFO: %s(): (line:%d) " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define WARN(fmt, ...) \
        printf("WARN: %s(): (line:%d) " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);

#define CHECK_ERROR(cond, label, fmt, ...) \
    if (!cond) { \
        error = 1; \
        printf("ERROR: %s(): (line:%d) " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        goto label; \
    }

#define ERROR_RETURN(fmt, ...) \
    do { \
        printf("ERROR: %s(): (line:%d) " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return false; \
    } while(0)

typedef struct
{
    // Hold the user accessible pointer
    unsigned char * start;
    // Hold the memory length
    unsigned int size;
    // Hold the file descriptor of NvBuffer
    int dmabuff_fd;
} nv_buffer;

typedef struct
{
    // camera v4l2 context
    const char * cam_devname;
    char cam_file[16];
    int cam_fd;
    unsigned int cam_pixfmt;
    unsigned int cam_w;
    unsigned int cam_h;
    unsigned int frame;
    unsigned int save_n_frame;

    // VIC v4l2 context
    NvVideoConverter * conv;
    unsigned int vic_pixfmt;
    enum v4l2_flip_method vic_flip;
    enum v4l2_interpolation_method vic_interpolation;
    enum v4l2_tnr_algorithm vic_tnr;
    bool got_error;

    // Global buffer ptr
    nv_buffer * g_buff;

    // Used to do synchonization between camera and VIC output plane
    std::queue < nv_buffer * > *conv_output_plane_buf_queue;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_cond;

    // EGL renderer
    NvEglRenderer *renderer;
    int fps;

    // CUDA processing
    bool enable_cuda;
    EGLDisplay egl_display;
    EGLImageKHR egl_image;

	// caffe 
    char *lib_file;
    void *lib_handler;
    //opencv_handler_open_t opencv_handler_open;
    //opencv_handler_close_t opencv_handler_close;
    //opencv_img_processing_t opencv_img_processing;
    //opencv_set_config_t opencv_set_config;
    void *opencv_handler;

    char *model_file;
    char *trained_file;
    char *mean_file;
    char *label_file;


    // Verbose option
    bool enable_verbose;

} context_t;

// Correlate v4l2 pixel format and NvBuffer color format
typedef struct
{
    unsigned int v4l2_pixfmt;
    NvBufferColorFormat nvbuff_color;
} nv_color_fmt;
