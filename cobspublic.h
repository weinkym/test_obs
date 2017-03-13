#ifndef COBSPUBLIC_H
#define COBSPUBLIC_H

#include <QTimer>
#include <QDebug>
#include <QFile>
#include <memory>
#include <QPointer>

#include "obs-data.h"
#include "obs.hpp"
#include "obs.h"
#define C_TEST_LOG qDebug()<<"TTTTTTTTTTTTTV:"<<__LINE__<<Q_FUNC_INFO
#ifndef DL_OPENGL
#define DL_OPENGL "libobs-opengl.dll"
#endif

#ifndef DL_D3D11
#define DL_D3D11 "libd3d11.dll"
#endif

#define DEFAULT_LANG "en-US"

struct OutputParam
{
    obs_sceneitem_crop crop;
    struct vec2 bounds;
};

profiler_name_store_t* doInit();
#endif // COBSPUBLIC_H
