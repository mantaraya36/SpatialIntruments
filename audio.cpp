
#include <vector>

#include "al/al_Allocore.hpp"
#include "al/ui/al_Parameter.hpp"
#include "al/ui/al_Preset.hpp"
#include "al/ui/al_PresetMIDI.hpp"
#include "al/ui/al_ParameterMIDI.hpp"
#include "al/io/al_MIDI.hpp"
//#include "alloaudio/al_SoundfileBuffered.hpp"

#include "common.hpp"

#include "chaos_synth.hpp"
#include "add_synth.hpp"
#include "granulator.hpp"
#include "downmixer.hpp"

#define CHAOS_SYNTH_POLYPHONY 1
#define ADD_SYNTH_POLYPHONY 1

using namespace std;
using namespace al;


class BaseAudioApp : public AudioCallback{
public:

	BaseAudioApp(){
		mAudioIO.append(*this);
		initAudio(44100);
	}

	void initAudio(
		double framesPerSec, unsigned framesPerBuffer=128,
		unsigned outChans=2, unsigned inChans=0
	){
		mAudioIO.framesPerSecond(framesPerSec);
		mAudioIO.framesPerBuffer(framesPerBuffer);
		mAudioIO.channelsOut(outChans);
		mAudioIO.channelsIn(inChans);
		gam::sampleRate(framesPerSec);
	}

	AudioIO& audioIO(){ return mAudioIO; }

	void start(bool block=true){
		mAudioIO.start();
        int c;
		if(block){
            do {
                c=getchar();
//                putchar (c);
                if (c == 's') {
                   printStatus();
                }
                if (c == 'w') {
                    std::cout << "ip" << std::endl;
                }
              } while (c != '.');
		}
	}

    virtual void printStatus() = 0;

private:
	AudioIO mAudioIO;
};

class AudioApp: public BaseAudioApp, public osc::PacketHandler
{
public:
    AudioApp(int midiChannel = 1) : BaseAudioApp()
    {
        for (int i = 0; i < 3; i++) {
            addSynth[i].outputRouting = {
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
                {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45},
                {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59}
            };
            addSynth3[i].outputRouting = {
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
                {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45},
                {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59}
            };
            addSynth4[i].outputRouting = {
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
                {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45},
                {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59}
            };
        }
        addSynth2.outputRouting = {
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
            {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45},
            {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59}
        };
        addSynthCampanas.outputRouting = {
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
            {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45},
            {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59}
        };


        mSequencer1a.setDirectory("sequences");
        mSequencer1a.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth[0].mFundamental.set(midi2cps(params[0]));
            app->addSynth[0].trigger(params[0]);
            std::cout << "Note On 1!! " << params[0]  << std::endl;
        }, this);
        mSequencer1a.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth[0].release(params[0]);
            std::cout << "Note Off 1!! " << params[0]  << std::endl;
        }, this);
        mSequencer1a.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program 1!! " << app->addSynth[0].mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);

        mSequencer1b.setDirectory("sequences");
        mSequencer1b.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth[1].mFundamental.set(midi2cps(params[0]));
            app->addSynth[1].trigger(params[0]);
            std::cout << "Note On 2!! " << params[0]  << std::endl;
        }, this);
        mSequencer1b.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth[1].release(params[0]);
            std::cout << "Note Off 2!! " << params[0]  << std::endl;
        }, this);
        mSequencer1b.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program 2!! " << app->addSynth[1].mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);

        mSequencer1c.setDirectory("sequences");
        mSequencer1c.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth[2].mFundamental.set(midi2cps(params[0]));
            app->addSynth[2].trigger(params[0]);
            std::cout << "Note On 3!! " << params[0]  << std::endl;
        }, this);
        mSequencer1c.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth[2].release(params[0]);
            std::cout << "Note Off 3!! " << params[0]  << std::endl;
        }, this);
        mSequencer1c.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program 3!! " << app->addSynth[2].mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);

        mSequencer2.setDirectory("sequences");
        mSequencer2.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth2.mFundamental.set(midi2cps(params[0]));
            app->addSynth2.trigger(params[0]);
            std::cout << "Note On!! " << params[0]  << std::endl;
        }, this);
        mSequencer2.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth2.release(params[0]);
            std::cout << "Note Off!! " << params[0]  << std::endl;
        }, this);
        mSequencer2.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program!! " << app->addSynth2.mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);


        mSequencer3a.setDirectory("sequences");
        mSequencer3a.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth3[0].mFundamental.set(midi2cps(params[0]));
            app->addSynth3[0].trigger(params[0]);
            std::cout << "Note On!! " << params[0]  << std::endl;
        }, this);
        mSequencer3a.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth3[0].release(params[0]);
            std::cout << "Note Off!! " << params[0]  << std::endl;
        }, this);
        mSequencer3a.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program!! " << app->addSynth3[0].mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);

        mSequencer3b.setDirectory("sequences");
        mSequencer3b.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth3[1].mFundamental.set(midi2cps(params[0]));
            app->addSynth3[1].trigger(params[0]);
            std::cout << "Note On!! " << params[0]  << std::endl;
        }, this);
        mSequencer3b.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth3[1].release(params[0]);
            std::cout << "Note Off!! " << params[0]  << std::endl;
        }, this);
        mSequencer3b.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program!! " << app->addSynth3[1].mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);

        mSequencer3c.setDirectory("sequences");
        mSequencer3c.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth3[2].mFundamental.set(midi2cps(params[0]));
            app->addSynth3[2].trigger(params[0]);
            std::cout << "Note On!! " << params[0]  << std::endl;
        }, this);
        mSequencer3c.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth3[2].release(params[0]);
            std::cout << "Note Off!! " << params[0]  << std::endl;
        }, this);
        mSequencer3c.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program!! " << app->addSynth3[2].mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);


        mSequencer4a.setDirectory("sequences");
        mSequencer4a.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth4[0].mFundamental.set(midi2cps(params[0]));
            app->addSynth4[0].trigger(params[0]);
            std::cout << "Note On!! " << params[0]  << std::endl;
        }, this);
        mSequencer4a.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth4[0].release(params[0]);
            std::cout << "Note Off!! " << params[0]  << std::endl;
        }, this);
        mSequencer4a.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program!! " << app->addSynth4[0].mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);

        mSequencer4b.setDirectory("sequences");
        mSequencer4b.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth4[1].mFundamental.set(midi2cps(params[0]));
            app->addSynth4[1].trigger(params[0]);
            std::cout << "Note On!! " << params[0]  << std::endl;
        }, this);
        mSequencer4b.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth4[1].release(params[0]);
            std::cout << "Note Off!! " << params[0]  << std::endl;
        }, this);
        mSequencer4b.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program!! " << app->addSynth4[1].mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);

        mSequencer4c.setDirectory("sequences");
        mSequencer4c.registerEventCommand("ON", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth4[2].mFundamental.set(midi2cps(params[0]));
            app->addSynth4[2].trigger(params[0]);
            std::cout << "Note On!! " << params[0]  << std::endl;
        }, this);
        mSequencer4c.registerEventCommand("OFF", [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            app->addSynth4[2].release(params[0]);
            std::cout << "Note Off!! " << params[0]  << std::endl;
        }, this);
        mSequencer4c.registerEventCommand("PROGRAM",
                                         [](void *data, std::vector<float> &params)
        {
            AudioApp *app = static_cast<AudioApp *>(data);
            std::cout << "Program!! " << app->addSynth4[2].mPresetHandler.recallPresetSynchronous(params[0]) << std::endl;
        }, this);
    }

    static inline float midi2cps(int midiNote) {
        return 440.0 * pow(2, (midiNote - 69.0)/ 12.0);
    }

    void init() {
        mFromSimulator.handler(*this);
        mFromSimulator.timeout(0.005);
        mFromSimulator.start();
    }

    void trigger1();
    void trigger2();
    void trigger22();

    virtual void onAudioCB(AudioIOData &io) override;
    virtual void onMessage(osc::Message &m) override {
        if (m.addressPattern() == "/chaos" && m.typeTags() == "f") {
            mPrevChaos = mChaos;
            m >> mChaos;
        }
    }
    virtual void printStatus() override {
        std::cout << "On States: ";
        for (int i : mCampanitasStates) {
            std::cout << i <<  "  ";
        }
        std::cout << std::endl;
    }

private:
    // Synthesis
    AddSynth addSynth[3];
    AddSynth addSynth2;
    AddSynth addSynth3[3];
    AddSynth addSynth4[3];
    AddSynth addSynthCampanas;

    // Sequence players
    PresetSequencer mSequencer1a;
    PresetSequencer mSequencer1b;
    PresetSequencer mSequencer1c;
    PresetSequencer mSequencer2;
    PresetSequencer mSequencer3a;
    PresetSequencer mSequencer3b;
    PresetSequencer mSequencer3c;
    PresetSequencer mSequencer4a;
    PresetSequencer mSequencer4b;
    PresetSequencer mSequencer4c;

    // Schedule Messages
    MsgQueue msgQueue;

    DownMixer mDownMixer;

    osc::Recv mFromSimulator {AUDIO_IN_PORT, AUDIO_IP_ADDRESS};

    float mChaos {0};
    float mPrevChaos {0};
    std::list<int> mCampanitasStates;
    int mCampanitasCounter[45] {0};
};

static void releaseAddSynth(al_sec timestamp, AddSynth *addSynth, int id)
{
    addSynth->release(id);
    std::cout << "release" << std::endl;
}

void AudioApp::trigger1() {
    std::cout << "CAMPANITAS 1 Trigger" << std::endl;
    addSynthCampanas.mPresetHandler.recallPresetSynchronous(1);

    int midinote = rnd::uniform(80,50);
    addSynthCampanas.mFundamental = midi2cps(midinote);
    addSynthCampanas.mLevel = 0.2;
    addSynthCampanas.trigger(midinote);
    msgQueue.send(msgQueue.now() + rnd::uniform(1.0, 0.3), releaseAddSynth, &addSynthCampanas, midinote);

    midinote = rnd::uniform(80,50);
    addSynthCampanas.mFundamental = midi2cps(midinote);
    addSynthCampanas.trigger(midinote);
    msgQueue.send(msgQueue.now() + rnd::uniform(1.0, 0.3), releaseAddSynth, &addSynthCampanas, midinote);

    midinote = rnd::uniform(80,50);
    addSynthCampanas.mFundamental = midi2cps(midinote);
    addSynthCampanas.trigger(midinote);
    msgQueue.send(msgQueue.now() + rnd::uniform(1.0, 0.3), releaseAddSynth, &addSynthCampanas, midinote);

    midinote = rnd::uniform(80,50);
    addSynthCampanas.mFundamental = midi2cps(midinote);
    addSynthCampanas.trigger(midinote);
    msgQueue.send(msgQueue.now() + rnd::uniform(1.0, 0.3), releaseAddSynth, &addSynthCampanas, midinote);

    midinote = rnd::uniform(80,50);
    addSynthCampanas.mFundamental = midi2cps(midinote);
    addSynthCampanas.trigger(midinote);
    msgQueue.send(msgQueue.now() + rnd::uniform(1.0, 0.3), releaseAddSynth, &addSynthCampanas, midinote);
}

void AudioApp::trigger2()
{
    std::cout << "CAMPANITAS 2 Trigger" << std::endl;
    addSynthCampanas.mPresetHandler.recallPresetSynchronous(2);

    int midinote = rnd::uniform(48,28);
    addSynthCampanas.mFundamental = midi2cps(midinote);
    addSynthCampanas.mLevel = 80;
    addSynthCampanas.trigger(midinote);
    msgQueue.send(msgQueue.now() + 4, releaseAddSynth, &addSynthCampanas, midinote);
};

void AudioApp::trigger22()
{
    std::cout << "CAMPANITAS 22 Trigger" << std::endl;
    addSynthCampanas.mPresetHandler.recallPresetSynchronous(22);

    int midinote = 36;
    addSynthCampanas.mFundamental = midi2cps(midinote);
    addSynthCampanas.mLevel = 300;
    addSynthCampanas.trigger(midinote);
    msgQueue.send(msgQueue.now() + 25, releaseAddSynth, &addSynthCampanas, midinote);
};

static void turnoffSeq(al_sec timestamp, AddSynth * addSynth)
{
    addSynth[0].allNotesOff();
    addSynth[1].allNotesOff();
    addSynth[2].allNotesOff();
}

void AudioApp::onAudioCB(AudioIOData &io)
{
    int bufferSize = io.framesPerBuffer();
    float *swBuffer = io.outBuffer(47);
    bool consumeChaos = false;

    // Campanitas

    float max = 0.3;
    if (mChaos < max) {
        float probCampanitas = 0.0005 + (mChaos/max) * 0.002;
        if (rnd::prob(probCampanitas)) {
            std::cout << "trigger" << std::endl;
            addSynthCampanas.mPresetHandler.recallPresetSynchronous("34");
            addSynthCampanas.mLayer = rnd::uniform(3);
            addSynthCampanas.mLevel = 0.09;
            addSynthCampanas.mArcSpan = rnd::uniform(0.5, 2.0);
            addSynthCampanas.mArcStart = rnd::uniform();
            addSynthCampanas.mCumulativeDelayRandomness = addSynthCampanas.mCumulativeDelayRandomness +rnd::uniform(0.2, -0.2);
            addSynthCampanas.mFundamental = addSynthCampanas.mFundamental + rnd::uniform(40, -40);
            addSynthCampanas.trigger(0);
            msgQueue.send(msgQueue.now() + 2.5, releaseAddSynth, &addSynthCampanas, 0);
        }
    }

    float TimeDelta;
    float ifNotTime;
    int stateCamp;
    if (!mSequencer1a.running() && !mSequencer2.running() && !mSequencer3a.running() && !mSequencer4a.running()) {
        // 1 --------------------
        TimeDelta = 8;
        ifNotTime = 2;
        stateCamp = 1;
        if (mPrevChaos < 0.15 && mChaos >= 0.15) {
            mCampanitasStates.push_back(stateCamp);
            std::cout << "ENTER campanitas stateCamp" << std::endl;
            if (rnd::prob(0.1)) {
                trigger1();
                mCampanitasCounter[stateCamp] = 0;
                std::cout << "CAMPANITAS 1 Launch" << std::endl;
            } else {
                mCampanitasCounter[stateCamp] =  (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
            }
            consumeChaos = true;
        } else if (mPrevChaos < 0.3 && mChaos >= 0.3) {

            std::cout << "EXIT campanitas stateCamp" << std::endl;
            mCampanitasStates.remove(stateCamp);
            consumeChaos = true;
        }  else if (mPrevChaos > 0.15 && mChaos <= 0.15) {
            std::cout << "EXIT campanitas stateCamp" << std::endl;
            mCampanitasStates.remove(stateCamp);
            consumeChaos = true;
        }

        if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
            mCampanitasCounter[stateCamp]++;
            if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
                if (rnd::prob(0.7)) {
                    std::cout << "CAMPANITAS 1 Launch" << std::endl;
                    trigger1();
                    mCampanitasCounter[stateCamp] = 0;
                    std::cout << "campanitas 1" << std::endl;
                } else {
                    mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
                }
            }
        }
        // 2 --------------------
        TimeDelta = 6;
        ifNotTime = 2;
        stateCamp = 2;
        if (mPrevChaos > 0.2 && mChaos <= 0.2) {
            mCampanitasStates.push_back(stateCamp);
            std::cout << "ENTER campanitas stateCamp 2" << std::endl;
            if (rnd::prob(0.5)) {
                trigger2();
                mCampanitasCounter[stateCamp] = 0;
            } else {
                mCampanitasCounter[stateCamp] =  (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
            }
            consumeChaos = true;
        } else if (mPrevChaos > 0.1 && mChaos <= 0.1) {
            std::cout << "EXIT campanitas stateCamp " << std::endl;
            mCampanitasStates.remove(stateCamp);
            consumeChaos = true;
        }  else if (mPrevChaos > 0.22 && mChaos <= 0.22) {
            std::cout << "EXIT campanitas state " << std::endl;
            mCampanitasStates.remove(stateCamp);
            consumeChaos = true;
        }

        if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
            mCampanitasCounter[stateCamp]++;
            if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
                if (rnd::prob(0.7)) {
                    trigger2();
                    mCampanitasCounter[stateCamp] = 0;
                    std::cout << "campanitas 1" << std::endl;
                } else {
                    mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
                }
            }
        }
		// 9 Oh Boy Bottom row AM --------------------
		TimeDelta = 30; // separacion entre notas
		ifNotTime = 2; //
		stateCamp = 9; //numero de preset
		if (mPrevChaos > 0.4 && mChaos <= 0.4) { //umbral de entrada
			mCampanitasStates.push_back(stateCamp);
			std::cout << "ENTER 9 Oh Boy Bottom row AM" << std::endl;//print esto
			mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
			consumeChaos = true;
		} else if (mPrevChaos < 0.4 && mChaos >= 0.4) { //umbral de salida subiendo
			std::cout << "EXIT 9 Oh boy bottom row AM" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		}  else if (mPrevChaos > 0.2 && mChaos <= 0.2) { //umbral de salida bajando
			std::cout << "EXIT 9 Oh boy bottom row AM" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		}
		
		if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
			mCampanitasCounter[stateCamp]++;
			if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
				if (rnd::prob(0.5)) { // prob
					std::cout << "9 Oh boy bottom row AM Trigger" << std::endl;
					addSynthCampanas.mPresetHandler.recallPresetSynchronous(9); // preset
					int midinote = rnd::uniform(40,30); //rango de notas entre MIDI 36 y 20. Si es solo una es el numero despues del =
					addSynthCampanas.mFundamental = midi2cps(midinote);
					addSynthCampanas.mLevel = 1;// nivel
					addSynthCampanas.trigger(midinote);
					msgQueue.send(msgQueue.now() + 15, releaseAddSynth, &addSynthCampanas, midinote); // duracion
					
					mCampanitasCounter[stateCamp] = 0;
					std::cout << "9 Oh boy bottom row AM" << std::endl;
				} else {
					mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
				}
			}
		}
		// 10 Oh Boy Bottom its FM 2 --------------------
		TimeDelta = 12; // separacion entre notas
		ifNotTime = 4; //
		stateCamp = 10; //numero de preset
		if (mPrevChaos < 0.55 && mChaos >= 0.55) { //umbral de entrada subiendo
			mCampanitasStates.push_back(stateCamp);
			std::cout << "ENTER 10 Oh Boy Its FM2" << std::endl;//print esto
			mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
			consumeChaos = true;
		} else if (mPrevChaos < 0.75 && mChaos >= 0.75) { //umbral de salida subiendo
			std::cout << "EXIT 10 Oh Boy Its FM2" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		} else if (mPrevChaos > 0.55 && mChaos <= 0.55) { //umbral de entrada bajando
			if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) == mCampanitasStates.end() ) {
				mCampanitasStates.push_back(stateCamp);
				std::cout << "ENTER 10 Oh Boy Its FM2" << std::endl;//print esto
				mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
				consumeChaos = true;
			}
		}  else if (mPrevChaos > 0.47 && mChaos <= 0.47) { //umbral de salida bajando
			std::cout << "EXIT 10 Oh Boy Its FM2" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		}
		
		if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
			mCampanitasCounter[stateCamp]++;
			if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
				if (rnd::prob(0.2)) { // probabilidad
					std::cout << "10 Oh Boy Its FM2 Trigger" << std::endl;
					addSynthCampanas.mPresetHandler.recallPresetSynchronous(10); // preset
					int midinote = rnd::uniform(76,24); //rango de notas entre MIDI 36 y 20. Si es solo una es el numero despues del =
					addSynthCampanas.mFundamental = midi2cps(midinote);
					addSynthCampanas.mLevel = 0.6;// nivel
					addSynthCampanas.trigger(midinote);
					msgQueue.send(msgQueue.now() + rnd::uniform(12,9), releaseAddSynth, &addSynthCampanas, midinote); // duracion
					
					mCampanitasCounter[stateCamp] = 0;
					std::cout << "10 Oh Boy Its FM2" << std::endl;
				} else {
					mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
				}
			}
		}
		// 12 Bells 1 --------------------
		TimeDelta = 0.05; // separacion entre notas
		ifNotTime = 4; //
		stateCamp = 12; //numero de preset
		if (mPrevChaos < 0.55 && mChaos >= 0.55) { //umbral de entrada subiendo
			mCampanitasStates.push_back(stateCamp);
			std::cout << "ENTER 12 Bells 1" << std::endl;//print esto
			mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
			consumeChaos = true;
		} else if (mPrevChaos < 0.75 && mChaos >= 0.75) { //umbral de salida subiendo
			std::cout << "EXIT 10 12 Bells 1" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		} else if (mPrevChaos > 0.55 && mChaos <= 0.55) { //umbral de entrada bajando
			if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) == mCampanitasStates.end() ) {
				mCampanitasStates.push_back(stateCamp);
				std::cout << "ENTER 12 Bells 1" << std::endl;//print esto
				mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
				consumeChaos = true;
			}
		}  else if (mPrevChaos > 0.3 && mChaos <= 0.3) { //umbral de salida bajando
			std::cout << "EXIT 12 Bells 1" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		}
		
		if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
			mCampanitasCounter[stateCamp]++;
			if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
				if (rnd::prob(0.4)) { // probabilidad
					std::cout << "12 Bells 1 Trigger" << std::endl;
					addSynthCampanas.mPresetHandler.recallPresetSynchronous(12); // preset
					int midinote = rnd::uniform(110,63); //rango de notas entre MIDI 36 y 20. Si es solo una es el numero despues del =
					addSynthCampanas.mFundamental = midi2cps(midinote);
					addSynthCampanas.mLevel = 0.6;// nivel
					addSynthCampanas.trigger(midinote);
					msgQueue.send(msgQueue.now() + rnd::uniform(12,9), releaseAddSynth, &addSynthCampanas, midinote); // duracion
					
					mCampanitasCounter[stateCamp] = 0;
					std::cout << "12 Bells 1" << std::endl;
				} else {
					mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
				}
			}
		}
		// 37 Is It a drop --------------------
		TimeDelta = 10; // separacion entre notas
		ifNotTime = 1; //
		stateCamp = 37; //numero de preset
		if (mPrevChaos < 0.65 && mChaos >= 0.65) { //umbral de entrada subiendo
			mCampanitasStates.push_back(stateCamp);
			std::cout << "ENTER 37 Is It  a   D R O P   ?" << std::endl;//print esto
			mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
			consumeChaos = true;
		} else if (mPrevChaos < 0.75 && mChaos >= 0.75) { //umbral de salida subiendo
			std::cout << "EXIT 37 Is It  a   D R O P   ?" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		} else if (mPrevChaos > 0.75 && mChaos <= 0.75) { //umbral de entrada bajando
			if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) == mCampanitasStates.end() ) {
				mCampanitasStates.push_back(stateCamp);
				std::cout << "ENTER 37 Is It  a   D R O P   ?" << std::endl;//print esto
				mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
				consumeChaos = true;
			}
		}  else if (mPrevChaos > 0.65 && mChaos <= 0.65) { //umbral de salida bajando
			std::cout << "EXIT 37 Is It  a   D R O P   ?" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		}
		
		if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
			mCampanitasCounter[stateCamp]++;
			if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
				if (rnd::prob(0.4)) { // probabilidad
					std::cout << "37 Is It  a   D R O P   ? Trigger" << std::endl;
					addSynthCampanas.mPresetHandler.recallPresetSynchronous(37); // preset
					int midinote = rnd::uniform(88,40); //rango de notas entre MIDI 36 y 20. Si es solo una es el numero despues del =
					addSynthCampanas.mFundamental = midi2cps(midinote);
					addSynthCampanas.mLevel = 0.9;// nivel
					addSynthCampanas.trigger(midinote);
					msgQueue.send(msgQueue.now() + 15, releaseAddSynth, &addSynthCampanas, midinote); // duracion. En este caso quiero que la duración se mayor que 15, no solo 15
					
					mCampanitasCounter[stateCamp] = 0;
					std::cout << "37 Is It  a   D R O P   ?" << std::endl;
				} else {
					mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
				}
			}
		}
		// 38 Slow FM Bells --------------------
		TimeDelta = 2; // separacion entre notas
		ifNotTime = 2; //
		stateCamp = 38; //numero de preset
		if (mPrevChaos < 0.25 && mChaos >= 0.25) { //umbral de entrada subiendo
			mCampanitasStates.push_back(stateCamp);
			std::cout << "ENTER 38 Slow   F M   B  e l l s" << std::endl;//print esto
			mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
			consumeChaos = true;
		} else if (mPrevChaos < 0.50 && mChaos >= 0.50) { //umbral de salida subiendo
			std::cout << "EXIT 38 Slow   F M   B  e l l s" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		} else if (mPrevChaos > 0.5 && mChaos <= 0.5) { //umbral de entrada bajando
			if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) == mCampanitasStates.end() ) {
				mCampanitasStates.push_back(stateCamp);
				std::cout << "ENTER 38 Slow   F M   B  e l l s" << std::endl;//print esto
				mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
				consumeChaos = true;
			}
		}  else if (mPrevChaos > 0.35 && mChaos <= 0.35) { //umbral de salida bajando
			std::cout << "EXIT 38 Slow   F M   B  e l l s" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		}  else if (mPrevChaos > 0.24 && mChaos <= 0.24) { //umbral de salida bajando
            std::cout << "EXIT 38 Slow   F M   B  e l l s" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		}
		
		if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
			mCampanitasCounter[stateCamp]++;
			if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
				if (rnd::prob(0.3)) { // probabilidad Quiero que sea diferente subiendo que bajando. Subiendo 30 bajando 10
					std::cout << "38 Slow   F M   B  e l l s Trigger" << std::endl;
					addSynthCampanas.mPresetHandler.recallPresetSynchronous(38); // preset
					int midinote = rnd::uniform(70,28); //rango de notas entre MIDI 36 y 20. Si es solo una es el numero despues del =
					addSynthCampanas.mFundamental = midi2cps(midinote);
					addSynthCampanas.mLevel = 0.9;// nivel
					addSynthCampanas.trigger(midinote);
					msgQueue.send(msgQueue.now() + 2.5, releaseAddSynth, &addSynthCampanas, midinote); // duracion.
					
					mCampanitasCounter[stateCamp] = 0;
					std::cout << "38 Slow   F M   B  e l l s" << std::endl;
				} else {
					mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
				}
			}
		}
		// 41 Slow FMs --------------------
		TimeDelta = 12; // separacion entre notas
		ifNotTime = 4; //
		stateCamp = 41; //numero de preset
		if (mPrevChaos < 0.55 && mChaos >= 0.55) { //umbral de entrada subiendo
			mCampanitasStates.push_back(stateCamp);
			std::cout << "ENTER 41 Slow   F M s" << std::endl;//print esto
			mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
			consumeChaos = true;
		} else if (mPrevChaos < 0.75 && mChaos >= 0.75) { //umbral de salida subiendo
			std::cout << "EXIT 41 Slow   F M s" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		}
        if (mPrevChaos > 0.75 && mChaos <= 0.75) { //umbral de entrada bajando
			if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) == mCampanitasStates.end() ) {
				mCampanitasStates.push_back(stateCamp);
				std::cout << "ENTER 41 Slow   F M s" << std::endl;//print esto
				mCampanitasCounter[stateCamp] = TimeDelta * io.framesPerSecond()/ io.framesPerBuffer();
				consumeChaos = true;
			}
		} else if (mPrevChaos > 0.54 && mChaos <= 0.54) { //umbral de salida bajando
			std::cout << "EXIT 41 Slow   F M s" << std::endl;
			mCampanitasStates.remove(stateCamp);
			consumeChaos = true;
		}
		
		if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
			mCampanitasCounter[stateCamp]++;
			if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
				if (rnd::prob(0.15)) { // probabilidad
					std::cout << "41 Slow   F M s Trigger" << std::endl;
					addSynthCampanas.mPresetHandler.recallPresetSynchronous(41); // preset
					int midinote = rnd::uniform(63,24); //rango de notas entre MIDI 36 y 20. Si es solo una es el numero despues del =
					addSynthCampanas.mFundamental = midi2cps(midinote);
					addSynthCampanas.mLevel = 0.9;// nivel
					addSynthCampanas.trigger(midinote);
					msgQueue.send(msgQueue.now() + 20, releaseAddSynth, &addSynthCampanas, midinote); // duracion.
					
					mCampanitasCounter[stateCamp] = 0;
					std::cout << "41 Slow   F M s" << std::endl;
				} else {
					mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
				}
			}
		}


    }
    // 22 simple bass --------------------
    TimeDelta = 40;
    ifNotTime = 2;
    stateCamp = 22;
    if (mPrevChaos < 0.5 && mChaos >= 0.5) {
        mCampanitasStates.push_back(stateCamp);
        std::cout << "ENTER campanitas stateCamp 22" << std::endl;
        if (rnd::prob(0.5)) {
            trigger22();
            mCampanitasCounter[stateCamp] = 0;
        } else {
            mCampanitasCounter[stateCamp] =  (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
        }
        consumeChaos = true;
    } else if (mPrevChaos > 0.49 && mChaos <= 0.49) {
        std::cout << "EXIT campanitas stateCamp 22" << std::endl;
        mCampanitasStates.remove(stateCamp);
        consumeChaos = true;
    }/*  else if (mPrevChaos < 0.49 && mChaos >= 0.49) {
        std::cout << "EXIT campanitas state " << std::endl;
        mCampanitasStates.remove(stateCamp);
consumeChaos = true;
    }*/

    if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
        mCampanitasCounter[stateCamp]++;
        if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
            if (rnd::prob(0.5)) {
                trigger22();
                mCampanitasCounter[stateCamp] = 0;
                std::cout << "campanitas" << stateCamp << std::endl;
            } else {
                mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
            }
        }
    }


    // 3,4,5 Beating --------------------
    TimeDelta = 20;
    ifNotTime = 2;
    stateCamp = 3;
    if (mPrevChaos < 0.8 && mChaos >= 0.8) {
        mCampanitasStates.push_back(stateCamp);
        std::cout << "ENTER beating" << std::endl;
        mCampanitasCounter[stateCamp] =  io.framesPerSecond()/ io.framesPerBuffer();
        consumeChaos = true;
    } else if (mPrevChaos > 0.7 && mChaos <= 0.7) {
        std::cout << "EXIT campanitas stateCamp 22" << std::endl;
        mCampanitasStates.remove(stateCamp);
        consumeChaos = true;
    }

    if (std::find(mCampanitasStates.begin(), mCampanitasStates.end(), stateCamp) != mCampanitasStates.end()) {
        mCampanitasCounter[stateCamp]++;
        if (mCampanitasCounter[stateCamp] > TimeDelta * io.framesPerSecond()/ io.framesPerBuffer()) {
            if (rnd::prob(0.7)) {
                float dur = rnd::uniform(20.0, 8.0);
                std::cout << "41 Slow   F M s Trigger" << std::endl;

                addSynthCampanas.mPresetHandler.recallPresetSynchronous(3); // preset
                int midinote = 36; //rango de notas entre MIDI 36 y 20. Si es solo una es el numero despues del =
                addSynthCampanas.mFundamental = midi2cps(midinote);
                addSynthCampanas.mLevel = 0.9;// nivel
                addSynthCampanas.trigger(midinote);
                msgQueue.send(msgQueue.now() + dur, releaseAddSynth, &addSynthCampanas, midinote); // duracion.

                midinote = 43;
                addSynthCampanas.mFundamental = midi2cps(midinote);
                addSynthCampanas.mLevel = 0.9;// nivel
                addSynthCampanas.trigger(midinote);
                msgQueue.send(msgQueue.now() + dur, releaseAddSynth, &addSynthCampanas, midinote); // duracion.

                midinote = 50;
                addSynthCampanas.mFundamental = midi2cps(midinote);
                addSynthCampanas.mLevel = 0.9;// nivel
                addSynthCampanas.trigger(midinote);
                msgQueue.send(msgQueue.now() + dur, releaseAddSynth, &addSynthCampanas, midinote); // duracion.

                addSynthCampanas.mPresetHandler.recallPresetSynchronous(4); // preset

                midinote = 36; //rango de notas entre MIDI 36 y 20. Si es solo una es el numero despues del =
                addSynthCampanas.mFundamental = midi2cps(midinote);
                addSynthCampanas.mLevel = 0.9;// nivel
                addSynthCampanas.trigger(midinote -12);
                msgQueue.send(msgQueue.now() + dur, releaseAddSynth, &addSynthCampanas, midinote-12); // duracion.

                midinote = 43;
                addSynthCampanas.mFundamental = midi2cps(midinote);
                addSynthCampanas.mLevel = 0.9;// nivel
                addSynthCampanas.trigger(midinote -12);
                msgQueue.send(msgQueue.now() + dur, releaseAddSynth, &addSynthCampanas, midinote -12); // duracion.

                midinote = 50;
                addSynthCampanas.mFundamental = midi2cps(midinote);
                addSynthCampanas.mLevel = 0.9;// nivel
                addSynthCampanas.trigger(midinote -12);
                msgQueue.send(msgQueue.now() + dur, releaseAddSynth, &addSynthCampanas, midinote -12); // duracion.

                addSynthCampanas.mPresetHandler.recallPresetSynchronous(5); // preset

                midinote = 36; //rango de notas entre MIDI 36 y 20. Si es solo una es el numero despues del =
                addSynthCampanas.mFundamental = midi2cps(midinote);
                addSynthCampanas.mLevel = 0.9;// nivel
                addSynthCampanas.trigger(midinote -24);
                msgQueue.send(msgQueue.now() + dur, releaseAddSynth, &addSynthCampanas, midinote-24); // duracion.

                midinote = 43;
                addSynthCampanas.mFundamental = midi2cps(midinote);
                addSynthCampanas.mLevel = 0.9;// nivel
                addSynthCampanas.trigger(midinote -24);
                msgQueue.send(msgQueue.now() + dur, releaseAddSynth, &addSynthCampanas, midinote -24); // duracion.

                midinote = 50;
                addSynthCampanas.mFundamental = midi2cps(midinote);
                addSynthCampanas.mLevel = 0.9;// nivel
                addSynthCampanas.trigger(midinote -24);
                msgQueue.send(msgQueue.now() + dur, releaseAddSynth, &addSynthCampanas, midinote -24); // duracion.

                mCampanitasCounter[stateCamp] = 0;
                std::cout << "campanitas" << stateCamp << std::endl;
            } else {
                mCampanitasCounter[stateCamp] = (TimeDelta - ifNotTime) * io.framesPerSecond()/ io.framesPerBuffer();
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        addSynth[i].generateAudio(io);
        addSynth3[i].generateAudio(io);
        addSynth4[i].generateAudio(io);
    }
//    addSynth2.generateAudio(io);
    addSynthCampanas.generateAudio(io);

    /// Sequences
    ///
    ///
    float rangeStart;
    float rangeEnd;


    rangeStart = 0.1;
    rangeEnd = 0.2;
    if ((mPrevChaos < rangeStart &&  mChaos >= rangeStart)
            || (mPrevChaos > rangeEnd &&  mChaos <= rangeEnd)
            ) {
        if (!mSequencer4a.running()) {
            mSequencer3a.stopSequence();
            mSequencer3b.stopSequence();
            mSequencer3c.stopSequence();
            addSynth3[0].allNotesOff();
            addSynth3[1].allNotesOff();
            addSynth3[2].allNotesOff();

            mSequencer2.stopSequence();
            addSynth2.allNotesOff();

            mSequencer1a.stopSequence();
            mSequencer1b.stopSequence();
            mSequencer1c.stopSequence();
            addSynth[0].allNotesOff();
            addSynth[1].allNotesOff();
            addSynth[2].allNotesOff();
            mSequencer4a.playSequence("Seq 4a-0");
            mSequencer4b.playSequence("Seq 4b-0");
            mSequencer4c.playSequence("Seq 4c-0");
            std::cout << "Seq 4" << std::endl;
            consumeChaos = true;
        }
    }

//    rangeStart = 0.25;
////    rangeEnd = 0.5;
//    if ((mPrevChaos < rangeStart &&  mChaos >= rangeStart)
//            || (mPrevChaos > rangeStart &&  mChaos <= rangeStart)
//            ) {
//        if (!mSequencer2.running()) {
//            mSequencer1a.stopSequence();
//            mSequencer1b.stopSequence();
//            mSequencer1c.stopSequence();
//            addSynth[0].allNotesOff();
//            addSynth[1].allNotesOff();
//            addSynth[2].allNotesOff();
//            mSequencer3a.stopSequence();
//            mSequencer3b.stopSequence();
//            mSequencer3c.stopSequence();
//            addSynth3[0].allNotesOff();
//            addSynth3[1].allNotesOff();
//            addSynth3[2].allNotesOff();
//            mSequencer2.playSequence("Seq 2-1");
//            std::cout << "Seq 2" << std::endl;
//        }
//    }

    rangeStart = 0.3;
    rangeEnd = 0.4;
    if ((mPrevChaos < rangeStart &&  mChaos >= rangeStart)
            || (mPrevChaos > rangeEnd &&  mChaos <= rangeEnd)
            ) {
        if (!mSequencer3a.running()) {
            mSequencer1a.stopSequence();
            mSequencer1b.stopSequence();
            mSequencer1c.stopSequence();
            mSequencer2.stopSequence();
//            addSynth2.allNotesOff();
            mSequencer4a.stopSequence();
            mSequencer4b.stopSequence();
            mSequencer4c.stopSequence();
//            addSynth3[0].allNotesOff();
//            addSynth3[1].allNotesOff();
//            addSynth3[3].allNotesOff();
//            addSynth2.allNotesOff();
            addSynth[0].allNotesOff();
            addSynth[1].allNotesOff();
            addSynth[2].allNotesOff();

            msgQueue.send(msgQueue.now() + 3.5, turnoffSeq, addSynth4); // duracion.
            mSequencer3a.playSequence("Seq 3-1");
            mSequencer3b.playSequence("Seq 3-2");
            mSequencer3c.playSequence("Seq 3-3");
            std::cout << "Seq 3" << std::endl;
            consumeChaos = true;
        }
    }

    ///
    rangeStart = 0.5;
    rangeEnd = 0.6;
    if ((mPrevChaos < rangeStart &&  mChaos >= rangeStart)
            || (mPrevChaos > rangeEnd &&  mChaos <= rangeEnd)
            ) {
        if (!mSequencer1a.running()) {
//            mSequencer2.stopSequence();
//            addSynth2.allNotesOff();
            mSequencer3a.stopSequence();
            mSequencer3b.stopSequence();
            mSequencer3c.stopSequence();
            msgQueue.send(msgQueue.now() + 2.0, turnoffSeq, addSynth3); // duracion.
            mSequencer1a.playSequence("Seq 1-1");
            mSequencer1b.playSequence("Seq 1-2");
            mSequencer1c.playSequence("Seq 1-3");
            std::cout << "Seq 1" << std::endl;
            consumeChaos = true;
        }
    }

    for (int i = 0; i < bufferSize; i++) {
        *swBuffer++ *= 0.07;
    }

    msgQueue.advance(io.framesPerBuffer()/io.framesPerSecond());

    if (mChaos == 0 && mPrevChaos > 0.0) {
        mCampanitasStates.clear();
    }

    if (consumeChaos) {
        mPrevChaos = mChaos;
    }
//    std::cout << "-------------" << std::endl;
//    for (int i= 0; i < 60; i++) {
//        std::cout << io.out(i, 0) << " ";
//    }
//    std::cout << std::endl;

#ifndef BUILDING_FOR_ALLOSPHERE
    mDownMixer.process(io);
#endif
}

int main(int argc, char *argv[] )
{
    AudioApp app;

    int outChans = 60;
#ifdef BUILDING_FOR_ALLOSPHERE
    app.audioIO().device(AudioDevice("ECHO X5"));
#endif
    app.initAudio(44100, 400, outChans, 0);
    gam::sampleRate(app.audioIO().fps());

//    AudioDevice::printAll();
    app.audioIO().print();
    app.init();
    app.start();
    return 0;
}
