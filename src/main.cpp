#include "controllers/ScreenCaptureController.h"
#include "translation/GoogleTranslator.h"
#include "ui/CaptureFrame.h"

#include <QShortcut>
#include <iostream>
#include <sys/socket.h>
#include "qhotkey.h"

void connectScreenUpdated(const QScreen& screen, const QObject& receiver, const char* member);

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    const auto screen = std::unique_ptr<QScreen>(QGuiApplication::primaryScreen());
    const CaptureFrame captureFrame(screen.get());
    connectScreenUpdated(*screen, captureFrame, SLOT(onScreenUpdated()));

    const SystemTrayIcon trayIcon(&app);
    QMainWindow window;

    // QHotkey hotkey(QKeySequence("Command+Shift+T"), true, &app);
    // if (!hotkey.isRegistered()) {
    //     qWarning() << "Global shortcut could not be registered!";
    // }

    QObject::connect(&trayIcon, SIGNAL(leftClicked()), &captureFrame, SLOT(onScreenCaptureStarted()));
    // QObject::connect(&hotkey, SIGNAL(activated()), &captureFrame, SLOT(onScreenCaptureStarted()));

    QDir dir(QCoreApplication::applicationDirPath());

#ifdef Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif

    dir.cd("tessdata");

    const ocr::TesseractParser parser(static_cast<int>(screen->devicePixelRatio()), dir.absolutePath().toStdString());
    connectScreenUpdated(*screen, parser, SLOT(onScreenUpdated()));

    const translation::GoogleTranslator translator;
    const ScreenCaptureController captureController(parser, translator);

    QObject::connect(&captureFrame, SIGNAL(screenCaptured(QPoint,QPixmap)), &captureController,
                     SLOT(onScreenCaptured(QPoint,QPixmap)));

    const Viewer viewer;
    QObject::connect(&captureController, SIGNAL(resultUpdated(QPixmap)), &viewer, SLOT(onResultUpdated(QPixmap)));
    QObject::connect(&captureController, SIGNAL(resultUpdatedWithPoint(QPoint,QPixmap)), &viewer,
                     SLOT(onResultUpdatedWithPoint(QPoint,QPixmap)));

    QCoreApplication::exec();
}

void connectScreenUpdated(const QScreen& screen, const QObject& receiver, const char* member) {
    QObject::connect(&screen, SIGNAL(geometryChanged(QRect)), &receiver, member);
    QObject::connect(&screen, SIGNAL(availableGeometryChanged(QRect)), &receiver, member);
    QObject::connect(&screen, SIGNAL(virtualGeometryChanged(QRect)), &receiver, member);
}