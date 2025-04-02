//------------------------------------------------------------------------------
//  m4player-sapp.c
//  sokol_app + sokol_audio + mod4play
//  This uses the user-data callback model both for sokol_app.h and
//  sokol_audio.h
//------------------------------------------------------------------------------

// Have to include this before Sokol headers to prevent the 'Status' define
// from goofing things
#define M4P_IMPLEMENTATION
#include "../m4p.h"
#define SOKOL_IMPL
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_audio.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"
#include "data/mods.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#define M4PLAYER_SRCBUF_SAMPLES (4096)

static int      user_mod_size = 0;
static uint8_t *user_mod_buffer = NULL;
static bool     mod_playing = false;

// common function to read sample stream from mod4play and convert to float
static void read_samples(float* buffer, int num_samples) {
    assert(num_samples <= M4PLAYER_SRCBUF_SAMPLES);
    // NOTE: for multi-channel playback, the samples are interleaved
    // (e.g. left/right/left/right/...)
    m4p_GenerateFloatSamples(buffer, num_samples * 2 /* num_channels */ / sizeof(float));
}

// stream callback, called by sokol_audio when new samples are needed,
// on most platforms, this runs on a separate thread
static void stream_cb(float* buffer, int num_frames, int num_channels, void* user_data) {
    if (mod_playing)
    {
        const int num_samples = num_frames * num_channels;
        read_samples(buffer, num_samples);
    }
}

static void init(void* user_data) {
    // setup sokol_gfx
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext(),
        .logger.func = slog_func,
    });

    // setup sokol_audio (default sample rate is 44100Hz)
    saudio_setup(&(saudio_desc){
        .num_channels = 2,
        .stream_userdata_cb = stream_cb,
        .logger.func = slog_func,
    });

    bool mod_good = false;

    if (user_mod_buffer != NULL)
        mod_good = m4p_LoadFromData(user_mod_buffer, user_mod_size, saudio_sample_rate(), M4PLAYER_SRCBUF_SAMPLES);
    else
        mod_good = m4p_LoadFromData(embed_disco_feva_baby_s3m, sizeof(embed_disco_feva_baby_s3m), saudio_sample_rate(), M4PLAYER_SRCBUF_SAMPLES);
        
    if (mod_good)
    {
        m4p_PlaySong();
        mod_playing = true;
    }
    else
    {
        printf("M4Player: Error starting playback\n");
        exit(EXIT_FAILURE);
    }
}

static void frame(void* user_data) {
    (void)user_data;
    sg_pass_action pass_action = {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.4f, 0.7f, 1.0f, 1.0f } }
    };
    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
    sg_end_pass();
    sg_commit();
}

static void cleanup(void* user_data) {
    (void)user_data;
    saudio_shutdown();
    m4p_Stop();
    m4p_Close();
    m4p_FreeSong();
    if (user_mod_buffer)
    {
        free(user_mod_buffer);
        user_mod_buffer = NULL;
        user_mod_size = 0;
    }
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    // Really simple, assumes the first argument passed is a filename to open;
    // make something more robust for the real world
    if (argc > 1)
    {
        FILE *user_mod_file = fopen(argv[1], "rb");
        if (user_mod_file != NULL)
        {
            long cur_pos = ftell(user_mod_file);
            fseek(user_mod_file, 0, SEEK_END);
            user_mod_size = (int)ftell(user_mod_file);
            fseek(user_mod_file, cur_pos, SEEK_SET);
            if (user_mod_size > 0)
            {
                user_mod_buffer = (uint8_t *)calloc(user_mod_size, 1);
                if (fread(user_mod_buffer, 1, user_mod_size, user_mod_file) != user_mod_size)
                {
                    free(user_mod_buffer);
                    user_mod_buffer = NULL;
                    user_mod_size = 0;
                }
            }
            fclose(user_mod_file);
            user_mod_file = NULL;
        }
        else
        {
            printf("Error opening %s (is this an IT/S3M/XM/FT/MOD file?)\n", argv[1]);
            printf("Will fallback to embedded song.\n");
        }
        
    }
    return (sapp_desc){
        .init_userdata_cb = init,
        .frame_userdata_cb = frame,
        .cleanup_userdata_cb = cleanup,
        .user_data = NULL,
        .width = 400,
        .height = 300,
        .window_title = "Sokol Audio + Mod4Play",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    };
}
