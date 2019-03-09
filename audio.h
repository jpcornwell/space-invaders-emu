
#ifndef AUDIO_H
#define AUDIO_H

typedef enum SoundType {
    SOUND_SHOOT,
    SOUND_INVADER_1,
    SOUND_INVADER_2,
    SOUND_INVADER_3,
    SOUND_INVADER_4,
    SOUND_INVADER_KILLED,
    SOUND_EXPLOSION,
    SOUND_UFO_HIGH,
    SOUND_UFO_LOW
} SoundType;

void init_audio(void);
void play_sound(SoundType);

#endif
