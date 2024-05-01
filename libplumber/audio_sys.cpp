#include "audio_sys.h"

#include <filesystem>

#include <Audio/Sound.hpp>

static Audio::Sound *pipeRotate = nullptr;
static Audio::Sound *mBgm = nullptr;

namespace Plumber::AudioSystem
{
    void init()
    {
        if (!pipeRotate)
        {
            pipeRotate = new Audio::Sound(std::filesystem::path("assets/audio/trig.wav"));
            pipeRotate->setVolume(0.5);
        }
        if (!mBgm)
        {
            mBgm = new Audio::Sound(std::filesystem::path("assets/audio/loop.wav"), Audio::Sound::Type::Stream);
            mBgm->setLooping(true);
            mBgm->setVolume(0.2);
        }
    }

    void triggerPipeRotateSound()
    {
        if (pipeRotate)
            pipeRotate->replay();
    }

    void stopPipeRotateSound()
    {
        if (pipeRotate)
            pipeRotate->stop();
    }

    void startBGM()
    {
        if (mBgm)
            mBgm->play();
    }

    void stopBGM()
    {
        if (mBgm)
            mBgm->stop();
    }

    void dispose()
    {
        stopPipeRotateSound();
        stopBGM();
        delete pipeRotate;
        delete mBgm;
        pipeRotate = nullptr;
        mBgm = nullptr;
    }
} // namespace Plumber::AudioSystem