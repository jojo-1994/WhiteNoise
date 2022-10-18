//
// Created by 姜姣姣 on 2022/9/13.
//

#include <utils/logging.h>
#include "Audio.h"
#include <thread>
#include <cinttypes>
#include <audio/AAssetDataSource.h>
#include <utils/UtilityFunctions.h>
#include <iostream>
#include <regex>


Audio::Audio(AAssetManager &assetManager) : mAssetManager(assetManager) {
//    int *p;
//    int i = 1;
//    p = &i;
//    LOGE("-----------1--%p", p);//输出地址
//    LOGE("-----------1--%p", &i);//输出地址
}

Player *Audio::getPlayer(const char *audioFile) {
    LOGE("%s", audioFile);
    std::map<const char *, Player *>::iterator iter;
    for (iter = mPlayMap.begin(); iter != mPlayMap.end(); iter++) {
        LOGE("----iter->first==%s", iter->first);
        LOGE("----audioFile==%s", audioFile);
        if (strcmp(audioFile, iter->first) == 0) {
            LOGE("相等");
            return iter->second;
        }
    }
    return nullptr;
}

void Audio::play(const char *audioFile) {
    Player *player = getPlayer(audioFile);

    if (player != nullptr) {
        player->setPlaying(true);
        return;
    }

    if (!setupAudioSources(audioFile)) {
        return;
    }

//    std::async(&Audio::setupAudioSources, this, audioFile);//std::async是异步接口, https://www.cnblogs.com/qicosmos/p/3534211.html
}

void Audio::pause(const char *audioFile) {
    LOGE("%s", audioFile);
    Player *player = getPlayer(audioFile);

    if (player != nullptr) {
        player->setPlaying(false);
    }
}

bool Audio::setupAudioSources(const char *audioFile) {
    AudioProperties targetProperties{
            .channelCount = mAudioStream->getChannelCount(),
            .sampleRate = mAudioStream->getSampleRate()
    };

    // Create a data source and player for the clap sound
    std::shared_ptr<AAssetDataSource> mClapSource{
            AAssetDataSource::newFromCompressedAsset(mAssetManager, audioFile, targetProperties)
    };
    if (mClapSource == nullptr) {
        LOGE("Could not load source data for clap sound");
        return false;
    }

    Player *play = new Player(
            mClapSource); //std::unique_ptr<Player> play = std::make_unique<Player>(mClapSource);
    play->setPlaying(true);
    play->setLooping(true);
    play->setFileName(audioFile);
    mPlayMap.insert(std::pair<const char *, Player *>(audioFile, play));
    mMixer.addTrack(play);
    return true;
}

void Audio::setVolume(const char *audioFile, float_t volume) {
    Player *player = getPlayer(audioFile);

    if (player != nullptr) {
        player->setVolumeValue(volume);
    }
//    std::future<void> *p=nullptr;
//    int pp = 1;
//    p=&pp;
//    LOGE("------------ee：%d", p==nullptr);//输出地址
//    std::future<void> *x = &mLoadingResult;
//    LOGE("-------------%d",  x!=0);//c++中只有判断变量为空的只有指针变量和static变量

//    mClaps->setVolumeValue(volume);
    //以下注释代码摘自：https://www.cnblogs.com/qicosmos/p/3534211.html
//    do {
//        status = mLoadingResult.wait_for(std::chrono::seconds(1));
//        if (status == std::future_status::deferred) {
//            LOGE("-------deferred") ;
//        } else if (status == std::future_status::timeout) {
//            LOGE("-------timeout") ;
//        } else if (status == std::future_status::ready) {
//            LOGE("-------ready") ;
//        }
//    } while (status != std::future_status::ready);
//    LOGE("-------finish") ;
//    mClaps->setVolumeValue(volume);
}


bool Audio::openStream() {
    // Create an audio stream
    oboe::AudioStreamBuilder builder;
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setFormatConversionAllowed(true);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setSampleRate(44100);
    builder.setSampleRateConversionQuality(
            oboe::SampleRateConversionQuality::Medium);
    builder.setChannelCount(2);
    builder.setDataCallback(this);
    builder.setErrorCallback(this);
    oboe::Result openStreamResult = builder.openStream(mAudioStream);
    if (openStreamResult != oboe::Result::OK) {
        LOGE("Failed to open stream. Error: %s", convertToText(openStreamResult));
        return false;
    }

    oboe::Result requestStartResult = mAudioStream->requestStart();
    if (requestStartResult != oboe::Result::OK) {
        LOGE("Failoboe::Resultstart stream. Error: %s", convertToText(requestStartResult));
        return false;
    }

    mMixer.setChannelCount(mAudioStream->getChannelCount());
    return true;
}

oboe::DataCallbackResult
Audio::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    auto *outputBuffer = static_cast<float *>(audioData);

    int64_t nextClapEventMs;

    for (int i = 0; i < numFrames; ++i) {

        mSongPositionMs = convertFramesToMillis(
                mCurrentFrame,
                mAudioStream->getSampleRate());

        if (mClapEvents.peek(nextClapEventMs) && mSongPositionMs >= nextClapEventMs) {
//            mClaps->setPlaying(true);
            mClapEvents.pop(nextClapEventMs);
        }

        mMixer.renderAudio(outputBuffer + (oboeStream->getChannelCount() * i), 1);


//        float *audioData2 = outputBuffer + (oboeStream->getChannelCount() * i);
//        mClaps->renderAudio(audioData2, 1);
//        for (int j = 0; j < numFrames * 2; ++j) {
//            audioData2[j] += mixingBuffer[j] * 1; // 振幅修改
//        }
//        mCurrentFrame++;
    }

    mLastUpdateTime = nowUptimeMillis();

    return oboe::DataCallbackResult::Continue;
}

void Audio::onErrorAfterClose(oboe::AudioStream *audioStream, oboe::Result error) {
    if (error == oboe::Result::ErrorDisconnected) {
        mAudioState = AudioState::Loading;
        mAudioStream.reset();
        mMixer.removeAllTracks();
        mCurrentFrame = 0;
        mSongPositionMs = 0;
        mLastUpdateTime = 0;
//        start(mAudioFile);
    } else {
        LOGE("Stream error: %s", convertToText(error));
    }
}


