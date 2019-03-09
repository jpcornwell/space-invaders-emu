
#include <SDL_mixer.h>

#include "audio.h"

Mix_Chunk *snd_shoot = NULL;
Mix_Chunk *snd_explosion = NULL;
Mix_Chunk *snd_invader_1 = NULL;
Mix_Chunk *snd_invader_2 = NULL;
Mix_Chunk *snd_invader_3 = NULL;
Mix_Chunk *snd_invader_4 = NULL;
Mix_Chunk *snd_invader_killed = NULL;
Mix_Chunk *snd_ufo_high = NULL;
Mix_Chunk *snd_ufo_low = NULL;

void init_audio() {
    snd_shoot = Mix_LoadWAV("sounds/shoot.wav");
    snd_explosion = Mix_LoadWAV("sounds/explosion.wav");
    snd_invader_1 = Mix_LoadWAV("sounds/fastinvader1.wav");
    snd_invader_2 = Mix_LoadWAV("sounds/fastinvader2.wav");
    snd_invader_3 = Mix_LoadWAV("sounds/fastinvader3.wav");
    snd_invader_4 = Mix_LoadWAV("sounds/fastinvader4.wav");
    snd_invader_killed = Mix_LoadWAV("sounds/invaderkilled.wav");
    snd_ufo_high = Mix_LoadWAV("sounds/ufo_highpitch.wav");
    snd_ufo_low = Mix_LoadWAV("sounds/ufo_lowpitch.wav");
}


void play_sound(SoundType sound) {
    switch (sound) {
        case SOUND_SHOOT:
            Mix_PlayChannel(-1, snd_shoot, 0);
            break;
        case SOUND_EXPLOSION:
            Mix_PlayChannel(-1, snd_explosion, 0);
            break;
        case SOUND_INVADER_1:
            Mix_PlayChannel(-1, snd_invader_1, 0);
            break;
        case SOUND_INVADER_2:
            Mix_PlayChannel(-1, snd_invader_2, 0);
            break;
        case SOUND_INVADER_3:
            Mix_PlayChannel(-1, snd_invader_3, 0);
            break;
        case SOUND_INVADER_4:
            Mix_PlayChannel(-1, snd_invader_4, 0);
            break;
        case SOUND_INVADER_KILLED:
            Mix_PlayChannel(-1, snd_invader_killed, 0);
            break;
        case SOUND_UFO_HIGH:
            Mix_PlayChannel(-1, snd_ufo_high, 0);
            break;
        case SOUND_UFO_LOW:
            Mix_PlayChannel(-1, snd_ufo_low, 0);
            break;
    }
}
