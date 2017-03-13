#ifndef COBSGLOBAL_H
#define COBSGLOBAL_H
#include <QObject>
#include "util/util.hpp"
#include "util/platform.h"
#include "util/profiler.h"
#include "cobspublic.h"

class COBSGlobal : public QObject
{
    Q_OBJECT
public:
    COBSGlobal();
    ~COBSGlobal();
    bool initGlobal();

    inline config_t *GlobalConfig() const {return globalConfig;}
    const char *InputAudioSource() const;
    const char *OutputAudioSource() const;
    const char *GetRenderModule() const;

    profiler_name_store_t *GetProfilerNameStore() const
    {
        return profilerNameStore;
    }

    inline void IncrementSleepInhibition()
    {
        if (!sleepInhibitor) return;
        if (sleepInhibitRefs++ == 0)
            os_inhibit_sleep_set_active(sleepInhibitor, true);
    }

    inline void DecrementSleepInhibition()
    {
        if (!sleepInhibitor) return;
        if (sleepInhibitRefs == 0) return;
        if (--sleepInhibitRefs == 0)
            os_inhibit_sleep_set_active(sleepInhibitor, false);
    }

private:
    //==============global
    bool InitGlobalConfig();
    bool InitGlobalConfigDefaults();





private:
    profiler_name_store_t          *profilerNameStore = nullptr;
    ConfigFile    globalConfig;
    os_inhibit_t                   *sleepInhibitor = nullptr;
    int                            sleepInhibitRefs = 0;
};

//inline const char *Str(const char *lookup) {return App()->GetString(lookup);}
//#define QTStr(lookupVal) QString::fromUtf8(Str(lookupVal))
//inline const char *Str(const char *lookup) {return QString::fromUtf8(lookup).toLatin1().data();}

#endif // COBSGLOBAL_H
