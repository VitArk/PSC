#include "Application.h"

int main(int argc, char *argv[]) {
    Application::setOrganizationName("vitark");
    Application::setApplicationName("power-supply-management");
    Application::setApplicationDisplayName("PS Management");
    Application::setApplicationVersion("1.0");

    Application a(argc, argv);
    return Application::exec();
}