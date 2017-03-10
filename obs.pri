
C_OBS_ROOT=E:\\bitbrothers\\obs-studio

C_RELEASE_DEBUG=debug
INCLUDEPATH +=$${C_OBS_ROOT}/libobs
LIBS += -luser32  -lkernel32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32

LIBS += -LE:\\bitbrothers\\obs-studio\\build\\libobs\\Debug -lobs

LIBS += -LE:\\bitbrothers\\obs-studio\\build\UI\\obs-frontend-api\\Debug -lobs-frontend-api

INCLUDEPATH +=E:\\bitbrothers\\obs-studio\\libobs
LIBS += -LE:\\bitbrothers\\obs-studio\\dependencies2013\\win32\\bin -llibcurl -lavcodec -lavfilter -lavdevice -lavutil -lswscale -lavformat -lswresample


INCLUDEPATH +=E:\bitbrothers\obs-studio\dependencies2013\win32\include

INCLUDEPATH +=E:\bitbrothers\obs-studio\deps\libff

INCLUDEPATH +=E:\bitbrothers\obs-studio\UI\obs-frontend-api


INCLUDEPATH +=E:\\bitbrothers\\obs-studio\\deps\\libff\\libff

LIBS += -LE:\\bitbrothers\\obs-studio\\build\\deps\\libff\\Debug -llibff
LIBS += -LE:\\bitbrothers\\obs-studio\\build\\deps\\file-updater\\Debug -lfile-updater
LIBS += -LE:\\bitbrothers\\obs-studio\\build\\deps\\glad\\Debug -lobsglad
LIBS += -LE:\\bitbrothers\\obs-studio\\build\\deps\\ipc-util\\Debug -lipc-util
LIBS += -LE:\\bitbrothers\\obs-studio\\build\\deps\\jansson\\lib\\Debug -ljansson_d
LIBS += -LE:\\bitbrothers\\obs-studio\\build\\deps\\w32-pthreads\\Debug -lw32-pthreads

