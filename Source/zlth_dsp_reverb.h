#pragma once

#include <vector>
#include <cmath>

namespace zlth::dsp::reverb
{
    class DelayLine
    {
    public:
        void prepare(uint32_t exponent)
        {
            size_t size = 1ULL << exponent;
            buffer.assign(size, 0.0f);
            indexMask = size - 1;
            writeIndex = 0;
        }
        float read(float delaySamples) const
        {
            float readPos = static_cast<float>(writeIndex) - delaySamples;
            float fraction = readPos - std::floor(readPos);
            int i1 = static_cast<int>(std::floor(readPos));
            int idx1 = i1 & indexMask;
            int idx2 = (i1 + 1) & indexMask;
            return buffer[idx1] * (1.0f - fraction) + buffer[idx2] * fraction;
        }
        void write(float input)
        {
            buffer[writeIndex] = input;
            writeIndex = (writeIndex + 1) & indexMask;
        }
    private:
        std::vector<float> buffer;
        int writeIndex {0};
        int indexMask {0};
    };
    class ButterflyFDN
    {
    public:
        void prepare(double sampleRate)
        {
            currentSampleRate = sampleRate;
            for (int i = 0; i < numDelays; ++i)
            {
                delays[i].prepare(14);
                lastLowpassOut[i] = 0.0f;
            }
            updateReverbTimes(1.5f);
        }
        void updateReverbTimes(float rt60Seconds)
        {
            float rt60Samples = rt60Seconds * static_cast<float>(currentSampleRate);
            for (int i = 0; i < numDelays; ++i)
            {
                float actualDelay = delayTimes[i] * (static_cast<float>(currentSampleRate) / 44100.0f);
                feedbackGains[i] = std::pow(10.0f, (-3.0f * actualDelay) / rt60Samples);
            }
        }
        void setMix(float newMix)
        {
            mix = std::clamp(newMix, 0.0f, 1.0f);
        }
        void process(float& inL, float& inR)
        {
            float monoIn = (inL + inR) * 0.5f;
            float buffer[numDelays] {};
            float sampleRateScale = (float)currentSampleRate / 44100.0f;
            for (int i = 0; i < numDelays; ++i)
            {
                buffer[i] = delays[i].read(delayTimes[i] * sampleRateScale);
            }
            applyFWHT16(buffer);
            for (int i = 0; i < numDelays; ++i)
            {
                buffer[i] *= 0.25f;
            }
            float wetL = 0.0f;
            float wetR = 0.0f;
            const float lpCoeff = 0.4f;
            for (int i = 0; i < numDelays; ++i)
            {
                float filtered = buffer[i] * (1.0f - lpCoeff) + lastLowpassOut[i] * lpCoeff;
                lastLowpassOut[i] = filtered;
                delays[i].write(monoIn + filtered * feedbackGains[i]);
                if (i % 2 == 0) wetL += filtered;
                else            wetR += filtered;
            }
            wetL *= 0.25f;
            wetR *= 0.25f;
            inL = inL * (1.0f - mix) + wetL * mix;
            inR = inR * (1.0f - mix) + wetR * mix;
        }
    private:
        void applyFWHT16(float* data)
        {
            for (int step = 1; step < 16; step <<= 1)
            {
                for (int i = 0; i < 16; i += (step << 1))
                {
                    for (int j = 0; j < step; ++j)
                    {
                        float a = data[i + j];
                        float b = data[i + j + step];
                        data[i + j] = a + b;
                        data[i + j + step] = a - b;
                    }
                }
            }
        }
        static constexpr int numDelays = 16;
        const int delayTimes[numDelays] = {641, 823, 1019, 1231, 1453, 1721, 2011, 2339, 2713, 3019, 3413, 3821, 4153, 4513, 4817, 5101};
        float lastLowpassOut[numDelays] {};
        DelayLine delays[numDelays];
        double currentSampleRate = 44100.0;
        float feedbackGains[numDelays];
        float mix = 1.0f;
    };
}
