//
// Created by 姜姣姣 on 2022/9/13.
//

#ifndef WHITENOISE_CONSTANTS_H
#define WHITENOISE_CONSTANTS_H


constexpr int kMaxQueueItems = 4; // Must be power of 2
constexpr char kClapFilename2[] { "test.mp3" };
struct AudioProperties {
    int32_t channelCount;
    int32_t sampleRate;
};
#endif //WHITENOISE_CONSTANTS_H
