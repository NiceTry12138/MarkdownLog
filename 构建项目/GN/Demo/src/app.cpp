#include "app.h"
#include "utils/utils.h"
#include <iostream>

void App::run() {
    std::cout << "App running" << std::endl;
    Utils::doSomething();
}
