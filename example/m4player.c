//------------------------------------------------------------------------------------------------
//  m4player.c
//  sokol_args + sokol_audio + mod4play
//-------------------------------------------------------------------------------------------------
//
// Copyright (c) 2025 dashodanger
// Copyright (c) 2017 Andre Weissflog (original player example code)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//------------------------------------------------------------------------------------------------

// Have to include this before Sokol headers to prevent the 'Status' define
// from goofing things
#define M4P_IMPLEMENTATION
#include "../m4p.h"
#define SOKOL_IMPL
#include "sokol/sokol_args.h"
#include "sokol/sokol_audio.h"
#include "sokol/sokol_log.h"
#include "data/mods.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

static bool     mod_playing = false;
static int      buffer_size = 0;

// stream callback, called by sokol_audio when new samples are needed,
// on most platforms, this runs on a separate thread
static void stream_cb(float* buffer, int num_frames, int num_channels) {
    if (mod_playing)
    {
        assert(num_frames <= buffer_size);
        m4p_GenerateFloatSamples(buffer, num_frames);
    }
    else
        memset(buffer, 0, num_frames * num_channels * sizeof(float));
}

static void print_help() {
	printf("\nUsage: m4player [song=file]\n"
		   "\n"
		   "Supported song formats:\n"
		   "- IT\n"
		   "- XM\n"
		   "- S3M\n"
		   "- MOD\n"
		   "- FT\n"
		   "\n");
}

int main(int argc, char *argv[]) {
	sargs_desc init_args;
	memset(&init_args, 0, sizeof(sargs_desc));
	init_args.argc = argc;
	init_args.argv = argv;
	sargs_setup(&init_args);

	if (sargs_exists("help")) {
		print_help();
		sargs_shutdown();
		exit(EXIT_SUCCESS);
	}

	// setup sokol_audio (default sample rate is 44100Hz)
	saudio_desc init_saudio;
	memset(&init_saudio, 0, sizeof(saudio_desc));
	init_saudio.num_channels = 2;
	init_saudio.stream_cb = stream_cb;
	init_saudio.logger.func = slog_func;
	saudio_setup(&init_saudio);
    if (!saudio_isvalid())
    {
        saudio_shutdown();
		printf("m4player: Error loading song\n");
		sargs_shutdown();
		exit(EXIT_FAILURE);
    }
    if (saudio_channels() != 2)
    {
        saudio_shutdown();
		printf("m4player: Audio backend does not support stereo output\n");
		sargs_shutdown();
		exit(EXIT_FAILURE);
    }
    buffer_size = saudio_buffer_frames();
   
	const char *song = sargs_value("song");
    bool mod_good;

	if (*song != '\0') 
        mod_good = m4p_LoadFromFile(song, saudio_sample_rate(), buffer_size);
    else
        mod_good = m4p_LoadFromData(embed_disco_feva_baby_s3m, sizeof(embed_disco_feva_baby_s3m), saudio_sample_rate(), buffer_size);
        
    if (mod_good)
    {
        m4p_PlaySong();
        mod_playing = true;
    }
    else
    {
        printf("m4player: Error starting playback\n");
        saudio_shutdown();
        sargs_shutdown();
        exit(EXIT_FAILURE);
    }

	printf("Playing %s....press enter to exit\n", song);
	fflush(stdout);
	getchar();

	mod_playing = false;

	saudio_shutdown();
    m4p_Stop();
    m4p_Close();
    m4p_FreeSong();
	sargs_shutdown();

	return EXIT_SUCCESS;
}