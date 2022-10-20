#include "Application.h"

int main(int argc, char *argv[]) {
    Application::setOrganizationName("vitark");
    Application::setApplicationName("power-supply-management");
    Application::setApplicationDisplayName("PS-Management");
    Application::setApplicationVersion("1.2.1");

    Application a(argc, argv);
    Application::setWindowIcon(QIcon(":power-supply-512"));

    return Application::exec();
}