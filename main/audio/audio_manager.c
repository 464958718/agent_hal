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

#include "audio_manager.h"
#include <stdlib.h>
#include <string.h>

// TODO: Include ESP-IDF audio headers when integrating
// #include "esp_err.h"
// #include "esp_audio.h"
// #include "i2s_stream.h"
// #include "audio_pipeline.h"
// #include "audio_element.h"
// #include "esp_afe_vad.h"
// #include "esp_spiffs.h"
// #include "nvs_flash.h"

// Audio manager internal structure
struct audio_manager {
    // Initialization state
    bool initialized;

    // Recording state
    bool is_recording;

    // Playback state
    bool is_playing;

    // Volume (0-100)
    int volume;

    // Callbacks and user data
    audio_data_callback_t audio_data_callback;
    void* audio_data_user_data;

    wake_word_callback_t wake_word_callback;
    void* wake_word_user_data;

    vad_callback_t vad_callback;
    void* vad_user_data;

    // TODO: Add ESP-IDF audio components
    // esp_audio_handle_t audio_handle;
    // audio_pipeline_handle_t pipeline;
    // i2s_stream_handle_t i2s_stream_reader;
    // i2s_stream_handle_t i2s_stream_writer;
    // vad_handle_t vad_handle;
};

// Create audio manager instance
audio_manager_t* audio_manager_create(void) {
    audio_manager_t* mgr = (audio_manager_t*)malloc(sizeof(audio_manager_t));
    if (mgr == NULL) {
        return NULL;
    }

    // Initialize to default values
    memset(mgr, 0, sizeof(audio_manager_t));
    mgr->volume = 80;  // Default volume 80%
    mgr->is_recording = false;
    mgr->is_playing = false;
    mgr->initialized = false;

    return mgr;
}

// Destroy audio manager instance
void audio_manager_destroy(audio_manager_t* mgr) {
    if (mgr == NULL) {
        return;
    }

    // Ensure resources are released
    if (mgr->initialized) {
        audio_manager_deinit(mgr);
    }

    free(mgr);
}

// Initialize audio manager
int audio_manager_init(audio_manager_t* mgr) {
    if (mgr == NULL) {
        return -1;
    }

    if (mgr->initialized) {
        return 0;  // Already initialized
    }

    // TODO: Initialize ESP-IDF audio components
    // 1. Initialize NVS
    // esp_err_t ret = nvs_flash_init();
    // if (ret != ESP_OK) {
    //     return -1;
    // }

    // 2. Initialize I2S for microphone input
    // i2s_stream_config_t i2s_config = {
    //     .i2s_port = I2S_NUM_0,
    //     .i2s_config = {
    //         .mode = I2S_MODE_MASTER | I2S_MODE_RX,
    //         .sample_rate = 16000,
    //         .bits_per_sample = 16,
    //         .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    //     },
    //     .i2s_pin_config = {
    //         .bck_io_num = GPIO_NUM_4,
    //         .ws_io_num = GPIO_NUM_5,
    //         .data_out_num = I2S_PIN_NO_CHANGE,
    //         .data_in_num = GPIO_NUM_6,
    //     },
    // };
    // i2s_stream_new(&i2s_config, &mgr->i2s_stream_reader);

    // 3. Initialize I2S for speaker output
    // i2s_stream_config_t i2s_out_config = {
    //     .i2s_port = I2S_NUM_1,
    //     .i2s_config = {
    //         .mode = I2S_MODE_MASTER | I2S_MODE_TX,
    //         .sample_rate = 16000,
    //         .bits_per_sample = 16,
    //         .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    //     },
    //     .i2s_pin_config = {
    //         .bck_io_num = GPIO_NUM_8,
    //         .ws_io_num = GPIO_NUM_9,
    //         .data_out_num = GPIO_NUM_10,
    //         .data_in_num = I2S_PIN_NO_CHANGE,
    //     },
    // };
    // i2s_stream_new(&i2s_out_config, &mgr->i2s_stream_writer);

    // 4. Initialize VAD (Voice Activity Detection)
    // esp_afe_vad_config_t vad_config = {
    //     .sample_rate = 16000,
    //     .frame_size = 1024,
    //     .init_flag = 1,
    // };
    // esp_afe_vad_create(&vad_config, &mgr->vad_handle);

    // 5. Create audio pipeline
    // audio_pipeline_cfg_t pipeline_cfg = {
    //     .rb_size = 2048,
    // };
    // audio_pipeline_new(&pipeline_cfg, &mgr->pipeline);

    // Mark as initialized
    mgr->initialized = true;

    return 0;
}

// Deinitialize audio manager
void audio_manager_deinit(audio_manager_t* mgr) {
    if (mgr == NULL || !mgr->initialized) {
        return;
    }

    // Stop any ongoing operations
    if (mgr->is_recording) {
        audio_manager_stop_recording(mgr);
    }

    if (mgr->is_playing) {
        audio_manager_stop_playback(mgr);
    }

    // TODO: Release ESP-IDF audio resources
    // if (mgr->pipeline) {
    //     audio_pipeline_stop(mgr->pipeline);
    //     audio_pipeline_deinit(mgr->pipeline);
    // }
    // if (mgr->i2s_stream_reader) {
    //     i2s_stream_destroy(mgr->i2s_stream_reader);
    // }
    // if (mgr->i2s_stream_writer) {
    //     i2s_stream_destroy(mgr->i2s_stream_writer);
    // }
    // if (mgr->vad_handle) {
    //     esp_afe_vad_destroy(mgr->vad_handle);
    // }

    mgr->initialized = false;
}

// Start recording audio
int audio_manager_start_recording(audio_manager_t* mgr) {
    if (mgr == NULL) {
        return -1;
    }

    if (!mgr->initialized) {
        return -2;
    }

    if (mgr->is_recording) {
        return 0;  // Already recording
    }

    // TODO: Start audio capture via I2S
    // i2s_stream_start(mgr->i2s_stream_reader);

    // TODO: Register callback to receive audio data
    // i2s_stream_set_read_cb(mgr->i2s_stream_reader, audio_data_callback_wrapper, mgr);

    mgr->is_recording = true;

    return 0;
}

// Stop recording audio
int audio_manager_stop_recording(audio_manager_t* mgr) {
    if (mgr == NULL) {
        return -1;
    }

    if (!mgr->is_recording) {
        return 0;  // Not recording
    }

    // TODO: Stop audio capture
    // i2s_stream_stop(mgr->i2s_stream_reader);

    mgr->is_recording = false;

    return 0;
}

// Check if currently recording
bool audio_manager_is_recording(const audio_manager_t* mgr) {
    if (mgr == NULL) {
        return false;
    }
    return mgr->is_recording;
}

// Play audio data
int audio_manager_play_audio(audio_manager_t* mgr, const uint8_t* data, size_t len) {
    if (mgr == NULL || data == NULL || len == 0) {
        return -1;
    }

    if (!mgr->initialized) {
        return -2;
    }

    // TODO: Play audio via I2S
    // i2s_stream_write(mgr->i2s_stream_writer, data, len, portMAX_DELAY);

    mgr->is_playing = true;

    // TODO: Set playback state to false when complete
    // This should be done in a callback or task

    return 0;
}

// Stop audio playback
int audio_manager_stop_playback(audio_manager_t* mgr) {
    if (mgr == NULL) {
        return -1;
    }

    if (!mgr->is_playing) {
        return 0;  // Not playing
    }

    // TODO: Stop audio playback
    // i2s_stream_stop(mgr->i2s_stream_writer);

    mgr->is_playing = false;

    return 0;
}

// Check if currently playing
bool audio_manager_is_playing(const audio_manager_t* mgr) {
    if (mgr == NULL) {
        return false;
    }
    return mgr->is_playing;
}

// Set volume (0-100)
int audio_manager_set_volume(audio_manager_t* mgr, int volume) {
    if (mgr == NULL) {
        return -1;
    }

    // Clamp volume to valid range
    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }

    mgr->volume = volume;

    // TODO: Apply volume to audio output
    // esp_audio_volume_set(mgr->audio_handle, volume);

    return 0;
}

// Get current volume
int audio_manager_get_volume(const audio_manager_t* mgr) {
    if (mgr == NULL) {
        return -1;
    }
    return mgr->volume;
}

// Set wake word callback
void audio_manager_set_wake_word_callback(audio_manager_t* mgr,
    wake_word_callback_t cb, void* user_data) {
    if (mgr == NULL) {
        return;
    }

    mgr->wake_word_callback = cb;
    mgr->wake_word_user_data = user_data;

    // TODO: Initialize wake word detection (e.g., "Hey XiaoZhi")
    // This would typically use ESP-IDF's speech recognition or a custom model
}

// Set VAD (Voice Activity Detection) callback
void audio_manager_set_vad_callback(audio_manager_t* mgr,
    vad_callback_t cb, void* user_data) {
    if (mgr == NULL) {
        return;
    }

    mgr->vad_callback = cb;
    mgr->vad_user_data = user_data;

    // TODO: VAD callback is typically triggered by the VAD component
    // when speech is detected or stops
}

// Set audio data callback
void audio_manager_set_audio_data_callback(audio_manager_t* mgr,
    audio_data_callback_t cb, void* user_data) {
    if (mgr == NULL) {
        return;
    }

    mgr->audio_data_callback = cb;
    mgr->audio_data_user_data = user_data;
}

// Check if audio manager is initialized
bool audio_manager_is_initialized(const audio_manager_t* mgr) {
    if (mgr == NULL) {
        return false;
    }
    return mgr->initialized;
}

// TODO: Add internal helper functions for ESP-IDF integration
// - audio_pipeline_setup_recording()
// - audio_pipeline_setup_playback()
// - vad_process_audio_frame()
// - wake_word_detected_handler()
