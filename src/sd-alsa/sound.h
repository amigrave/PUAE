 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for Linux/ALSA sound
  *
  * Copyright 1997 Bernd Schmidt
  * Copyright 2004 Heikki Orsila
  */

#include <alsa/asoundlib.h>

extern int sound_fd;
extern uae_u16 sndbuffer[];
extern uae_u16 *sndbufpt;
extern int sndbufsize;
extern snd_pcm_t *alsa_playback_handle;
extern int bytes_per_frame;

/* alsa_xrun_recovery() function is copied from ALSA manual. why the hell did
   they make ALSA this hard?! i bet 95% of ALSA programmers would like a
   simpler way to do error handling.. let the 5% use tricky APIs.
*/
static int alsa_xrun_recovery(snd_pcm_t *handle, int err)
{
  if (err == -EPIPE) {
    /* under-run */
    err = snd_pcm_prepare(handle);
    if (err < 0)
      fprintf(stderr, "uae: no recovery with alsa from underrun, prepare failed: %s\n", snd_strerror(err));
    return 0;
  } else if (err == -ESTRPIPE) {
    while ((err = snd_pcm_resume(handle)) == -EAGAIN) {
      /* wait until the suspend flag is released */
      fprintf(stderr, "uae: sleeping for alsa.\n");
      sleep(1);
    }
    if (err < 0) {
      err = snd_pcm_prepare(handle);
      if (err < 0)
	fprintf(stderr, "uae: no recovery with alsa from suspend, prepare failed: %s\n", snd_strerror(err));
    }
    return 0;
  }
  return err;
}

static void check_sound_buffers (void)
{
  if ((char *)sndbufpt - (char *)sndbuffer >= sndbufsize) {
    int frames = sndbufsize / bytes_per_frame;
    char *buf = (char *) sndbuffer;
    int ret;
    while (frames > 0) {
      ret = snd_pcm_writei(alsa_playback_handle, buf, frames);
      if (ret < 0) {
	if (ret == -EAGAIN || ret == -EINTR)
	  continue;
	if (alsa_xrun_recovery(alsa_playback_handle, ret) < 0) {
	  fprintf(stderr, "uae: write error with alsa: %s\n", snd_strerror(ret));
	  exit(-1);
	}
	continue;
      }
      frames -= ret;
      buf += ret * bytes_per_frame;
    }
    sndbufpt = sndbuffer;
  }
}

STATIC_INLINE void clear_sound_buffers (void)
{
    memset (sndbuffer, 0, sndbufsize);
    sndbufpt = sndbuffer;
}

#define AUDIO_NAME "alsa"

#define PUT_SOUND_WORD(b) do { *(uae_u16 *)sndbufpt = b; sndbufpt = (uae_u16 *)(((uae_u8 *)sndbufpt) + 2); } while (0)
#define PUT_SOUND_WORD_LEFT(b) do { if (currprefs.sound_filter) b = filter (b, &sound_filter_state[0]); PUT_SOUND_WORD(b); } while (0)
#define PUT_SOUND_WORD_RIGHT(b) do { if (currprefs.sound_filter) b = filter (b, &sound_filter_state[1]); PUT_SOUND_WORD(b); } while (0)
#define PUT_SOUND_WORD_LEFT2(b) do { if (currprefs.sound_filter) b = filter (b, &sound_filter_state[2]); PUT_SOUND_WORD(b); } while (0)
#define PUT_SOUND_WORD_RIGHT2(b) do { if (currprefs.sound_filter) b = filter (b, &sound_filter_state[3]); PUT_SOUND_WORD(b); } while (0)

#define PUT_SOUND_WORD_MONO(b) PUT_SOUND_WORD_LEFT(b)
#define SOUND16_BASE_VAL 0
#define SOUND8_BASE_VAL 128

#define DEFAULT_SOUND_MAXB 16384
#define DEFAULT_SOUND_MINB 16384
#define DEFAULT_SOUND_BITS 16
#define DEFAULT_SOUND_FREQ 44100
#define HAVE_STEREO_SUPPORT

#define FILTER_SOUND_OFF 0
#define FILTER_SOUND_EMUL 1
#define FILTER_SOUND_ON 2

#define FILTER_SOUND_TYPE_A500 0
#define FILTER_SOUND_TYPE_A1200 1
