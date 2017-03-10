#include "obs-app.hpp"
#include "util/base.h"
#include "cobspublic.h"
#include "ctestwidget.h"



//int main(int argc, char *argv[])
//{

////    doInit();

//    QCoreApplication::addLibraryPath(".");

//    OBSApp program(argc, argv,doInit());
//    try {
//        program.AppInit();

//        OBSTranslator translator;

////        create_log_file(logFile);
////        delete_oldest_file("obs-studio/profiler_data");

//        program.installTranslator(&translator);

//        if (!program.OBSInit())
//            return 0;

////        prof.Stop();

//        CTestWidget tw;
//        tw.show();
//        return program.exec();

//    } catch (const char *error) {
//        blog(LOG_ERROR, "%s", error);
////        OBSErrorBox(nullptr, "%s", error);
//    }

////    blog(LOG_INFO, "Number of memory leaks: %ld", bnum_allocs());
////    base_set_log_handler(nullptr, nullptr);
//    return 0;
//}
