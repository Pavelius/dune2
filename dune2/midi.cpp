/*
	Copyright 2026 by Pavel Chistyakov

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	MIDI Music Player

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.77
*/

#include "midi.h"

#ifdef _MSC_VER

#include "win.h"

#pragma comment(lib, "winmm.lib")

#define HANDLE void*
#define HMIDISTRM HANDLE
#define HMIDIOUT HANDLE
#define WINMMAPI extern "C" __declspec(dllimport)
#define MMRESULT unsigned int
#define WINAPI __stdcall
#define CALLBACK __stdcall
#define DWORD_PTR DWORD*

const int MMSYSERR_NOERROR = 0; /* no error */

#define MHDR_DONE       0x00000001  /* done bit */
#define MHDR_PREPARED   0x00000002  /* set if header prepared */
#define MHDR_INQUEUE    0x00000004  /* reserved for driver */
#define MHDR_ISSTRM     0x00000008  /* buffer is stream buffer */

#define MM_MOM_OPEN     0x3C7 /* MIDI output */
#define MM_MOM_CLOSE    0x3C8
#define MM_MOM_DONE     0x3C9

#define MOM_OPEN        MM_MOM_OPEN
#define MOM_CLOSE       MM_MOM_CLOSE
#define MOM_DONE        MM_MOM_DONE

#define MEVT_SHORTMSG   ((unsigned char)0x00)    /* parm = shortmsg for midiOutShortMsg */
#define MEVT_TEMPO      ((unsigned char)0x01)    /* parm = new tempo in microsec/qn     */
#define MEVT_NOP        ((unsigned char)0x02)    /* parm = unused; does nothing         */

#define MIDIPROP_SET    0x80000000L
#define MIDIPROP_GET    0x40000000L

#define MIDIPROP_TIMEDIV    0x00000001L
#define MIDIPROP_TEMPO      0x00000002L

#define CALLBACK_TYPEMASK   0x00070000l    /* callback type mask */
#define CALLBACK_NULL       0x00000000l    /* no callback */
#define CALLBACK_WINDOW     0x00010000l    /* dwCallback is a HWND */
#define CALLBACK_TASK       0x00020000l    /* dwCallback is a HTASK */
#define CALLBACK_FUNCTION   0x00030000l    /* dwCallback is a FARPROC */

#define INFINITE            0xFFFFFFFF  // Infinite timeout
#define MIDI_MAPPER         0xFFFFFFFF

struct MIDIEVENT {
	DWORD       dwDeltaTime;          /* Ticks since last event */
	DWORD       dwStreamID;           /* Reserved; must be zero */
	DWORD       dwEvent;              /* Event type and parameters */
	DWORD       dwParms[1];           /* Parameters if this is a long event */
};
struct MIDIPROPTIMEDIV {
	DWORD       cbStruct;
	DWORD       dwTimeDiv;
};
struct MIDIHDR {
	char*       lpData;               /* pointer to locked data block */
	DWORD       dwBufferLength;       /* length of data in data block */
	DWORD       dwBytesRecorded;      /* used for input only */
	DWORD*		dwUser;               /* for client's use */
	DWORD       dwFlags;              /* assorted flags (see defines) */
	struct midihdr_tag *lpNext;		  /* reserved for driver */
	DWORD*		reserved;             /* reserved for driver */
	DWORD       dwOffset;             /* Callback offset into buffer */
	DWORD*		dwReserved[8];        /* Reserved for MMSYSTEM */
};

WINMMAPI MMRESULT WINAPI midiOutOpen(HMIDIOUT* phmo, unsigned int uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
WINMMAPI MMRESULT WINAPI midiOutClose(HMIDIOUT hmo);
WINMMAPI MMRESULT WINAPI midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg);

WINMMAPI MMRESULT WINAPI midiOutReset(HMIDIOUT hmo);
WINMMAPI MMRESULT WINAPI midiOutPrepareHeader(HMIDIOUT hmo, MIDIHDR* pmh, unsigned int cbmh);
WINMMAPI MMRESULT WINAPI midiOutUnprepareHeader(HMIDIOUT hmo, MIDIHDR* pmh, unsigned int cbmh);

WINMMAPI MMRESULT WINAPI midiStreamOpen(HMIDISTRM* phms, unsigned int* puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
WINMMAPI MMRESULT WINAPI midiStreamProperty(HMIDISTRM hms, unsigned char* lppropdata, DWORD dwProperty);
WINMMAPI MMRESULT WINAPI midiStreamPause(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiStreamRestart(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiStreamStop(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiStreamOut(HMIDISTRM hms, MIDIHDR* pmh, unsigned int cbmh);
WINMMAPI MMRESULT WINAPI midiStreamClose(HMIDISTRM hms);
WINMMAPI MMRESULT WINAPI midiOutSetVolume(HMIDIOUT hmo, DWORD dwVolume);

class midiplayer {

	static const unsigned BufferSize = 15000; // 60000 bytes, 5000 MIDI events
	MIDIHDR			header = {};
	const unsigned*	data = 0;
	unsigned		size = 0;
	unsigned		position = 0;
	void*			stream = 0;
	volatile bool	done = false;

	static void callback(HMIDIOUT, unsigned msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR) {
		if(msg != MOM_DONE)
			return;
		auto p = (midiplayer*)instance;
		if(p)
			p->done = true;
	}

	static bool unprepare(void* stream, MIDIHDR& header) {
		if((header.dwFlags & MHDR_PREPARED) != 0) {
			auto error = midiOutUnprepareHeader(stream, &header, sizeof(MIDIHDR));
			if(error != MMSYSERR_NOERROR)
				return false;
			header = {};
		}
		return true;
	}

	static bool post(void* stream, MIDIHDR& header, const unsigned* data, unsigned post_count) {
		if(!unprepare(stream, header))
			return false;
		header = {};
		header.lpData = (char*)data;
		header.dwBufferLength = post_count * sizeof(unsigned);
		header.dwBytesRecorded = header.dwBufferLength;
		auto error_code = midiOutPrepareHeader(stream, &header, sizeof(header));
		if(error_code != MMSYSERR_NOERROR)
			return false;
		error_code = midiStreamOut(stream, &header, sizeof(header));
		if(error_code != MMSYSERR_NOERROR) {
			midiOutUnprepareHeader(stream, &header, sizeof(header));
			return false;
		}
		return true;
	}

	void post(const unsigned* data, unsigned count) {
		// Prevent MIDI buffer overrun and error 11 from midiOutPrepareHeader.
		// Maximum buffer is 65k bytes.
		auto post_count = count;
		if(post_count > BufferSize)
			post_count = BufferSize;
		if(post(stream, header, data, post_count))
			position += post_count;
	}

	void update() {
		if(done) {
			done = false;
			unprepare(stream, header);
			if(position < size) {
				// Not played all song. Post next song portion.
				post(data + position, size - position);
			} else {
				data = 0;
				size = 0;
				position = 0;
			}
		}
	}

	void initialize() {
		auto device = MIDI_MAPPER;
		auto error = midiStreamOpen(&stream, &device, 1, (DWORD_PTR)&callback, (DWORD_PTR)this, CALLBACK_FUNCTION);
		if(error != MMSYSERR_NOERROR)
			stream = 0;
	}

	void close() {
		stop();
		if(stream) {
			midiStreamClose(stream);
			stream = 0;
		}
	}

	void reset() {
		for(auto channel = 0; channel < 16; ++channel) {
			midiOutShortMsg((HMIDIOUT)stream, 0xB0 | channel | (123 << 8)); // CC 123 - All Notes Off
			midiOutShortMsg((HMIDIOUT)stream, 0xB0 | channel | (121 << 8)); // CC 121 - Reset All Controllers
		}
	}

public:

	midiplayer() {
		initialize();
	}

	~midiplayer() {
		close();
	}

	void stop() {
		if(stream) {
			midiStreamStop(stream);
			midiOutReset(stream);
			unprepare(stream, header);
		}
		done = false;
		data = 0;
		size = 0;
		position = 0;
	}

	void play(const unsigned* data, unsigned size, unsigned short division) {

		if(this->data == data) {
			update();
			return; // Already play this
		}

		stop();

		if(!data || !size || !stream)
			return;

		reset();

		// MIDI time division.
		MIDIPROPTIMEDIV div{};
		div.cbStruct = sizeof(div);
		div.dwTimeDiv = division;
		if(midiStreamProperty(stream, (unsigned char*)&div, MIDIPROP_SET | MIDIPROP_TIMEDIV) != MMSYSERR_NOERROR)
			return;

		this->data = data;
		this->size = size;
		this->position = 0;

		post(data, size);

		if(midiStreamRestart(stream) != MMSYSERR_NOERROR) {
			stop();
			return;
		}

	}

	void setvolume(unsigned short value) {
		if(!stream)
			return;
		DWORD v = value | ((unsigned)value << 16);
		midiOutSetVolume(stream, v);
	}

};

#else

class midiplayer {

public:

	midiplayer() {
	}

	~midiplayer() {
	}

	void stop() {
	}

	void play(const unsigned* data, unsigned size, unsigned short division) {
	}

	void setvolume(unsigned short value) {
	}

};

#endif // _MSC_VER

static midiplayer music;

bool music_enable = true;
soundn current_music;

void music_update() {
	if(!music_enable)
		return;
	music.play(songs[current_music].data, songs[current_music].size, songs[current_music].division);
}

void music_stop() {
	music.stop();
}

void music_setvolume(short unsigned v) {
	music.setvolume(v);
}