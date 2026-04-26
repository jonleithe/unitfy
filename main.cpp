
/*
 * filename:    main.cpp
 * created:     26 April 2026
 * author:      jole
 * description: Entry point; instantiates and runs the MDelta application.
 */

#include "delta.h"

int main(int argc, char *argv[]) {
    unitfy::MDelta app;
    return app.run(argc, argv);
}   