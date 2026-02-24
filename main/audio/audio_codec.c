/**
 * @file audio_codec.c
 * @brief Audio codec interface implementation
 */

#include "audio_codec.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Error codes for audio codec
 */
#define AUDIO_CODEC_OK          0
#define AUDIO_CODEC_ERR_INVALID -1
#define AUDIO_CODEC_ERR_NO_MEM  -2
#define AUDIO_CODEC_ERR_NOT_INIT -3

/**
 * @brief Default audio format
 */
static const audio_format_t default_format = {
    .sample_rate = 16000,
    .bits_per_sample = 16,
    .channels = 1
};

/**
 * @brief Create an audio codec instance
 * @param type Codec type
 * @return Pointer to audio codec instance, or NULL on failure
 */
audio_codec_t* audio_codec_create(codec_type_t type) {
    if (type <= CODEC_TYPE_NONE || type >= CODEC_TYPE_MAX) {
        return NULL;
    }

    audio_codec_t* codec = (audio_codec_t*)malloc(sizeof(audio_codec_t));
    if (codec == NULL) {
        return NULL;
    }

    memset(codec, 0, sizeof(audio_codec_t));
    codec->type = type;
    codec->volume = 70;  /* default volume */
    codec->initialized = false;
    memcpy(&codec->format, &default_format, sizeof(audio_format_t));

    return codec;
}

/**
 * @brief Destroy an audio codec instance
 * @param codec Pointer to audio codec instance
 */
void audio_codec_destroy(audio_codec_t* codec) {
    if (codec == NULL) {
        return;
    }

    if (codec->initialized) {
        audio_codec_deinit(codec);
    }

    free(codec);
}

/**
 * @brief Initialize the audio codec
 * @param codec Pointer to audio codec instance
 * @param fmt Pointer to audio format
 * @return 0 on success, negative error code on failure
 */
int audio_codec_init(audio_codec_t* codec, const audio_format_t* fmt) {
    if (codec == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    if (codec->ops == NULL || codec->ops->init == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    if (fmt != NULL) {
        memcpy(&codec->format, fmt, sizeof(audio_format_t));
    }

    int ret = codec->ops->init(codec, &codec->format);
    if (ret == AUDIO_CODEC_OK) {
        codec->initialized = true;
    }

    return ret;
}

/**
 * @brief Deinitialize the audio codec
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_deinit(audio_codec_t* codec) {
    if (codec == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    if (!codec->initialized) {
        return AUDIO_CODEC_ERR_NOT_INIT;
    }

    if (codec->ops == NULL || codec->ops->deinit == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    int ret = codec->ops->deinit(codec);
    if (ret == AUDIO_CODEC_OK) {
        codec->initialized = false;
    }

    return ret;
}

/**
 * @brief Set codec volume
 * @param codec Pointer to audio codec instance
 * @param volume Volume level (0-100)
 * @return 0 on success, negative error code on failure
 */
int audio_codec_set_volume(audio_codec_t* codec, int volume) {
    if (codec == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }

    if (codec->ops == NULL || codec->ops->set_volume == NULL) {
        /* No operation defined, just store the value */
        codec->volume = volume;
        return AUDIO_CODEC_OK;
    }

    return codec->ops->set_volume(codec, volume);
}

/**
 * @brief Get codec volume
 * @param codec Pointer to audio codec instance
 * @param volume Pointer to store volume level
 * @return 0 on success, negative error code on failure
 */
int audio_codec_get_volume(audio_codec_t* codec, int* volume) {
    if (codec == NULL || volume == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    if (codec->ops == NULL || codec->ops->get_volume == NULL) {
        /* No operation defined, return stored value */
        *volume = codec->volume;
        return AUDIO_CODEC_OK;
    }

    return codec->ops->get_volume(codec, volume);
}

/**
 * @brief Start audio recording
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_start_record(audio_codec_t* codec) {
    if (codec == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    if (!codec->initialized) {
        return AUDIO_CODEC_ERR_NOT_INIT;
    }

    if (codec->ops == NULL || codec->ops->start_record == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    return codec->ops->start_record(codec);
}

/**
 * @brief Stop audio recording
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_stop_record(audio_codec_t* codec) {
    if (codec == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    if (!codec->initialized) {
        return AUDIO_CODEC_ERR_NOT_INIT;
    }

    if (codec->ops == NULL || codec->ops->stop_record == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    return codec->ops->stop_record(codec);
}

/**
 * @brief Start audio playback
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_start_playback(audio_codec_t* codec) {
    if (codec == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    if (!codec->initialized) {
        return AUDIO_CODEC_ERR_NOT_INIT;
    }

    if (codec->ops == NULL || codec->ops->start_playback == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    return codec->ops->start_playback(codec);
}

/**
 * @brief Stop audio playback
 * @param codec Pointer to audio codec instance
 * @return 0 on success, negative error code on failure
 */
int audio_codec_stop_playback(audio_codec_t* codec) {
    if (codec == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    if (!codec->initialized) {
        return AUDIO_CODEC_ERR_NOT_INIT;
    }

    if (codec->ops == NULL || codec->ops->stop_playback == NULL) {
        return AUDIO_CODEC_ERR_INVALID;
    }

    return codec->ops->stop_playback(codec);
}
