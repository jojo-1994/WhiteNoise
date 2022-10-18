//
// Created by 姜姣姣 on 2022/9/13.
//

#ifndef WHITENOISE_AUDIO_H
#define WHITENOISE_AUDIO_H

#include "audio/Mixer.h"
#include "audio/Player.h"
#include <future>
#include <android/asset_manager.h>
#include <oboe/Oboe.h>
#include <Constants.h>
#include <utils/LockFreeQueue.h>
#include<list>
#include<map>


enum class AudioState {
    Loading,
    Playing,
    FailedToLoad
};

class Audio : public oboe::AudioStreamDataCallback, oboe::AudioStreamErrorCallback {

public:
    explicit Audio(AAssetManager &);

    void play(const char *audioFile);

    void pause(const char *audioFile);

    bool setupAudioSources(const char *audioFile);

    bool openStream();

    void setVolume(const char *audioFile, float_t volume);

    bool scheduleAudioEvents();

    // Inherited from oboe::AudioStreamDataCallback.
    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;

    // Inherited from oboe::AudioStreamErrorCallback.
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) override;

private:
    AAssetManager &mAssetManager;
    std::future<void> mLoadingResult;
    std::atomic<AudioState> mAudioState{AudioState::Loading}; //todo 用原始指针初始化
    std::shared_ptr<oboe::AudioStream> mAudioStream;
    Mixer mMixer;
    std::atomic<int64_t> mCurrentFrame{0};
    std::atomic<int64_t> mSongPositionMs{0};
    std::atomic<int64_t> mLastUpdateTime{0};
    LockFreeQueue<int64_t, kMaxQueueItems> mClapEvents;
    std::map<const char *, Player *> mPlayMap;

    float mixingBuffer[kBufferSize];
    Player* getPlayer(const char *audioFile);
};


#endif //WHITENOISE_AUDIO_H
