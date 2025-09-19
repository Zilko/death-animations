#include "BaseAnimation.hpp"

#include <opusfile.h>

class MrHippo : public BaseAnimation {

private:

    struct OpusData {
        OggOpusFile* opusFile;
        int channels;

        OpusData(OggOpusFile* opusFile, int channels)
            : opusFile(opusFile), channels(channels) {}
    };

    OpusData* m_data = nullptr;

    ~MrHippo() {
        SoundManager::stop();

        if (m_data) {
            op_free(m_data->opusFile);
            delete m_data;
        }
    }

    ANIMATION_CTOR_CREATE(MrHippo)
    
public:

    void start() override {
        int err = 0;

        OggOpusFile* opusFile = op_open_file(utils::string::pathToString(Mod::get()->getResourcesDir() / "hippo.opus").c_str(), &err);

        if (err != 0) return log::error("Opus has failed you, boy. {}", err);

        int channels = op_head(opusFile, 0)->channel_count;

        m_data = new OpusData(opusFile, channels);

        FMOD_CREATESOUNDEXINFO exinfo = {
            .cbsize = sizeof(FMOD_CREATESOUNDEXINFO),
            .length = static_cast<unsigned int>(op_pcm_total(opusFile, -1) * channels * sizeof(float)),
            .numchannels = channels,
            .defaultfrequency = 48000,
            .format = FMOD_SOUND_FORMAT_PCMFLOAT,
            .decodebuffersize = 48000,
            .userdata = m_data
        };

        exinfo.pcmreadcallback = [](FMOD_SOUND* sound, void* data, unsigned int datalen) -> FMOD_RESULT {
            void* userdata = nullptr;
            reinterpret_cast<FMOD::Sound*>(sound)->getUserData(&userdata);

            OpusData* opusData = static_cast<OpusData*>(userdata);
            OggOpusFile* opusFile = opusData->opusFile;
            float* floatData = static_cast<float*>(data);
            int channels = opusData->channels;
            unsigned int samplesRequested = datalen / (sizeof(float) * channels);
            unsigned int samplesDecodedTotal = 0;

            while (samplesDecodedTotal < samplesRequested) {
                int samples = op_read_float(
                    opusFile,
                    floatData + samplesDecodedTotal * channels,
                    (samplesRequested - samplesDecodedTotal) * channels,
                    0
                );

                if (samples <= 0) {
                    memset(floatData + samplesDecodedTotal * channels,
                        0,
                        (samplesRequested - samplesDecodedTotal) * channels * sizeof(float)
                    );

                    return FMOD_ERR_FILE_EOF;
                }

                samplesDecodedTotal += samples;
            }

            return FMOD_OK;
        };

        FMOD::System* system = FMODAudioEngine::get()->m_system;
        FMOD::Channel* channel = nullptr;
        FMOD::Sound* sound = nullptr;

        system->createSound(
            nullptr,
            FMOD_OPENUSER | FMOD_LOOP_OFF | FMOD_CREATESTREAM,
            &exinfo,
            &sound
        );

        system->playSound(sound, nullptr, false, &channel);

        SoundManager::add(channel, sound);
    }

    };