#ifndef PLUMBER_AUDIO_SYS_H
#define PLUMBER_AUDIO_SYS_H

namespace Plumber::AudioSystem
{
    void init();
    void triggerPipeRotateSound();
    void stopPipeRotateSound();
    void startBGM();
    void stopBGM();
    void dispose();
} // namespace Plumber::AudioSystem

#endif // PLUMBER_AUDIO_SYS_H
