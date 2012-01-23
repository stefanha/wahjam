#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  /* These are used by QSettings persistent settings */
  QCoreApplication::setOrganizationName("Wahjam Project");
  QCoreApplication::setOrganizationDomain("wahjam.org");
  QCoreApplication::setApplicationName("Wahjam");

  MainWindow mainWindow;

  mainWindow.show();
  return app.exec();
}
