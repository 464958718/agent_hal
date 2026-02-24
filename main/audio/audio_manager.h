// Copyright 2024 Xiaozhi
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Audio stream packet structure
typedef struct {
    uint8_t* data;
    size_t len;
    uint32_t timestamp;
    bool is_text;
} audio_stream_packet_t;

// Audio manager opaque type
typedef struct audio_manager audio_manager_t;

// Audio callback types
typedef void (*audio_data_callback_t)(const uint8_t* data, size_t len, void* user_data);
typedef void (*wake_word_callback_t)(const char* wake_word, void* user_data);
typedef void (*vad_callback_t)(bool speaking, void* user_data);

// Audio manager API
audio_manager_t* audio_manager_create(void);
void audio_manager_destroy(audio_manager_t* mgr);
int audio_manager_init(audio_manager_t* mgr);
void audio_manager_deinit(audio_manager_t* mgr);

// Recording control
int audio_manager_start_recording(audio_manager_t* mgr);
int audio_manager_stop_recording(audio_manager_t* mgr);
bool audio_manager_is_recording(const audio_manager_t* mgr);

// Playback control
int audio_manager_play_audio(audio_manager_t* mgr, const uint8_t* data, size_t len);
int audio_manager_stop_playback(audio_manager_t* mgr);
bool audio_manager_is_playing(const audio_manager_t* mgr);

// Volume control
int audio_manager_set_volume(audio_manager_t* mgr, int volume);
int audio_manager_get_volume(const audio_manager_t* mgr);

// Callback registration
void audio_manager_set_wake_word_callback(audio_manager_t* mgr,
    wake_word_callback_t cb, void* user_data);
void audio_manager_set_vad_callback(audio_manager_t* mgr,
    vad_callback_t cb, void* user_data);
void audio_manager_set_audio_data_callback(audio_manager_t* mgr,
    audio_data_callback_t cb, void* user_data);

// Status query
bool audio_manager_is_initialized(const audio_manager_t* mgr);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_MANAGER_H
