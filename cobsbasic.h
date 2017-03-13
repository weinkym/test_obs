#ifndef COBSBASIC_H
#define COBSBASIC_H

#include "cobspublic.h"
#include "util/util.hpp"
#include "cbasicoutputhandler.h"
#include "cobsglobal.h"

#define DESKTOP_AUDIO_1 Str("DesktopAudioDevice1")
#define DESKTOP_AUDIO_2 Str("DesktopAudioDevice2")
#define AUX_AUDIO_1     Str("AuxAudioDevice1")
#define AUX_AUDIO_2     Str("AuxAudioDevice2")
#define AUX_AUDIO_3     Str("AuxAudioDevice3")

#define SIMPLE_ENCODER_X264                    "x264"
#define SIMPLE_ENCODER_X264_LOWCPU             "x264_lowcpu"
#define SIMPLE_ENCODER_QSV                     "qsv"
#define SIMPLE_ENCODER_NVENC                   "nvenc"
#define SIMPLE_ENCODER_AMD                     "amd"

#define PREVIEW_EDGE_SIZE 10


class OBSBasicPreview;
class OBSQTDisplay;
class COBSBasic : public QObject
{
    Q_OBJECT
public:
    COBSBasic();
    ~COBSBasic();
    //==================================

    OBSScene      GetCurrentScene();
    inline OBSSource GetCurrentSceneSource()
    {
        OBSScene curScene = GetCurrentScene();
        return OBSSource(obs_scene_get_source(curScene));
    }

    obs_service_t *GetService();
    void SetService(obs_service_t *newService);
    static void RenderProgram(void *data, uint32_t cx, uint32_t cy);
    config_t *Config() const;


public slots:
    void StartStreaming();
    void StopStreaming();
    void ForceStopStreaming();

    void StreamDelayStarting(int sec);
    void StreamDelayStopping(int sec);

    void StreamingStart();
    void StreamStopping();
    void StreamingStop(int errorcode);

    void StartRecording();
    void StopRecording();

    void RecordingStart();
    void RecordStopping();
    void RecordingStop(int code);

    void StartReplayBuffer();
    void StopReplayBuffer();

    void ReplayBufferStart();
    void ReplayBufferStopping();
    void ReplayBufferStop(int code);

//    void SaveProjectDeferred();
//    void SaveProject();

private:
    bool OBSInit();
    bool InitBasicConfigDefaults();
    int  ResetVideo();
    bool ResetAudio();

    void ResetOutputs();

    void ResetAudioDevice(const char *sourceId, const char *deviceId,
            const char *deviceDesc, int channel);

    inline void OnActivate();
    inline void OnDeactivate();

    void GetFPSCommon(uint32_t &num, uint32_t &den) const;
        void GetFPSInteger(uint32_t &num, uint32_t &den) const;
        void GetFPSFraction(uint32_t &num, uint32_t &den) const;
        void GetFPSNanoseconds(uint32_t &num, uint32_t &den) const;
        void GetConfigFPS(uint32_t &num, uint32_t &den) const;

        void ResizePreview(uint32_t cx, uint32_t cy);
        void CreateProgramDisplay();
        void ResizeProgram(uint32_t cx, uint32_t cy);

        void          DrawBackdrop(float cx, float cy);

private:
    COBSGlobal m_obsGlobal;
    ConfigFile    globalConfig;
    ConfigFile    basicConfig;
    OBSScene m_scene;
    OBSService service;
    std::unique_ptr<CBasicOutputHandler> outputHandler;
    QPointer<OBSQTDisplay> program;
    //===

    int           previewX = 0,  previewY = 0;
    int           previewCX = 0, previewCY = 0;
    float         previewScale = 0.0f;

    int   programX = 0,  programY = 0;
    int   programCX = 0, programCY = 0;
    float programScale = 0.0f;

    bool profileMenuisEnabled;
    bool streamingStopping = false;
    bool recordingStopping = false;
    bool replayBufferStopping = false;

    OBSBasicPreview *preview;
};

//TODO
//api 未实现
#endif // COBSBASIC_H
