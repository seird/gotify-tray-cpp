#include "mainapplication.h"


int main(int argc, char *argv[])
{
    MainApplication app(argc, argv);

    if (app.acquireLock() && app.verifyServer()) {
        app.init();
        return app.exec();
    } else {
        qFatal() << "Failed to acquire lock.";
        return EXIT_FAILURE;
    }
}
