#include "Application.h"

int main(int argc, char *argv[]) {
    Application::setOrganizationName("vitark");
    Application::setApplicationName("uni-t.power-supply-management");
    Application::setApplicationDisplayName("UNI-T - Power Supply Management");
    Application::setApplicationVersion("1.0");

    Application a(argc, argv);
    return Application::exec();
}