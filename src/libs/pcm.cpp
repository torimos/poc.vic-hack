#include "pcm.h"
#include "utils.h"
#include <dlfcn.h>

void pcm_lib_init(pcm_lib_t* lib)
{
    memset(lib, 0, sizeof(pcm_lib_t));
    void* ptr = dlopen("libasound.so.2", RTLD_NOW);
    if (ptr == NULL)
    {   
        printf("%s: ERROR. failed to open lib\n",__func__);
    }
    *(void **)&(lib->open) = dlsym(ptr, "snd_pcm_open");
    *(void **)&(lib->set_params) = dlsym(ptr, "snd_pcm_set_params");
    *(void **)&(lib->recover) = dlsym(ptr, "snd_pcm_recover");
    *(void **)&(lib->writei) = dlsym(ptr, "snd_pcm_writei");
    *(void **)&(lib->drain) = dlsym(ptr, "snd_pcm_drain");
    *(void **)&(lib->close) = dlsym(ptr, "snd_pcm_close");
    *(void **)&(lib->strerror) = dlsym(ptr, "snd_strerror");
    *(void **)&(lib->format_width) = dlsym(ptr, "snd_pcm_format_width");
    *(void **)&(lib->avail_update) = dlsym(ptr, "snd_pcm_avail_update");
}

long pcm_lib_writebuf(pcm_lib_t* lib, uint8_t *buf, long len, size_t *frames)
{
    long r,a;
    int channels = 1;
    snd_pcm_format_t format = SND_PCM_FORMAT_U8;
    long frame_bytes = (lib->format_width(format) / 8) * channels;
    printf("%s: frame_bytes = %li\n", __func__, frame_bytes);
    while (len > 0) {
        r = lib->writei(lib->handle, (char*)buf, len);
        if (r == -EAGAIN)
            continue;
        printf("%s: write = %li\n", __func__, r);
        if (r < 0)
            return r;
        buf += r * frame_bytes;
        len -= r;
        *frames += r;
    }
    return 0;
}