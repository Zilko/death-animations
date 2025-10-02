#include "BaseAnimation.hpp"

#include <opusfile.h>

class MrHippo : public BaseAnimation {

private:

    struct OpusData {
        OggOpusFile* opusFile;
        int channels;

        OpusData(OggOpusFile* opusFile, int channels)
            : opusFile(opusFile), channels(channels) {}

        ~OpusData() {
            op_free(opusFile);
        }
    };

    OpusData* m_data = nullptr;

    int m_speech = 1;

    static int read(void* stream, unsigned char* ptr, int nbytes) {
        return fread(ptr, 1, nbytes, static_cast<FILE*>(stream));
    }

    void playSpeech(float) {
        std::filesystem::path path = Mod::get()->getResourcesDir() / fmt::format("hippo-{}.opus", m_speech);

        FILE* fp = fopen(utils::string::pathToString(path).c_str(), "rb");

        if (!fp) return log::error("Mr. Hippo: Couldn't open file. {}", path);

        OpusFileCallbacks cb = {
            read,
            NULL,
            NULL,
            (op_close_func)fclose
        };
        int err = 0;

        OggOpusFile* opusFile = op_open_callbacks(fp, &cb, NULL, 0, &err);

        if (err != 0 || !opusFile) {
            fclose(fp);
            return log::error("Opus has failed you, boy. {}", err);
        }

        int channels = op_head(opusFile, 0)->channel_count;
        float duration = m_duration - 8.f;
        ogg_int64_t pcmTotal = duration * 48000;

        m_data = new OpusData(opusFile, channels);

        FMOD_CREATESOUNDEXINFO exinfo = {
            .cbsize = sizeof(FMOD_CREATESOUNDEXINFO),
            .length = static_cast<unsigned int>(pcmTotal * channels * sizeof(float)),
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
                    memset(
                        floatData + samplesDecodedTotal * channels,
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
        channel->setVolume(FMODAudioEngine::get()->m_musicVolume * 0.5f + 0.5f);

        SoundManager::add(channel, sound, duration + 10.f);

        scheduleOnce(schedule_selector(MrHippo::finish), duration);
    }

    void finish(float) {
        CCLayerColor* overlay = CCLayerColor::create({0, 0, 0, 0});
        overlay->runAction(CCFadeIn::create(5.f));

        addChild(overlay);
    }

    CCSprite* createSprite(const std::string& file, CCPoint anchor, int opacity) {
        CCSprite* spr = CCSprite::create(file.c_str());
        spr->setPosition(m_size * anchor);
        spr->setAnchorPoint(anchor);
        spr->setOpacity(opacity);

        if (m_size.width > m_size.height)
            spr->setScale(m_size.width / spr->getContentWidth());
        else
            spr->setScale(m_size.height / spr->getContentHeight());

        addChild(spr);

        return spr;
    }

    void showSpeech(const std::string& file1, const std::string& file2) {
        CCSprite* spr = createSprite(file1, {0, 1}, 0);

        spr->runAction(
            CCSequence::create(
                CCDelayTime::create(6.2f),
                CCFadeIn::create(25.f),
                CCDelayTime::create(13.f),
                CCScaleTo::create(240.f, spr->getScale() * 1.7676f),
                nullptr
            )
        );

        createSprite(file2, {0, 0}, 0)->runAction(
            CCSequence::create(
                CCMoveBy::create(0.f, {m_size.width * 0.3f, 0}),
                CCDelayTime::create(57.f),
                CCSpawn::create(
                    CCFadeTo::create(30.f, 120),
                    CCMoveTo::create(175.f, {-m_size.width, 0}),
                    nullptr
                ),
                nullptr
            )
        );
    }

    float getSpeechDuration(int speech) {
        switch (m_speech) {
            case 1: return 207.f;
            case 2: return 172.f;
            case 3: return 139.f;
            case 4: return 140.f;
        }

        return 207.f;
    }

    ~MrHippo() {
        SoundManager::stop();

        if (m_data)
            delete m_data;
    }

    ANIMATION_CTOR_CREATE(MrHippo) {
        m_speech = Utils::getSettingBool(Anim::MrHippo, "monologue");

        if (m_speech == 0)
            m_speech = Utils::getRandomInt(1, 4);

        m_duration = getSpeechDuration(m_speech) + 8.f;

        m_retryLayerDelay = m_duration - 0.3f;
    }
    
public:

    void start() override {
        Utils::setHighestZ(this);

        showSpeech(
            fmt::format("hippo-{}.png"_spr, Utils::getRandomInt(1, 3)),
            fmt::format("hippo-{}.png"_spr, Utils::getRandomInt(4, 6))
        );

        scheduleOnce(schedule_selector(MrHippo::playSpeech), 1.f);
    }

    };