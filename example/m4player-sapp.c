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
#define SOKOL_GLCORE33
#define SOKOL_GFX_IMPL
#define SOKOL_AUDIO_IMPL
#define SOKOL_LOG_IMPL
#define SOKOL_APP_IMPL
#define SOKOL_GLUE_IMPL
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
    const int num_samples = num_frames * num_channels;
    read_samples(buffer, num_samples);
}

void init(void* user_data) {
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

    m4p_LoadFromData(embed_disco_feva_baby_s3m, sizeof(embed_disco_feva_baby_s3m), saudio_sample_rate(), M4PLAYER_SRCBUF_SAMPLES);
    m4p_PlaySong();
}

void frame(void* user_data) {
    (void)user_data;
    sg_pass_action pass_action = {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.4f, 0.7f, 1.0f, 1.0f } }
    };
    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
    sg_end_pass();
    sg_commit();
}

void cleanup(void* user_data) {
    (void)user_data;
    saudio_shutdown();
    m4p_Stop();
    m4p_Close();
    m4p_FreeSong();
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
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
