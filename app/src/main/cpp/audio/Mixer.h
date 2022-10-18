
/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SHARED_MIXER_H
#define SHARED_MIXER_H

#include <array>
#include "IRenderableAudio.h"
#include <iostream>

constexpr int32_t kBufferSize = 192*10;  // Temporary buffer is used for mixing
constexpr uint8_t kMaxTracks = 100;

/**
 * A Mixer object which sums the output from multiple tracks into a single output. The number of
 * input channels on each track must match the number of output channels (default 1=mono). This can
 * be changed by calling `setChannelCount`.
 * The inputs to the mixer are not owned by the mixer, they should not be deleted while rendering.
 */
class Mixer : public IRenderableAudio {

public:
    void renderAudio(float *audioData, int32_t numFrames) { //float *audioData：声明一个float类型的指针变量
        //使用*操作符来对audioData指针进行取值，输出存储在指针地址中的值， https://www.jianshu.com/p/215145b5c1ad
//        float a = *audioData;
//        LOGE("aaaa = %f",a);

        // Zero out the incoming container array
        // 初始化内存， 第一个参数audioData：内存地址， 第二个参数0：被设置的值， 第三个参数：被设置0的长度，
        // https://www.runoob.com/cprogramming/c-function-memset.html， http://c.biancheng.net/view/231.html
        memset(audioData, 0, sizeof(float) * numFrames * mChannelCount);
//        float a = *audioData;
//        LOGE("aaaa = %f",a);

        for (int i = 0; i < mNextFreeTrackIndex; ++i) {
            float a = 1.0;
//            if(i == 0) {
//                a = 0.9;
//            }else {
//                a = 0.4;
//            }

            mTracks[i]->renderAudio(mixingBuffer, numFrames);

            for (int j = 0; j < numFrames * mChannelCount; ++j) {
                audioData[j] += mixingBuffer[j]*a; // 振幅修改
            }
        }
    }

    void addTrack(IRenderableAudio *renderer){
        mTracks[mNextFreeTrackIndex++] = renderer;
    }

    void stopTrack(){
        stop=true;
    }

    void setChannelCount(int32_t channelCount){ mChannelCount = channelCount; }

    void removeAllTracks(){
        for (int i = 0; i < mNextFreeTrackIndex; i++){
            mTracks[i] = nullptr;
        }
        mNextFreeTrackIndex = 0;
    }

private:
    float mixingBuffer[kBufferSize];
    std::array<IRenderableAudio*, kMaxTracks> mTracks;
    uint8_t mNextFreeTrackIndex = 0;
    int32_t mChannelCount = 1; // Default to mono

    bool stop = false;
};


#endif //SHARED_MIXER_H
