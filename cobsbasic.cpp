#include "cobsbasic.h"
#include <QApplication>
#include <QMessageBox>
#include <QScreen>
#include "platform.hpp"
#include "qt-wrappers.hpp"
#include "util/dstr.h"
#include "window-basic-preview.hpp"
#include "qt-display.hpp"
#include "display-helpers.hpp"

#ifdef _WIN32
#define IS_WIN32 1
#else
#define IS_WIN32 0
#endif
static inline int AttemptToResetVideo(struct obs_video_info *ovi)
{
    return obs_reset_video(ovi);
}

static inline enum obs_scale_type GetScaleType(ConfigFile &basicConfig)
{
    const char *scaleTypeStr = config_get_string(basicConfig,
            "Video", "ScaleType");

    if (astrcmpi(scaleTypeStr, "bilinear") == 0)
        return OBS_SCALE_BILINEAR;
    else if (astrcmpi(scaleTypeStr, "lanczos") == 0)
        return OBS_SCALE_LANCZOS;
    else
        return OBS_SCALE_BICUBIC;
}

static inline enum video_format GetVideoFormatFromName(const char *name)
{
    if (astrcmpi(name, "I420") == 0)
        return VIDEO_FORMAT_I420;
    else if (astrcmpi(name, "NV12") == 0)
        return VIDEO_FORMAT_NV12;
    else if (astrcmpi(name, "I444") == 0)
        return VIDEO_FORMAT_I444;
#if 0 //currently unsupported
    else if (astrcmpi(name, "YVYU") == 0)
        return VIDEO_FORMAT_YVYU;
    else if (astrcmpi(name, "YUY2") == 0)
        return VIDEO_FORMAT_YUY2;
    else if (astrcmpi(name, "UYVY") == 0)
        return VIDEO_FORMAT_UYVY;
#endif
    else
        return VIDEO_FORMAT_RGBA;
}

static const double scaled_vals[] =
{
    1.0,
    1.25,
    (1.0/0.75),
    1.5,
    (1.0/0.6),
    1.75,
    2.0,
    2.25,
    2.5,
    2.75,
    3.0,
    0.0
};

#ifdef _WIN32
static inline void UpdateProcessPriority(const char *priority)
{
    //const char *priority = config_get_string(App()->GlobalConfig(),
           // "General", "ProcessPriority");
    if (priority && strcmp(priority, "Normal") != 0)
        SetProcessPriority(priority);
}

static inline void ClearProcessPriority(const char *priority)
{
//    const char *priority = config_get_string(App()->GlobalConfig(),
//            "General", "ProcessPriority");
    if (priority && strcmp(priority, "Normal") != 0)
        SetProcessPriority("Normal");
}
#else
#define UpdateProcessPriority() do {} while(false)
#define ClearProcessPriority() do {} while(false)
#endif

COBSBasic::COBSBasic()
{
    preview = new OBSBasicPreview(NULL);
}

COBSBasic::~COBSBasic()
{
}

bool COBSBasic::OBSInit()
{
    profileMenuisEnabled = true;
    bool ok = false;
    //====global
    ok = m_obsGlobal.initGlobal();
    ok = ResetAudio();
    if (!ok)
        throw "Failed to initialize audio";

    ok = ResetVideo();
    return ok;
}

bool COBSBasic::InitBasicConfigDefaults()
{
    QList<QScreen*> screens = QGuiApplication::screens();

    if (!screens.size()) {
        OBSErrorBox(NULL, "There appears to be no monitors.  Er, this "
                          "technically shouldn't be possible.");
        return false;
    }

    QScreen *primaryScreen = QGuiApplication::primaryScreen();

    uint32_t cx = primaryScreen->size().width();
    uint32_t cy = primaryScreen->size().height();

    /* ----------------------------------------------------- */
    /* move over mixer values in advanced if older config */
    if (config_has_user_value(basicConfig, "AdvOut", "RecTrackIndex") &&
        !config_has_user_value(basicConfig, "AdvOut", "RecTracks")) {

        uint64_t track = config_get_uint(basicConfig, "AdvOut",
                "RecTrackIndex");
        track = 1ULL << (track - 1);
        config_set_uint(basicConfig, "AdvOut", "RecTracks", track);
        config_remove_value(basicConfig, "AdvOut", "RecTrackIndex");
        config_save_safe(basicConfig, "tmp", nullptr);
    }

    /* ----------------------------------------------------- */

    config_set_default_string(basicConfig, "Output", "Mode", "Simple");

    config_set_default_string(basicConfig, "SimpleOutput", "FilePath",
            GetDefaultVideoSavePath().c_str());
    config_set_default_string(basicConfig, "SimpleOutput", "RecFormat",
            "flv");
    config_set_default_uint  (basicConfig, "SimpleOutput", "VBitrate",
            2500);
    config_set_default_string(basicConfig, "SimpleOutput", "StreamEncoder",
            SIMPLE_ENCODER_X264);
    config_set_default_uint  (basicConfig, "SimpleOutput", "ABitrate", 160);
    config_set_default_bool  (basicConfig, "SimpleOutput", "UseAdvanced",
            false);
    config_set_default_bool  (basicConfig, "SimpleOutput", "EnforceBitrate",
            true);
    config_set_default_string(basicConfig, "SimpleOutput", "Preset",
            "veryfast");
    config_set_default_string(basicConfig, "SimpleOutput", "RecQuality",
            "Stream");
    config_set_default_string(basicConfig, "SimpleOutput", "RecEncoder",
            SIMPLE_ENCODER_X264);
    config_set_default_bool(basicConfig, "SimpleOutput", "RecRB", false);
    config_set_default_int(basicConfig, "SimpleOutput", "RecRBTime", 20);
    config_set_default_int(basicConfig, "SimpleOutput", "RecRBSize", 512);
    config_set_default_string(basicConfig, "SimpleOutput", "RecRBPrefix",
            "Replay");

    config_set_default_bool  (basicConfig, "AdvOut", "ApplyServiceSettings",
            true);
    config_set_default_bool  (basicConfig, "AdvOut", "UseRescale", false);
    config_set_default_uint  (basicConfig, "AdvOut", "TrackIndex", 1);
    config_set_default_string(basicConfig, "AdvOut", "Encoder", "obs_x264");

    config_set_default_string(basicConfig, "AdvOut", "RecType", "Standard");

    config_set_default_string(basicConfig, "AdvOut", "RecFilePath",
            GetDefaultVideoSavePath().c_str());
    config_set_default_string(basicConfig, "AdvOut", "RecFormat", "flv");
    config_set_default_bool  (basicConfig, "AdvOut", "RecUseRescale",
            false);
    config_set_default_uint  (basicConfig, "AdvOut", "RecTracks", (1<<0));
    config_set_default_string(basicConfig, "AdvOut", "RecEncoder",
            "none");

    config_set_default_bool  (basicConfig, "AdvOut", "FFOutputToFile",
            true);
    config_set_default_string(basicConfig, "AdvOut", "FFFilePath",
            GetDefaultVideoSavePath().c_str());
    config_set_default_string(basicConfig, "AdvOut", "FFExtension", "mp4");
    config_set_default_uint  (basicConfig, "AdvOut", "FFVBitrate", 2500);
    config_set_default_bool  (basicConfig, "AdvOut", "FFUseRescale",
            false);
    config_set_default_uint  (basicConfig, "AdvOut", "FFABitrate", 160);
    config_set_default_uint  (basicConfig, "AdvOut", "FFAudioTrack", 1);

    config_set_default_uint  (basicConfig, "AdvOut", "Track1Bitrate", 160);
    config_set_default_uint  (basicConfig, "AdvOut", "Track2Bitrate", 160);
    config_set_default_uint  (basicConfig, "AdvOut", "Track3Bitrate", 160);
    config_set_default_uint  (basicConfig, "AdvOut", "Track4Bitrate", 160);
    config_set_default_uint  (basicConfig, "AdvOut", "Track5Bitrate", 160);
    config_set_default_uint  (basicConfig, "AdvOut", "Track6Bitrate", 160);

    config_set_default_uint  (basicConfig, "Video", "BaseCX",   cx);
    config_set_default_uint  (basicConfig, "Video", "BaseCY",   cy);

    config_set_default_string(basicConfig, "Output", "FilenameFormatting",
            "%CCYY-%MM-%DD %hh-%mm-%ss");

    config_set_default_bool  (basicConfig, "Output", "DelayEnable", false);
    config_set_default_uint  (basicConfig, "Output", "DelaySec", 20);
    config_set_default_bool  (basicConfig, "Output", "DelayPreserve", true);

    config_set_default_bool  (basicConfig, "Output", "Reconnect", true);
    config_set_default_uint  (basicConfig, "Output", "RetryDelay", 10);
    config_set_default_uint  (basicConfig, "Output", "MaxRetries", 20);

    config_set_default_string(basicConfig, "Output", "BindIP", "default");

    int i = 0;
    uint32_t scale_cx = cx;
    uint32_t scale_cy = cy;

    /* use a default scaled resolution that has a pixel count no higher
     * than 1280x720 */
    while (((scale_cx * scale_cy) > (1280 * 720)) && scaled_vals[i] > 0.0) {
        double scale = scaled_vals[i++];
        scale_cx = uint32_t(double(cx) / scale);
        scale_cy = uint32_t(double(cy) / scale);
    }

    config_set_default_uint  (basicConfig, "Video", "OutputCX", scale_cx);
    config_set_default_uint  (basicConfig, "Video", "OutputCY", scale_cy);

    config_set_default_uint  (basicConfig, "Video", "FPSType", 0);
    config_set_default_string(basicConfig, "Video", "FPSCommon", "30");
    config_set_default_uint  (basicConfig, "Video", "FPSInt", 30);
    config_set_default_uint  (basicConfig, "Video", "FPSNum", 30);
    config_set_default_uint  (basicConfig, "Video", "FPSDen", 1);
    config_set_default_string(basicConfig, "Video", "ScaleType", "bicubic");
    config_set_default_string(basicConfig, "Video", "ColorFormat", "NV12");
    config_set_default_string(basicConfig, "Video", "ColorSpace", "601");
    config_set_default_string(basicConfig, "Video", "ColorRange",
            "Partial");

    config_set_default_string(basicConfig, "Audio", "MonitoringDeviceId",
            "default");
    config_set_default_string(basicConfig, "Audio", "MonitoringDeviceName",
            Str("Basic.Settings.Advanced.Audio.MonitoringDevice"
                ".Default"));
    config_set_default_uint  (basicConfig, "Audio", "SampleRate", 44100);
    config_set_default_string(basicConfig, "Audio", "ChannelSetup",
            "Stereo");

    return true;
}

int COBSBasic::ResetVideo()
{
    struct obs_video_info ovi;
    int ret;

    GetConfigFPS(ovi.fps_num, ovi.fps_den);

    const char *colorFormat = config_get_string(basicConfig, "Video",
            "ColorFormat");
    const char *colorSpace = config_get_string(basicConfig, "Video",
            "ColorSpace");
    const char *colorRange = config_get_string(basicConfig, "Video",
            "ColorRange");

//    ovi.graphics_module = App()->GetRenderModule();
    ovi.graphics_module = m_obsGlobal.GetRenderModule();
    ovi.base_width     = (uint32_t)config_get_uint(basicConfig,
            "Video", "BaseCX");
    ovi.base_height    = (uint32_t)config_get_uint(basicConfig,
            "Video", "BaseCY");
    ovi.output_width   = (uint32_t)config_get_uint(basicConfig,
            "Video", "OutputCX");
    ovi.output_height  = (uint32_t)config_get_uint(basicConfig,
            "Video", "OutputCY");
    ovi.output_format  = GetVideoFormatFromName(colorFormat);
    ovi.colorspace     = astrcmpi(colorSpace, "601") == 0 ?
        VIDEO_CS_601 : VIDEO_CS_709;
    ovi.range          = astrcmpi(colorRange, "Full") == 0 ?
        VIDEO_RANGE_FULL : VIDEO_RANGE_PARTIAL;
    ovi.adapter        = 0;
    ovi.gpu_conversion = true;
    ovi.scale_type     = GetScaleType(basicConfig);

    if (ovi.base_width == 0 || ovi.base_height == 0) {
        ovi.base_width = 1920;
        ovi.base_height = 1080;
        config_set_uint(basicConfig, "Video", "BaseCX", 1920);
        config_set_uint(basicConfig, "Video", "BaseCY", 1080);
    }

    if (ovi.output_width == 0 || ovi.output_height == 0) {
        ovi.output_width = ovi.base_width;
        ovi.output_height = ovi.base_height;
        config_set_uint(basicConfig, "Video", "OutputCX",
                ovi.base_width);
        config_set_uint(basicConfig, "Video", "OutputCY",
                ovi.base_height);
    }

    ret = AttemptToResetVideo(&ovi);
    if (IS_WIN32 && ret != OBS_VIDEO_SUCCESS) {
        /* Try OpenGL if DirectX fails on windows */
        if (astrcmpi(ovi.graphics_module, DL_OPENGL) != 0) {
            blog(LOG_WARNING, "Failed to initialize obs video (%d) "
                      "with graphics_module='%s', retrying "
                      "with graphics_module='%s'",
                      ret, ovi.graphics_module,
                      DL_OPENGL);
            ovi.graphics_module = DL_OPENGL;
            ret = AttemptToResetVideo(&ovi);
        }
    } else if (ret == OBS_VIDEO_SUCCESS) {
        ResizePreview(ovi.base_width, ovi.base_height);
        if (program)
            ResizeProgram(ovi.base_width, ovi.base_height);
    }

    return ret;
}

bool COBSBasic::ResetAudio()
{
    struct obs_audio_info ai;
    ai.samples_per_sec = config_get_uint(basicConfig, "Audio",
            "SampleRate");

    const char *channelSetupStr = config_get_string(basicConfig,
            "Audio", "ChannelSetup");

    if (strcmp(channelSetupStr, "Mono") == 0)
        ai.speakers = SPEAKERS_MONO;
    else
        ai.speakers = SPEAKERS_STEREO;

    return obs_reset_audio(&ai);
}

void COBSBasic::ResetOutputs()
{
    const char *mode = config_get_string(basicConfig, "Output", "Mode");
    bool advOut = astrcmpi(mode, "Advanced") == 0;

    if (!outputHandler || !outputHandler->Active()) {
        outputHandler.reset();
        outputHandler.reset(advOut ?
            CreateCAdvancedOutputHandler(this) :
            CreateCSimpleOutputHandler(this));

       // delete replayBufferButton;

        if (outputHandler->replayBuffer) {
           // replayBufferButton = new QPushButton(
                 //   QTStr("Basic.Main.StartReplayBuffer"),
                 //   this);
           // connect(replayBufferButton.data(),
                 //   &QPushButton::clicked,
                 //   this,
                 //   &OBSBasic::ReplayBufferClicked);

            //ui->buttonsVLayout->insertWidget(2, replayBufferButton);
        }

       // if (sysTrayReplayBuffer)
            //sysTrayReplayBuffer->setEnabled(
                 //   !!outputHandler->replayBuffer);
    } else {
        outputHandler->Update();
    }
}

void COBSBasic::ResetAudioDevice(const char *sourceId, const char *deviceId, const char *deviceDesc, int channel)
{
    obs_source_t *source;
    obs_data_t *settings;
    bool same = false;

    source = obs_get_output_source(channel);
    if (source) {
        settings = obs_source_get_settings(source);
        const char *curId = obs_data_get_string(settings, "device_id");

        same = (strcmp(curId, deviceId) == 0);

        obs_data_release(settings);
        obs_source_release(source);
    }

    if (!same)
        obs_set_output_source(channel, nullptr);

    if (!same && strcmp(deviceId, "disabled") != 0) {
        obs_data_t *settings = obs_data_create();
        obs_data_set_string(settings, "device_id", deviceId);
        source = obs_source_create(sourceId, deviceDesc, settings,
                nullptr);
        obs_data_release(settings);

        obs_set_output_source(channel, source);
        obs_source_release(source);
    }
}

void COBSBasic::OnActivate()
{
//    if (ui->profileMenu->isEnabled())
//    {
//        ui->profileMenu->setEnabled(false);
//        App()->IncrementSleepInhibition();
//        UpdateProcessPriority();

//        if (trayIcon)
//            trayIcon->setIcon(QIcon(":/res/images/tray_active.png"));
//    }
    if(profileMenuisEnabled)
    {
        profileMenuisEnabled = false;
        m_obsGlobal.IncrementSleepInhibition();
        const char *priority = config_get_string(App()->GlobalConfig(),
                "General", "ProcessPriority");
        UpdateProcessPriority(priority);
    }

}

void COBSBasic::OnDeactivate()
{
//    if (!outputHandler->Active() && !ui->profileMenu->isEnabled()) {
//        ui->profileMenu->setEnabled(true);
//        App()->DecrementSleepInhibition();
//        ClearProcessPriority();

//        if (trayIcon)
//            trayIcon->setIcon(QIcon(":/res/images/obs.png"));
//    }
    if (!outputHandler->Active() && !profileMenuisEnabled) {
        profileMenuisEnabled = true;
        m_obsGlobal.DecrementSleepInhibition();
        const char *priority = config_get_string(App()->GlobalConfig(),
                "General", "ProcessPriority");
        ClearProcessPriority(priority);
    }
}

void COBSBasic::GetFPSCommon(uint32_t &num, uint32_t &den) const
{
    const char *val = config_get_string(basicConfig, "Video", "FPSCommon");

    if (strcmp(val, "10") == 0) {
        num = 10;
        den = 1;
    } else if (strcmp(val, "20") == 0) {
        num = 20;
        den = 1;
    } else if (strcmp(val, "24 NTSC") == 0) {
        num = 24000;
        den = 1001;
    } else if (strcmp(val, "25") == 0) {
        num = 25;
        den = 1;
    } else if (strcmp(val, "29.97") == 0) {
        num = 30000;
        den = 1001;
    } else if (strcmp(val, "48") == 0) {
        num = 48;
        den = 1;
    } else if (strcmp(val, "59.94") == 0) {
        num = 60000;
        den = 1001;
    } else if (strcmp(val, "60") == 0) {
        num = 60;
        den = 1;
    } else {
        num = 30;
        den = 1;
    }
}

void COBSBasic::GetFPSInteger(uint32_t &num, uint32_t &den) const
{
    num = (uint32_t)config_get_uint(basicConfig, "Video", "FPSInt");
    den = 1;
}

void COBSBasic::GetFPSFraction(uint32_t &num, uint32_t &den) const
{
    num = (uint32_t)config_get_uint(basicConfig, "Video", "FPSNum");
    den = (uint32_t)config_get_uint(basicConfig, "Video", "FPSDen");
}

void COBSBasic::GetFPSNanoseconds(uint32_t &num, uint32_t &den) const
{
    num = 1000000000;
    den = (uint32_t)config_get_uint(basicConfig, "Video", "FPSNS");
}

void COBSBasic::GetConfigFPS(uint32_t &num, uint32_t &den) const
{
    uint32_t type = config_get_uint(basicConfig, "Video", "FPSType");

    if (type == 1) //"Integer"
        GetFPSInteger(num, den);
    else if (type == 2) //"Fraction"
        GetFPSFraction(num, den);
    else if (false) //"Nanoseconds", currently not implemented
        GetFPSNanoseconds(num, den);
    else
        GetFPSCommon(num, den);
}

void COBSBasic::ResizePreview(uint32_t cx, uint32_t cy)
{
    QSize  targetSize;
        ScalingMode scalingMode;
        obs_video_info ovi;

        /* resize preview panel to fix to the top section of the window */
        targetSize = GetPixelSize(preview);

        scalingMode = preview->GetScalingMode();
        obs_get_video_info(&ovi);

        if (scalingMode == ScalingMode::Canvas) {
            previewScale = 1.0f;
            GetCenterPosFromFixedScale(int(cx), int(cy),
                    targetSize.width() - PREVIEW_EDGE_SIZE * 2,
                    targetSize.height() - PREVIEW_EDGE_SIZE * 2,
                    previewX, previewY, previewScale);
            previewX += preview->ScrollX();
            previewY += preview->ScrollY();

        } else if (scalingMode == ScalingMode::Output) {
            previewScale = float(ovi.output_width) / float(ovi.base_width);
            GetCenterPosFromFixedScale(int(cx), int(cy),
                    targetSize.width() - PREVIEW_EDGE_SIZE * 2,
                    targetSize.height() - PREVIEW_EDGE_SIZE * 2,
                    previewX, previewY, previewScale);
            previewX += preview->ScrollX();
            previewY += preview->ScrollY();

        } else {
            GetScaleAndCenterPos(int(cx), int(cy),
                    targetSize.width() - PREVIEW_EDGE_SIZE * 2,
                    targetSize.height() - PREVIEW_EDGE_SIZE * 2,
                    previewX, previewY, previewScale);
        }

        previewX += float(PREVIEW_EDGE_SIZE);
        previewY += float(PREVIEW_EDGE_SIZE);
}

OBSScene COBSBasic::GetCurrentScene()
{
    return m_scene;
}

obs_service_t *COBSBasic::GetService()
{
    if (!service) {
        service = obs_service_create("rtmp_common", NULL, NULL,
                nullptr);
        obs_service_release(service);
    }
    return service;
}

void COBSBasic::SetService(obs_service_t *newService)
{
    if (newService)
        service = newService;
}

void COBSBasic::RenderProgram(void *data, uint32_t cx, uint32_t cy)
{
    COBSBasic *window = static_cast<COBSBasic*>(data);
        obs_video_info ovi;

        obs_get_video_info(&ovi);

        window->programCX = int(window->programScale * float(ovi.base_width));
        window->programCY = int(window->programScale * float(ovi.base_height));

        gs_viewport_push();
        gs_projection_push();

        /* --------------------------------------- */

        gs_ortho(0.0f, float(ovi.base_width), 0.0f, float(ovi.base_height),
                -100.0f, 100.0f);
        gs_set_viewport(window->programX, window->programY,
                window->programCX, window->programCY);

        window->DrawBackdrop(float(ovi.base_width), float(ovi.base_height));

        obs_render_main_view();
        gs_load_vertexbuffer(nullptr);

        /* --------------------------------------- */

        gs_projection_pop();
        gs_viewport_pop();

        UNUSED_PARAMETER(cx);
        UNUSED_PARAMETER(cy);
}

void COBSBasic::StartStreaming()
{
    if (outputHandler->StreamingActive())
        return;

 //   if (api)
      //  api->on_event(OBS_FRONTEND_EVENT_STREAMING_STARTING);

   // SaveProject();

   // ui->streamButton->setEnabled(false);
   // ui->streamButton->setText(QTStr("Basic.Main.Connecting"));

//    if (sysTrayStream) {
//        sysTrayStream->setEnabled(false);
//        sysTrayStream->setText(ui->streamButton->text());
//    }

    if (!outputHandler->StartStreaming(service)) {
       // ui->streamButton->setText(QTStr("Basic.Main.StartStreaming"));
      //  ui->streamButton->setEnabled(true);

//        if (sysTrayStream) {
//            sysTrayStream->setText(ui->streamButton->text());
//            sysTrayStream->setEnabled(true);
//        }

        QMessageBox::critical(NULL,
                QTStr("Output.StartStreamFailed"),
                QTStr("Output.StartFailedGeneric"));
        return;
    }

    bool recordWhenStreaming = config_get_bool(GetGlobalConfig(),
            "BasicWindow", "RecordWhenStreaming");
    if (recordWhenStreaming)
        StartRecording();

    bool replayBufferWhileStreaming = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "ReplayBufferWhileStreaming");
    if (replayBufferWhileStreaming)
        StartReplayBuffer();
}

void COBSBasic::StopStreaming()
{
//    SaveProject();

    if (outputHandler->StreamingActive())
        outputHandler->StopStreaming(streamingStopping);

    OnDeactivate();

    bool recordWhenStreaming = config_get_bool(GetGlobalConfig(),
            "BasicWindow", "RecordWhenStreaming");
    bool keepRecordingWhenStreamStops = config_get_bool(GetGlobalConfig(),
            "BasicWindow", "KeepRecordingWhenStreamStops");
    if (recordWhenStreaming && !keepRecordingWhenStreamStops)
        StopRecording();

    bool replayBufferWhileStreaming = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "ReplayBufferWhileStreaming");
    bool keepReplayBufferStreamStops = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "KeepReplayBufferStreamStops");
    if (replayBufferWhileStreaming && !keepReplayBufferStreamStops)
        StopReplayBuffer();
}

void COBSBasic::ForceStopStreaming()
{
  //  SaveProject();

    if (outputHandler->StreamingActive())
        outputHandler->StopStreaming(true);

    OnDeactivate();

    bool recordWhenStreaming = config_get_bool(GetGlobalConfig(),
            "BasicWindow", "RecordWhenStreaming");
    bool keepRecordingWhenStreamStops = config_get_bool(GetGlobalConfig(),
            "BasicWindow", "KeepRecordingWhenStreamStops");
    if (recordWhenStreaming && !keepRecordingWhenStreamStops)
        StopRecording();

    bool replayBufferWhileStreaming = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "ReplayBufferWhileStreaming");
    bool keepReplayBufferStreamStops = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "KeepReplayBufferStreamStops");
    if (replayBufferWhileStreaming && !keepReplayBufferStreamStops)
        StopReplayBuffer();
}

void COBSBasic::StreamDelayStarting(int sec)
{
//    ui->streamButton->setText(QTStr("Basic.Main.StopStreaming"));
//    ui->streamButton->setEnabled(true);

//    if (sysTrayStream) {
//        sysTrayStream->setText(ui->streamButton->text());
//        sysTrayStream->setEnabled(true);
//    }

//    if (!startStreamMenu.isNull())
//        startStreamMenu->deleteLater();

//    startStreamMenu = new QMenu();
//    startStreamMenu->addAction(QTStr("Basic.Main.StopStreaming"),
//            this, SLOT(StopStreaming()));
//    startStreamMenu->addAction(QTStr("Basic.Main.ForceStopStreaming"),
//            this, SLOT(ForceStopStreaming()));
//    ui->streamButton->setMenu(startStreamMenu);

//    ui->statusbar->StreamDelayStarting(sec);

    OnActivate();
}

void COBSBasic::StreamDelayStopping(int sec)
{
 //   ui->streamButton->setText(QTStr("Basic.Main.StartStreaming"));
  //  ui->streamButton->setEnabled(true);

//    if (sysTrayStream) {
//        sysTrayStream->setText(ui->streamButton->text());
//        sysTrayStream->setEnabled(true);
//    }

//    if (!startStreamMenu.isNull())
//        startStreamMenu->deleteLater();

//    startStreamMenu = new QMenu();
//    startStreamMenu->addAction(QTStr("Basic.Main.StartStreaming"),
//            this, SLOT(StartStreaming()));
//    startStreamMenu->addAction(QTStr("Basic.Main.ForceStopStreaming"),
//            this, SLOT(ForceStopStreaming()));
//    ui->streamButton->setMenu(startStreamMenu);

    //    ui->statusbar->StreamDelayStopping(sec);
}

void COBSBasic::StreamingStart()
{
   // ui->streamButton->setText(QTStr("Basic.Main.StopStreaming"));
  //  ui->streamButton->setEnabled(true);
   // ui->statusbar->StreamStarted(outputHandler->streamOutput);

  //  if (sysTrayStream) {
   //     sysTrayStream->setText(ui->streamButton->text());
   //     sysTrayStream->setEnabled(true);
   // }

    //if (api)
       // api->on_event(OBS_FRONTEND_EVENT_STREAMING_STARTED);

    OnActivate();

  //  blog(LOG_INFO, STREAMING_START);
}


void COBSBasic::StreamStopping()
{
//    ui->streamButton->setText(QTStr("Basic.Main.StoppingStreaming"));

//    if (sysTrayStream)
//        sysTrayStream->setText(ui->streamButton->text());

    streamingStopping = true;
  //  if (api)
        //api->on_event(OBS_FRONTEND_EVENT_STREAMING_STOPPING);
}

void COBSBasic::StreamingStop(int code)
{
    const char *errorMessage;

    switch (code) {
    case OBS_OUTPUT_BAD_PATH:
        errorMessage = Str("Output.ConnectFail.BadPath");
        break;

    case OBS_OUTPUT_CONNECT_FAILED:
        errorMessage = Str("Output.ConnectFail.ConnectFailed");
        break;

    case OBS_OUTPUT_INVALID_STREAM:
        errorMessage = Str("Output.ConnectFail.InvalidStream");
        break;

    default:
    case OBS_OUTPUT_ERROR:
        errorMessage = Str("Output.ConnectFail.Error");
        break;

    case OBS_OUTPUT_DISCONNECTED:
        /* doesn't happen if output is set to reconnect.  note that
         * reconnects are handled in the output, not in the UI */
        errorMessage = Str("Output.ConnectFail.Disconnected");
    }

   // ui->statusbar->StreamStopped();

   // ui->streamButton->setText(QTStr("Basic.Main.StartStreaming"));
  //  ui->streamButton->setEnabled(true);

//    if (sysTrayStream) {
//        sysTrayStream->setText(ui->streamButton->text());
//        sysTrayStream->setEnabled(true);
//    }

    streamingStopping = false;
   // if (api)
       // api->on_event(OBS_FRONTEND_EVENT_STREAMING_STOPPED);

    OnDeactivate();

  //  blog(LOG_INFO, STREAMING_STOP);

//    if (code != OBS_OUTPUT_SUCCESS && isVisible()) {
//        QMessageBox::information(this,
//                QTStr("Output.ConnectFail.Title"),
//                QT_UTF8(errorMessage));
//    } else if (code != OBS_OUTPUT_SUCCESS && !isVisible()) {
//        SysTrayNotify(QT_UTF8(errorMessage), QSystemTrayIcon::Warning);
//    }

//    if (!startStreamMenu.isNull()) {
//        ui->streamButton->setMenu(nullptr);
//        startStreamMenu->deleteLater();
//        startStreamMenu = nullptr;
    //    }
}

void COBSBasic::StartRecording()
{
    if (outputHandler->RecordingActive())
        return;

   // if (api)
     //   api->on_event(OBS_FRONTEND_EVENT_RECORDING_STARTING);

   // SaveProject();
    outputHandler->StartRecording();
}

void COBSBasic::StopRecording()
{
  //  SaveProject();

    if (outputHandler->RecordingActive())
        outputHandler->StopRecording(recordingStopping);

    OnDeactivate();
}

void COBSBasic::RecordingStart()
{
   // ui->statusbar->RecordingStarted(outputHandler->fileOutput);
   // ui->recordButton->setText(QTStr("Basic.Main.StopRecording"));

   // if (sysTrayRecord)
     //   sysTrayRecord->setText(ui->recordButton->text());

    recordingStopping = false;
  //  if (api)
      //  api->on_event(OBS_FRONTEND_EVENT_RECORDING_STARTED);

    OnActivate();

    // blog(LOG_INFO, RECORDING_START);
}

void COBSBasic::RecordStopping()
{

//    ui->recordButton->setText(QTStr("Basic.Main.StoppingRecording"));

//	if (sysTrayRecord)
//		sysTrayRecord->setText(ui->recordButton->text());

    recordingStopping = true;
    //if (api)
    //	api->on_event(OBS_FRONTEND_EVENT_RECORDING_STOPPING);
}

void COBSBasic::RecordingStop(int code)
{
//    ui->statusbar->RecordingStopped();
//    ui->recordButton->setText(QTStr("Basic.Main.StartRecording"));

//    if (sysTrayRecord)
//        sysTrayRecord->setText(ui->recordButton->text());

//    blog(LOG_INFO, RECORDING_STOP);

//    if (code == OBS_OUTPUT_UNSUPPORTED && isVisible()) {
//        QMessageBox::information(this,
//                QTStr("Output.RecordFail.Title"),
//                QTStr("Output.RecordFail.Unsupported"));

//    } else if (code == OBS_OUTPUT_NO_SPACE && isVisible()) {
//        QMessageBox::information(this,
//                QTStr("Output.RecordNoSpace.Title"),
//                QTStr("Output.RecordNoSpace.Msg"));

//    } else if (code != OBS_OUTPUT_SUCCESS && isVisible()) {
//        QMessageBox::information(this,
//                QTStr("Output.RecordError.Title"),
//                QTStr("Output.RecordError.Msg"));

//    } else if (code == OBS_OUTPUT_UNSUPPORTED && !isVisible()) {
//        SysTrayNotify(QTStr("Output.RecordFail.Unsupported"),
//            QSystemTrayIcon::Warning);

//    } else if (code == OBS_OUTPUT_NO_SPACE && !isVisible()) {
//        SysTrayNotify(QTStr("Output.RecordNoSpace.Msg"),
//            QSystemTrayIcon::Warning);

//    } else if (code != OBS_OUTPUT_SUCCESS && !isVisible()) {
//        SysTrayNotify(QTStr("Output.RecordError.Msg"),
//            QSystemTrayIcon::Warning);
//    }

//    if (api)
//        api->on_event(OBS_FRONTEND_EVENT_RECORDING_STOPPED);

    OnDeactivate();
}

void COBSBasic::StartReplayBuffer()
{
    if (!outputHandler || !outputHandler->replayBuffer)
        return;
    if (outputHandler->ReplayBufferActive())
        return;

    obs_output_t *output = outputHandler->replayBuffer;
    obs_data_t *hotkeys = obs_hotkeys_save_output(output);
    obs_data_array_t *bindings = obs_data_get_array(hotkeys,
            "ReplayBuffer.Save");
    size_t count = obs_data_array_count(bindings);
    obs_data_array_release(bindings);
    obs_data_release(hotkeys);

//    if (!count) {
//        QMessageBox::information(this,
//                RP_NO_HOTKEY_TITLE,
//                RP_NO_HOTKEY_TEXT);
//        return;
//    }

    //if (api)
     //   api->on_event(OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTING);

    //SaveProject();
    outputHandler->StartReplayBuffer();
}

void COBSBasic::StopReplayBuffer()
{
    if (!outputHandler || !outputHandler->replayBuffer)
        return;

  //  SaveProject();

    if (outputHandler->ReplayBufferActive())
        outputHandler->StopReplayBuffer(replayBufferStopping);

    OnDeactivate();
}

void COBSBasic::ReplayBufferStart()
{
    if (!outputHandler || !outputHandler->replayBuffer)
        return;

   // replayBufferButton->setText(QTStr("Basic.Main.StopReplayBuffer"));

   // if (sysTrayReplayBuffer)
     //   sysTrayReplayBuffer->setText(replayBufferButton->text());

    replayBufferStopping = false;
  //  if (api)
       // api->on_event(OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTED);

    OnActivate();

    // blog(LOG_INFO, REPLAY_BUFFER_START);
}

void COBSBasic::ReplayBufferStopping()
{
    if (!outputHandler || !outputHandler->replayBuffer)
        return;

 //   replayBufferButton->setText(QTStr("Basic.Main.StoppingReplayBuffer"));

   // if (sysTrayReplayBuffer)
   //     sysTrayReplayBuffer->setText(replayBufferButton->text());

    replayBufferStopping = true;
 //   if (api)
    //  api->on_event(OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPING);
}

void COBSBasic::ReplayBufferStop(int code)
{
    if (!outputHandler || !outputHandler->replayBuffer)
        return;

  //  replayBufferButton->setText(QTStr("Basic.Main.StartReplayBuffer"));

  //  if (sysTrayReplayBuffer)
     //   sysTrayReplayBuffer->setText(replayBufferButton->text());

  //  blog(LOG_INFO, REPLAY_BUFFER_STOP);

//    if (code == OBS_OUTPUT_UNSUPPORTED && isVisible()) {
//        QMessageBox::information(this,
//                QTStr("Output.RecordFail.Title"),
//                QTStr("Output.RecordFail.Unsupported"));

//    } else if (code == OBS_OUTPUT_NO_SPACE && isVisible()) {
//        QMessageBox::information(this,
//                QTStr("Output.RecordNoSpace.Title"),
//                QTStr("Output.RecordNoSpace.Msg"));

//    } else if (code != OBS_OUTPUT_SUCCESS && isVisible()) {
//        QMessageBox::information(this,
//                QTStr("Output.RecordError.Title"),
//                QTStr("Output.RecordError.Msg"));

//    } else if (code == OBS_OUTPUT_UNSUPPORTED && !isVisible()) {
//        SysTrayNotify(QTStr("Output.RecordFail.Unsupported"),
//            QSystemTrayIcon::Warning);

//    } else if (code == OBS_OUTPUT_NO_SPACE && !isVisible()) {
//        SysTrayNotify(QTStr("Output.RecordNoSpace.Msg"),
//            QSystemTrayIcon::Warning);

//    } else if (code != OBS_OUTPUT_SUCCESS && !isVisible()) {
//        SysTrayNotify(QTStr("Output.RecordError.Msg"),
//            QSystemTrayIcon::Warning);
//    }

 //   if (api)
 //       api->on_event(OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPED);

    OnDeactivate();
}

void COBSBasic::CreateProgramDisplay()
{
    program = new OBSQTDisplay();

    auto displayResize = [this]() {
        struct obs_video_info ovi;

        if (obs_get_video_info(&ovi))
            ResizeProgram(ovi.base_width, ovi.base_height);
    };

    connect(program.data(), &OBSQTDisplay::DisplayResized,
            displayResize);

    auto addDisplay = [this] (OBSQTDisplay *window)
    {
        obs_display_add_draw_callback(window->GetDisplay(),
                COBSBasic::RenderProgram, this);

        struct obs_video_info ovi;
        if (obs_get_video_info(&ovi))
            ResizeProgram(ovi.base_width, ovi.base_height);
    };

    connect(program.data(), &OBSQTDisplay::DisplayCreated, addDisplay);

    program->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Expanding);
}

void COBSBasic::ResizeProgram(uint32_t cx, uint32_t cy)
{
    QSize targetSize;

    /* resize program panel to fix to the top section of the window */
    targetSize = GetPixelSize(program);
    GetScaleAndCenterPos(int(cx), int(cy),
            targetSize.width()  - PREVIEW_EDGE_SIZE * 2,
            targetSize.height() - PREVIEW_EDGE_SIZE * 2,
            programX, programY, programScale);

    programX += float(PREVIEW_EDGE_SIZE);
    programY += float(PREVIEW_EDGE_SIZE);
}

void COBSBasic::DrawBackdrop(float cx, float cy)
{
    //TODO
}

config_t *COBSBasic::Config() const
{
    return basicConfig;
}
