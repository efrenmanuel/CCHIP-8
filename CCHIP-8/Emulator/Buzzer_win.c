#ifndef BUZZER_LOADED
	#define BUZZER_LOADED
	#include "Buzzer_win.h"
	#include "Global.h"
	#include "CPU.h"
	#include <windows.h>
	#include <mmsystem.h>
	#include <stdio.h>
	#include <math.h>


	#define BUFFER_COUNT 4
	#define BUFFER_SAMPLE_COUNT 1000
	#define SAMPLES_PER_SECOND 48000

	Byte cannot_start_output;
	Byte c8_buz_started;


	HWAVEOUT waveOut;
	HANDLE waveEvent;
	WAVEHDR headers[BUFFER_COUNT];
	unsigned short buffers[BUFFER_COUNT][BUFFER_SAMPLE_COUNT * 2];
	WAVEHDR* currentHeader;
	double volume = 6000;
	double phase;
	double phase_increment;
	double audio_value;

	void c8_buz_sound(double frequency) {
		if (frequency == 0) {
			phase_increment = 0;
			return;
		}
		phase_increment = 2 * M_PI / SAMPLES_PER_SECOND * frequency;
	}

	void fill_buffer(unsigned short* buffer) {
		for (size_t i = 0; i < BUFFER_SAMPLE_COUNT * 2; i += 2) {
			if (phase_increment == 0) {
				phase = 0;
				audio_value *= 0.9;
			}
			else {
				phase += phase_increment;
				if (phase > 0) { phase -= 2 * M_PI; }
				audio_value = sin(phase) * volume;
			}
			buffer[i + 0] = audio_value;  // Left channel
			buffer[i + 1] = audio_value;  // Right channel
		}
	}

	DWORD __stdcall audio_thread(LPVOID param) {
		while (1) {
			DWORD waitResult = WaitForSingleObject(waveEvent, INFINITE);
			if (waitResult) {
				fprintf(stderr, "Failed to wait for event.\n");
				return 1;
			}

			BOOL success = ResetEvent(waveEvent);
			if (!success) {
				fprintf(stderr, "Failed to reset event.\n");
				return 1;
			}

			while (currentHeader->dwFlags & WHDR_DONE) {
				fill_buffer((unsigned short*)currentHeader->lpData);
				MMRESULT result = waveOutWrite(waveOut, currentHeader, sizeof(WAVEHDR));
				if (result) {
					fprintf(stderr, "Failed to write wave data.  Error code %u.\n", result);
					return 1;
				}

				currentHeader++;
				if (currentHeader == headers + BUFFER_COUNT) { currentHeader = headers; }
			}
		}
	}

	void c8_buz_initialize() {
		WAVEFORMATEX format = { 0 };
		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = 2;
		format.nSamplesPerSec = SAMPLES_PER_SECOND;
		format.wBitsPerSample = 16;
		format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
		format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;

		waveEvent = CreateEvent(NULL, 1, 0, NULL);
		if (waveEvent == NULL) {
		fprintf(stderr, "Failed to create event.");
		return;
		}

		MMRESULT result = waveOutOpen(&waveOut, WAVE_MAPPER, &format,
		(DWORD_PTR)waveEvent, 0, CALLBACK_EVENT);
		if (result) {
		fprintf(stderr, "Failed to start audio output.  Error code %u.\n", result);
		return;
		}

		for (size_t i = 0; i < BUFFER_COUNT; i++) {
		headers[i] = (WAVEHDR) {
			.lpData = (char*)buffers[i],
			.dwBufferLength = BUFFER_SAMPLE_COUNT * 4,
		};
		result = waveOutPrepareHeader(waveOut, &headers[i], sizeof(WAVEHDR));
		if (result) {
			fprintf(stderr, "Failed to prepare header.  Error code %u.\n", result);
			return;
		}
		headers[i].dwFlags |= WHDR_DONE;
		}
		currentHeader = headers;

		HANDLE thread = CreateThread(NULL, 0, audio_thread, NULL, 0, NULL);
		if (thread == NULL) {
		fprintf(stderr, "Failed to start thread");
		}
	}

	void c8_buz_tick() {
		if (r_ST > 0) {
			c8_buz_start_beep();
		}
		else {
			c8_buz_stop_beep();
		}
	}

	void c8_buz_start_beep() {
		if (!c8_buz_started) {
			c8_buz_sound(880);
			c8_buz_started = 1;
		}
	}
	void c8_buz_stop_beep() {
		if (c8_buz_started) {
			c8_buz_sound(0);
			c8_buz_started = 0;
		}
	}
	
#endif