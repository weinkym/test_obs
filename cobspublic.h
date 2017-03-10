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

#endif // COBSPUBLIC_H
