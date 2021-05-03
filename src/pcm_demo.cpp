#include "utils.h"
#include "pcm.h"

#define BUF_SIZE 247342
uint8_t buffer[BUF_SIZE];              /* some random data */

void pcm_demo()
{
    pcm_lib_t lib;
    int err;
    uint32_t i;
    snd_pcm_sframes_t frames;

    pcm_lib_init(&lib);

    err = lib.open(&lib.handle, "default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
    if (err < 0) printf("%s: open failed: %s\n", __func__, lib.strerror(err));

    int file_fd = open("/home/root/test1.raw", O_RDONLY);
    if (file_fd >= 0)
    {
        int r = read(file_fd, buffer, BUF_SIZE);
        printf("%s: file read: %d\n", __func__, r);
        for (i = 0; i < BUF_SIZE; i++)
            buffer[i] = buffer[i]/10;    
    } 
    err = lib.set_params(lib.handle,
                      SND_PCM_FORMAT_U8,
                      SND_PCM_ACCESS_RW_INTERLEAVED,
                      1,
                      4000,
                      1,
                      500000);
    if (err < 0) printf("%s: set_params failed: %s\n", __func__, lib.strerror(err));
    
    size_t f = 0;
    err = pcm_lib_writebuf(&lib, buffer, BUF_SIZE, &f);
    if (err < 0) printf("%s: writebuf failed: %s\n", __func__, lib.strerror(err));

    
    err = lib.drain(lib.handle);
    if (err < 0) printf("%s: drain failed: %s\n", __func__, lib.strerror(err));
    err = lib.close(lib.handle);
    if (err < 0) printf("%s: close failed: %s\n", __func__, lib.strerror(err));
}