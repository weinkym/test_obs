#include "cobsglobal.h"
#include "qt-wrappers.hpp"
#include <QApplication>
#include <QScreen>
#include "util/dstr.h"
#include "platform.hpp"

#include <fstream>

#include <curl/curl.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

#include <iostream>

using namespace std;

static log_handler_t c_def_log_handler;

static string currentLogFile;
static string lastLogFile;

static bool portable_mode = false;
static bool log_verbose = false;
static bool unfiltered_log = false;
//bool opt_start_streaming = false;
//bool opt_start_recording = false;
//bool opt_studio_mode = false;
//bool opt_start_replaybuffer = false;
//bool opt_minimize_tray = false;
string c_opt_starting_collection;
string c_opt_starting_profile;
//string opt_starting_scene;

// AMD PowerXpress High Performance Flags
#ifdef _MSC_VER
extern "C" __declspec(dllexport) int C_AmdPowerXpressRequestHighPerformance = 1;
#endif

//QObject *CreateShortcutFilter()
//{
//    return new OBSEventFilter([](QObject *obj, QEvent *event)
//    {
//        auto mouse_event = [](QMouseEvent &event)
//        {
//            obs_key_combination_t hotkey = {0, OBS_KEY_NONE};
//            bool pressed = event.type() == QEvent::MouseButtonPress;

//            switch (event.button()) {
//            case Qt::NoButton:
//            case Qt::LeftButton:
//            case Qt::RightButton:
//            case Qt::AllButtons:
//            case Qt::MouseButtonMask:
//                return false;

//            case Qt::MidButton:
//                hotkey.key = OBS_KEY_MOUSE3;
//                break;

//#define MAP_BUTTON(i, j) case Qt::ExtraButton ## i: \
//        hotkey.key = OBS_KEY_MOUSE ## j; break;
//            MAP_BUTTON( 1,  4);
//            MAP_BUTTON( 2,  5);
//            MAP_BUTTON( 3,  6);
//            MAP_BUTTON( 4,  7);
//            MAP_BUTTON( 5,  8);
//            MAP_BUTTON( 6,  9);
//            MAP_BUTTON( 7, 10);
//            MAP_BUTTON( 8, 11);
//            MAP_BUTTON( 9, 12);
//            MAP_BUTTON(10, 13);
//            MAP_BUTTON(11, 14);
//            MAP_BUTTON(12, 15);
//            MAP_BUTTON(13, 16);
//            MAP_BUTTON(14, 17);
//            MAP_BUTTON(15, 18);
//            MAP_BUTTON(16, 19);
//            MAP_BUTTON(17, 20);
//            MAP_BUTTON(18, 21);
//            MAP_BUTTON(19, 22);
//            MAP_BUTTON(20, 23);
//            MAP_BUTTON(21, 24);
//            MAP_BUTTON(22, 25);
//            MAP_BUTTON(23, 26);
//            MAP_BUTTON(24, 27);
//#undef MAP_BUTTON
//            }

//            hotkey.modifiers = TranslateQtKeyboardEventModifiers(
//                            event.modifiers());

//            obs_hotkey_inject_event(hotkey, pressed);
//            return true;
//        };

//        auto key_event = [&](QKeyEvent *event)
//        {
//            QDialog *dialog = qobject_cast<QDialog*>(obj);

//            obs_key_combination_t hotkey = {0, OBS_KEY_NONE};
//            bool pressed = event->type() == QEvent::KeyPress;

//            switch (event->key()) {
//            case Qt::Key_Shift:
//            case Qt::Key_Control:
//            case Qt::Key_Alt:
//            case Qt::Key_Meta:
//                break;

//#ifdef __APPLE__
//            case Qt::Key_CapsLock:
//                // kVK_CapsLock == 57
//                hotkey.key = obs_key_from_virtual_key(57);
//                pressed = true;
//                break;
//#endif

//            case Qt::Key_Enter:
//            case Qt::Key_Escape:
//            case Qt::Key_Return:
//                if (dialog && pressed)
//                    return false;
//            default:
//                hotkey.key = obs_key_from_virtual_key(
//                    event->nativeVirtualKey());
//            }

//            hotkey.modifiers = TranslateQtKeyboardEventModifiers(
//                            event->modifiers());

//            obs_hotkey_inject_event(hotkey, pressed);
//            return true;
//        };

//        switch (event->type()) {
//        case QEvent::MouseButtonPress:
//        case QEvent::MouseButtonRelease:
//            return mouse_event(*static_cast<QMouseEvent*>(event));

//        /*case QEvent::MouseButtonDblClick:
//        case QEvent::Wheel:*/
//        case QEvent::KeyPress:
//        case QEvent::KeyRelease:
//            return key_event(static_cast<QKeyEvent*>(event));

//        default:
//            return false;
//        }
//    });
//}

//string CurrentTimeString()
//{
//    using namespace std::chrono;

//    struct tm  tstruct;
//    char       buf[80];

//    auto tp = system_clock::now();
//    auto now = system_clock::to_time_t(tp);
//    tstruct = *localtime(&now);

//    size_t written = strftime(buf, sizeof(buf), "%X", &tstruct);
//    if (ratio_less<system_clock::period, seconds::period>::value &&
//            written && (sizeof(buf) - written) > 5) {
//        auto tp_secs =
//            time_point_cast<seconds>(tp);
//        auto millis  =
//            duration_cast<milliseconds>(tp - tp_secs).count();

//        snprintf(buf + written, sizeof(buf) - written, ".%03u",
//                static_cast<unsigned>(millis));
//    }

//    return buf;
//}

//string CurrentDateTimeString()
//{
//    time_t     now = time(0);
//    struct tm  tstruct;
//    char       buf[80];
//    tstruct = *localtime(&now);
//    strftime(buf, sizeof(buf), "%Y-%m-%d, %X", &tstruct);
//    return buf;
//}

static inline void LogString(fstream &logFile, const char *timeString,
        char *str)
{
    logFile << timeString << str << endl;
}

//static inline void LogStringChunk(fstream &logFile, char *str)
//{
//    char *nextLine = str;
//    string timeString = CurrentTimeString();
//    timeString += ": ";

//    while (*nextLine) {
//        char *nextLine = strchr(str, '\n');
//        if (!nextLine)
//            break;

//        if (nextLine != str && nextLine[-1] == '\r') {
//            nextLine[-1] = 0;
//        } else {
//            nextLine[0] = 0;
//        }

//        LogString(logFile, timeString.c_str(), str);
//        nextLine++;
//        str = nextLine;
//    }

//    LogString(logFile, timeString.c_str(), str);
//}

#define MAX_REPEATED_LINES 30
#define MAX_CHAR_VARIATION (255 * 3)

static inline int sum_chars(const char *str)
{
    int val = 0;
    for (; *str != 0; str++)
        val += *str;

    return val;
}

//static inline bool too_many_repeated_entries(fstream &logFile, const char *msg,
//        const char *output_str)
//{
//    static mutex log_mutex;
//    static const char *last_msg_ptr = nullptr;
//    static int last_char_sum = 0;
//    static char cmp_str[4096];
//    static int rep_count = 0;

//    int new_sum = sum_chars(output_str);

//    lock_guard<mutex> guard(log_mutex);

//    if (unfiltered_log) {
//        return false;
//    }

//    if (last_msg_ptr == msg) {
//        int diff = std::abs(new_sum - last_char_sum);
//        if (diff < MAX_CHAR_VARIATION) {
//            return (rep_count++ >= MAX_REPEATED_LINES);
//        }
//    }

//    if (rep_count > MAX_REPEATED_LINES) {
//        logFile << CurrentTimeString() <<
//            ": Last log entry repeated for " <<
//            to_string(rep_count - MAX_REPEATED_LINES) <<
//            " more lines" << endl;
//    }

//    last_msg_ptr = msg;
//    strcpy(cmp_str, output_str);
//    last_char_sum = new_sum;
//    rep_count = 0;

//    return false;
//}

//static void do_log(int log_level, const char *msg, va_list args, void *param)
//{
//    fstream &logFile = *static_cast<fstream*>(param);
//    char str[4096];

//#ifndef _WIN32
//    va_list args2;
//    va_copy(args2, args);
//#endif

//    vsnprintf(str, 4095, msg, args);

//#ifdef _WIN32
//    if (IsDebuggerPresent()) {
//        int wNum = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
//        if (wNum > 1) {
//            static wstring wide_buf;
//            static mutex wide_mutex;

//            lock_guard<mutex> lock(wide_mutex);
//            wide_buf.reserve(wNum + 1);
//            wide_buf.resize(wNum - 1);
//            MultiByteToWideChar(CP_UTF8, 0, str, -1, &wide_buf[0],
//                    wNum);
//            wide_buf.push_back('\n');

//            OutputDebugStringW(wide_buf.c_str());
//        }
//    }
//#else
//    c_def_log_handler(log_level, msg, args2, nullptr);
//#endif

//    if (too_many_repeated_entries(logFile, msg, str))
//        return;

//    if (log_level <= LOG_INFO || log_verbose)
//        LogStringChunk(logFile, str);

//#if defined(_WIN32) && defined(OBS_DEBUGBREAK_ON_ERROR)
//    if (log_level <= LOG_ERROR && IsDebuggerPresent())
//        __debugbreak();
//#endif
//}

//#define DEFAULT_LANG "en-US"



static bool do_mkdir(const char *path)
{
    if (os_mkdirs(path) == MKDIR_ERROR) {
        OBSErrorBox(NULL, "Failed to create directory %s", path);
        return false;
    }

    return true;
}

#ifdef __APPLE__
#define BASE_PATH ".."
#else
#define BASE_PATH "../.."
#endif

#define CONFIG_PATH BASE_PATH "/config"

#ifndef OBS_UNIX_STRUCTURE
#define OBS_UNIX_STRUCTURE 0
#endif

int c_GetConfigPath(char *path, size_t size, const char *name)
{
    if (!OBS_UNIX_STRUCTURE && portable_mode) {
        if (name && *name) {
            return snprintf(path, size, CONFIG_PATH "/%s", name);
        } else {
            return snprintf(path, size, CONFIG_PATH);
        }
    } else {
        return os_get_config_path(path, size, name);
    }
}

static bool MakeUserDirs()
{
    char path[512];

    if (c_GetConfigPath(path, sizeof(path), "obs-studio/basic") <= 0)
        return false;
    if (!do_mkdir(path))
        return false;

    if (c_GetConfigPath(path, sizeof(path), "obs-studio/logs") <= 0)
        return false;
    if (!do_mkdir(path))
        return false;

    if (c_GetConfigPath(path, sizeof(path), "obs-studio/profiler_data") <= 0)
        return false;
    if (!do_mkdir(path))
        return false;

#ifdef _WIN32
    if (c_GetConfigPath(path, sizeof(path), "obs-studio/crashes") <= 0)
        return false;
    if (!do_mkdir(path))
        return false;
#endif
    if (c_GetConfigPath(path, sizeof(path), "obs-studio/plugin_config") <= 0)
        return false;
    if (!do_mkdir(path))
        return false;

    return true;
}

static bool MakeUserProfileDirs()
{
    char path[512];

    if (c_GetConfigPath(path, sizeof(path), "obs-studio/basic/profiles") <= 0)
        return false;
    if (!do_mkdir(path))
        return false;

    if (c_GetConfigPath(path, sizeof(path), "obs-studio/basic/scenes") <= 0)
        return false;
    if (!do_mkdir(path))
        return false;

    return true;
}

static string GetProfileDirFromName(const char *name)
{
    string outputPath;
    os_glob_t *glob;
    char path[512];

    if (c_GetConfigPath(path, sizeof(path), "obs-studio/basic/profiles") <= 0)
        return outputPath;

    strcat(path, "/*");

    if (os_glob(path, 0, &glob) != 0)
        return outputPath;

    for (size_t i = 0; i < glob->gl_pathc; i++) {
        struct os_globent ent = glob->gl_pathv[i];
        if (!ent.directory)
            continue;

        strcpy(path, ent.path);
        strcat(path, "/basic.ini");

        ConfigFile config;
        if (config.Open(path, CONFIG_OPEN_EXISTING) != 0)
            continue;

        const char *curName = config_get_string(config, "General",
                "Name");
        if (astrcmpi(curName, name) == 0) {
            outputPath = ent.path;
            break;
        }
    }

    os_globfree(glob);

    if (!outputPath.empty()) {
        replace(outputPath.begin(), outputPath.end(), '\\', '/');
        const char *start = strrchr(outputPath.c_str(), '/');
        if (start)
            outputPath.erase(0, start - outputPath.c_str() + 1);
    }

    return outputPath;
}

static string GetSceneCollectionFileFromName(const char *name)
{
    string outputPath;
    os_glob_t *glob;
    char path[512];

    if (c_GetConfigPath(path, sizeof(path), "obs-studio/basic/scenes") <= 0)
        return outputPath;

    strcat(path, "/*.json");

    if (os_glob(path, 0, &glob) != 0)
        return outputPath;

    for (size_t i = 0; i < glob->gl_pathc; i++) {
        struct os_globent ent = glob->gl_pathv[i];
        if (ent.directory)
            continue;

        obs_data_t *data =
            obs_data_create_from_json_file_safe(ent.path, "bak");
        const char *curName = obs_data_get_string(data, "name");

        if (astrcmpi(name, curName) == 0) {
            outputPath = ent.path;
            obs_data_release(data);
            break;
        }

        obs_data_release(data);
    }

    os_globfree(glob);

    if (!outputPath.empty()) {
        outputPath.resize(outputPath.size() - 5);
        replace(outputPath.begin(), outputPath.end(), '\\', '/');
        const char *start = strrchr(outputPath.c_str(), '/');
        if (start)
            outputPath.erase(0, start - outputPath.c_str() + 1);
    }

    return outputPath;
}

static auto ProfilerNameStoreRelease = [](profiler_name_store_t *store)
{
    profiler_name_store_free(store);
};
using ProfilerNameStore =
std::unique_ptr<profiler_name_store_t,
decltype(ProfilerNameStoreRelease)>;

ProfilerNameStore CreateNameStore()
{
    return ProfilerNameStore{profiler_name_store_create(),
                ProfilerNameStoreRelease};
}

COBSGlobal::COBSGlobal()
    :profilerNameStore(CreateNameStore().get())
{
    sleepInhibitor = os_inhibit_sleep_create("OBS Video/audio");
}

COBSGlobal::~COBSGlobal()
{
#ifdef __APPLE__
    bool vsyncDiabled = config_get_bool(globalConfig, "Video",
                                        "DisableOSXVSync");
    bool resetVSync = config_get_bool(globalConfig, "Video",
                                      "ResetOSXVSyncOnExit");
    if (vsyncDiabled && resetVSync)
        EnableOSXVSync(true);
#endif

    os_inhibit_sleep_set_active(sleepInhibitor, false);
    os_inhibit_sleep_destroy(sleepInhibitor);
}

static void move_basic_to_profiles(void)
{
    char path[512];
    char new_path[512];
    os_glob_t *glob;

    /* if not first time use */
    if (c_GetConfigPath(path, 512, "obs-studio/basic") <= 0)
        return;
    if (!os_file_exists(path))
        return;

    /* if the profiles directory doesn't already exist */
    if (c_GetConfigPath(new_path, 512, "obs-studio/basic/profiles") <= 0)
        return;
    if (os_file_exists(new_path))
        return;

    if (os_mkdir(new_path) == MKDIR_ERROR)
        return;

    strcat(new_path, "/");
    strcat(new_path, /*Str*/("Untitled"));
    if (os_mkdir(new_path) == MKDIR_ERROR)
        return;

    strcat(path, "/*.*");
    if (os_glob(path, 0, &glob) != 0)
        return;

    strcpy(path, new_path);

    for (size_t i = 0; i < glob->gl_pathc; i++) {
        struct os_globent ent = glob->gl_pathv[i];
        char *file;

        if (ent.directory)
            continue;

        file = strrchr(ent.path, '/');
        if (!file++)
            continue;

        if (astrcmpi(file, "scenes.json") == 0)
            continue;

        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, file);
        os_rename(ent.path, new_path);
    }

    os_globfree(glob);
}

bool COBSGlobal::initGlobal()
{
    if (!InitApplicationBundle())
        throw "Failed to initialize application bundle";
    if (!MakeUserDirs())
        throw "Failed to create required user directories";
    if (!InitGlobalConfig())
        throw "Failed to initialize global config";
 //   if (!InitLocale())
      //  throw "Failed to load locale";
  //  if (!InitTheme())
      //  throw "Failed to load theme";

    config_set_default_string(globalConfig, "Basic", "Profile",
                              /*Str*/("Untitled"));
    config_set_default_string(globalConfig, "Basic", "ProfileDir",
                              /*Str*/("Untitled"));
    config_set_default_string(globalConfig, "Basic", "SceneCollection",
                              /*Str*/("Untitled"));
    config_set_default_string(globalConfig, "Basic", "SceneCollectionFile",
                              /*Str*/("Untitled"));

#ifdef __APPLE__
    if (config_get_bool(globalConfig, "Video", "DisableOSXVSync"))
        EnableOSXVSync(false);
#endif

    move_basic_to_profiles();
 //   move_basic_to_scene_collections();

    if (!MakeUserProfileDirs())
        throw "Failed to create profile directories";

   // InitGlobalConfig();
    InitGlobalConfigDefaults();
    return true;
}

#ifdef __APPLE__
#define INPUT_AUDIO_SOURCE  "coreaudio_input_capture"
#define OUTPUT_AUDIO_SOURCE "coreaudio_output_capture"
#elif _WIN32
#define INPUT_AUDIO_SOURCE  "wasapi_input_capture"
#define OUTPUT_AUDIO_SOURCE "wasapi_output_capture"
#else
#define INPUT_AUDIO_SOURCE  "pulse_input_capture"
#define OUTPUT_AUDIO_SOURCE "pulse_output_capture"
#endif

const char *COBSGlobal::InputAudioSource() const
{
    return INPUT_AUDIO_SOURCE;
}

const char *COBSGlobal::OutputAudioSource() const
{
    return OUTPUT_AUDIO_SOURCE;
}

const char *COBSGlobal::GetRenderModule() const
{
    const char *renderer = config_get_string(globalConfig, "Video",
                                             "Renderer");

    return (astrcmpi(renderer, "Direct3D 11") == 0) ?
                DL_D3D11 : DL_OPENGL;
}

bool COBSGlobal::InitGlobalConfig()
{
    char path[512];

    int len = c_GetConfigPath(path, sizeof(path),
                            "obs-studio/global.ini");
    if (len <= 0) {
        return false;
    }

    int errorcode = globalConfig.Open(path, CONFIG_OPEN_ALWAYS);
    if (errorcode != CONFIG_SUCCESS) {
        OBSErrorBox(NULL, "Failed to open global.ini: %d", errorcode);
        return false;
    }

    if (!c_opt_starting_collection.empty()) {
        string path = GetSceneCollectionFileFromName(
                    c_opt_starting_collection.c_str());
        if (!path.empty()) {
            config_set_string(globalConfig,
                              "Basic", "SceneCollection",
                              c_opt_starting_collection.c_str());
            config_set_string(globalConfig,
                              "Basic", "SceneCollectionFile",
                              path.c_str());
        }
    }

    if (!c_opt_starting_profile.empty()) {
        string path = GetProfileDirFromName(
                    c_opt_starting_profile.c_str());
        if (!path.empty()) {
            config_set_string(globalConfig, "Basic", "Profile",
                              c_opt_starting_profile.c_str());
            config_set_string(globalConfig, "Basic", "ProfileDir",
                              path.c_str());
        }
    }

    return InitGlobalConfigDefaults();
}

bool COBSGlobal::InitGlobalConfigDefaults()
{
    config_set_default_string(globalConfig, "General", "Language",
                              DEFAULT_LANG);
    config_set_default_uint(globalConfig, "General", "MaxLogs", 10);
    config_set_default_string(globalConfig, "General", "ProcessPriority",
                              "Normal");

#if _WIN32
    config_set_default_string(globalConfig, "Video", "Renderer",
                              "Direct3D 11");
#else
    config_set_default_string(globalConfig, "Video", "Renderer", "OpenGL");
#endif

    config_set_default_bool(globalConfig, "BasicWindow", "PreviewEnabled",
                            true);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "PreviewProgramMode", false);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "SceneDuplicationMode", true);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "SwapScenesMode", true);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "SnappingEnabled", true);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "ScreenSnapping", true);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "SourceSnapping", true);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "CenterSnapping", false);
    config_set_default_double(globalConfig, "BasicWindow",
                              "SnapDistance", 10.0);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "RecordWhenStreaming", false);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "KeepRecordingWhenStreamStops", false);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "SysTrayEnabled", true);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "SysTrayWhenStarted", false);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "SaveProjectors", false);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "ShowTransitions", true);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "ShowListboxToolbars", true);
    config_set_default_bool(globalConfig, "BasicWindow",
                            "ShowStatusBar", true);

#ifdef __APPLE__
    config_set_default_bool(globalConfig, "Video", "DisableOSXVSync", true);
    config_set_default_bool(globalConfig, "Video", "ResetOSXVSyncOnExit",
                            true);
#endif
    return true;
}
