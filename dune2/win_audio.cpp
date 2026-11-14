// #include "audio.h"

namespace {
// WAV file header structure
struct wav {
	// RIFF chunk descriptor
	char            chunkID[4]; // "RIFF"
	unsigned        chunkSize;  // Size of the entire file in bytes minus 8 bytes
	char            format[4];  // "WAVE"
	// format sub-chunk
	char            subchunk1ID[4]; // "fmt "
	unsigned        subchunk1Size; // Size of the fmt chunk (16 for PCM)
	unsigned short  audioFormat; // Audio format (1 for PCM)
	unsigned short  numChannels; // Number of channels (1 = mono, 2 = stereo)
	unsigned        sampleRate; // Sampling rate (e.g., 44100 Hz)
	unsigned        byteRate; // Byte rate = SampleRate * NumChannels * BitsPerSample / 8
	unsigned short  blockAlign; // Block align = NumChannels * BitsPerSample / 8
	unsigned short  bitsPerSample; // Bits per sample (e.g., 8, 16, 24, 32)
	// data sub-chunk
	char            subchunk2ID[4]; // "data"
	unsigned        subchunk2Size; // Size of the data chunk (number of bytes of audio data)
};
}

#pragma comment(lib, "winmm.lib")

#define WINBASEAPI extern "C"
#define WINAPI __stdcall
#define CALLBACK __stdcall
#define WORD short unsigned
#define DWORD unsigned

/* wave callback messages */
#define MM_WOM_OPEN         0x3BB           /* waveform output */
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD
#define WOM_OPEN    MM_WOM_OPEN
#define WOM_CLOSE   MM_WOM_CLOSE
#define WOM_DONE    MM_WOM_DONE

#define CALLBACK_TYPEMASK   0x00070000l    /* callback type mask */
#define CALLBACK_NULL       0x00000000l    /* no callback */
#define CALLBACK_WINDOW     0x00010000l    /* dwCallback is a HWND */
#define CALLBACK_TASK       0x00020000l    /* dwCallback is a HTASK */
#define CALLBACK_FUNCTION   0x00030000l    /* dwCallback is a FARPROC */

#define MMSYSERR_NOERROR 0

struct WAVEFORMATEX {
	WORD		wFormatTag;
	WORD		nChannels;
	DWORD		nSamplesPerSec;
	DWORD		nAvgBytesPerSec;
	WORD		nBlockAlign;
	WORD		wBitsPerSample;
	WORD		cbSize;
};
struct WAVEHDR {
	char*		lpData;
	unsigned	dwBufferLength;
	unsigned	dwBytesRecorded;
	unsigned*	dwUser;
	unsigned	dwFlags;
	unsigned	dwLoops;
	WAVEHDR*	lpNext;
	unsigned*	reserved;
};

WINBASEAPI int WINAPI waveOutReset(void* hwo);
WINBASEAPI int WINAPI waveOutClose(void* hwo);
WINBASEAPI int WINAPI waveOutGetVolume(void* hwo, unsigned* pdwVolume);
WINBASEAPI int WINAPI waveOutOpen(void** phwo, unsigned uDeviceID, const struct WAVEFORMATEX* pwfx, void* dwCallback, void* dwInstance, unsigned fdwOpen);
WINBASEAPI int WINAPI waveOutPrepareHeader(void* hwo, WAVEHDR* pwh, unsigned int cbwh);
WINBASEAPI int WINAPI waveOutSetVolume(void* hwo, unsigned dwVolume);
WINBASEAPI int WINAPI waveOutUnprepareHeader(void* hwo, WAVEHDR* pwh, unsigned int cbwh);
WINBASEAPI int WINAPI waveOutWrite(void* hwo, WAVEHDR* pwh, unsigned int cbwh);

enum channelplayn : unsigned char {
	ChannelReady, ChannelPlayed, ChannelDone,
};

struct channelinfo {
	volatile channelplayn mode;
	WAVEHDR     header;
	void*		object; // wav data playing
	void*       handle;
	explicit operator bool() const { return handle != 0; }
};
static channelinfo music_channel;
static channelinfo channels[16];

static void channel_check_done(channelinfo* p) {
	if(!p->handle)
		return;
	if(p->mode == ChannelDone) {
		waveOutUnprepareHeader(p->handle, &p->header, sizeof(p->header));
		p->mode = ChannelReady;
	}
}

static void CALLBACK audio_callback(void* hWaveOut, unsigned int uMsg, unsigned int dwInstance, unsigned int dwParam1, unsigned int dwParam2) {
	if(uMsg == WOM_DONE) {
		auto p = (channelinfo*)dwInstance;
		p->mode = ChannelDone;
	}
}

static void channel_create(channelinfo* p, int number_channels, int sample_rate, int bits_per_sample) {
	if(p->handle) {
		auto pw = (wav*)p->object;
		if(!pw)
			return;
		if(pw->numChannels != number_channels || pw->sampleRate != sample_rate || pw->bitsPerSample != bits_per_sample) {
			waveOutClose(p->handle);
			p->handle = 0;
		}
	}
	WAVEFORMATEX wfx = {0};
	wfx.cbSize = 0;
	wfx.wFormatTag = 1;
	wfx.nChannels = number_channels;
	wfx.nSamplesPerSec = sample_rate;
	wfx.wBitsPerSample = bits_per_sample;
	wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	waveOutOpen(&p->handle, (DWORD)-1, &wfx, (void*)audio_callback, p, CALLBACK_FUNCTION);
}

static void channel_write(channelinfo* p, void* object) {
	auto ph = (wav*)object;
	if(!ph)
		return;
	channel_create(p, ph->numChannels, ph->sampleRate, ph->bitsPerSample);
	p->object = object;
	p->header = {};
	p->header.dwBufferLength = ph->subchunk2Size;
	p->header.lpData = (char*)ph + sizeof(*ph);
	p->mode = ChannelPlayed;
	waveOutPrepareHeader(p->handle, &p->header, sizeof(WAVEHDR));
	waveOutWrite(p->handle, &p->header, sizeof(WAVEHDR));
}

static void channel_reset(channelinfo* p) {
	if(p->handle && p->mode == ChannelPlayed)
		waveOutReset(p->handle);
}

void audio_update_channels() {
	channel_check_done(&music_channel);
	for(auto& e : channels)
		channel_check_done(&e);
}

static channelinfo* free_channel() {
	audio_update_channels();
	for(auto& e : channels) {
		if(e.mode == ChannelReady)
			return &e;
	}
	return 0;
}

void audio_reset() {
	audio_update_channels();
	for(auto& e : channels)
		channel_reset(&e);
	audio_update_channels();
}

void play_music_raw(void* object) {
	if(music_channel.mode != ChannelReady && music_channel.object == object)
		return;
	channel_reset(&music_channel);
	if(!object)
		music_channel.object = object;
	else
		channel_write(&music_channel, object);
}

void audio_play(void* object) {
	auto p = free_channel();
	if(!p)
		return;
	channel_write(p, object);
}

bool audio_played(const void* object) {
	if(!object)
		return false;
	for(auto& e : channels) {
		if(e.object == object)
			return true;
	}
	return false;
}

int audio_lenght(const void* object) {
	auto ph = (wav*)object;
	if(!ph || !ph->byteRate)
		return 0;
	auto r = (unsigned long long)ph->subchunk2Size * 1000;
	return (int)(r / ph->byteRate);
}