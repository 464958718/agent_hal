/**
 * @file audio_codec.h
 * @brief Audio codec interface header file
 */
#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Codec type enumeration
 */
typedef enum {
    CODEC_TYPE_NONE,
    CODEC_TYPE_ES8311,
    CODEC_TYPE_ES8388,
    CODEC_TYPE_ES8374,
    CODEC_TYPE_ES8389,
    CODEC_TYPE_MAX
} codec_type_t;

/**
 * @brief Audio format structure
 */
typedef struct {
    int sample_rate;
    int bits_per_sample;
    int channels;
} audio_format_t;

/**
 * @brief Audio codec opaque structure
 */
typedef struct audio_codec audio_codec_t;

/**
 * @brief Codec operations structure
 */
typedef struct {
    int (*init)(audio_codec_t* codec, const audio_format_t* fmt);
    int (*deinit)(audio_codec_t* codec);
    int (*set_volume)(audio_codec_t* codec, int volume);
    int (*get_volume)(audio_codec_t* codec, int* volume);
    int (*start_record)(audio_codec_t* codec);
    int (*stop_record)(audio_codec_t* codec);
    int (*start_playback)(audio_codec_t* codec);
    int (*stop_playback)(audio_codec_t* codec);
} codec_ops_t;

/**
 * @brief Audio codec structure
 */
struct audio_codec {
    const codec_ops_t* ops;
    void* impl;
    codec_type_t type;
    audio_format_t format;
    int volume;
    bool initialized;
};

/**
 * @brief Create an audio codec instance
 * @param type Codec type
 * @return Pointer to audio codec instance, or NULL on failure
 */
audio_codec_t* audio_codec_create(codec_type_t type);

/**
 * @brief Destroy an audio codec instance
 * @param codec Pointer to audio codec instance
 */
void audio_codec_destroy(audio_codec_t* codec);

/**
 * @brief Initialize the audio codec
 * @param codec Pointer to audio codec instance
 * @param fmt Pointer to audio format
 * @return 0 on success, negative error code on failure
 */
int audio_codec_init(audio_codec_t* codec, const audio_format_t* fmt);

/**
 * @brief Deinitialize the audio codec
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_deinit(audio_codec_t* codec);

/**
 * @brief Set codec volume
 * @param codec Pointer to audio codec instance
 * @param volume Volume level (0-100)
 * @return 0 on success, negative error code on failure
 */
int audio_codec_set_volume(audio_codec_t* codec, int volume);

/**
 * @brief Get codec volume
 * @param codec Pointer to audio codec instance
 * @param volume Pointer to store volume level
 * @return 0 on success, negative error code on failure
 */
int audio_codec_get_volume(audio_codec_t* codec, int* volume);

/**
 * @brief Start audio recording
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_start_record(audio_codec_t* codec);

/**
 * @brief Stop audio recording
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_stop_record(audio_codec_t* codec);

/**
 * @brief Start audio playback
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_start_playback(audio_codec_t* codec);

/**
 * @brief Stop audio playback
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_stop_playback(audio_codec_t* codec);

#ifdef __cplusplus
}
#endif

#endif // AUDIO_CODEC_H
