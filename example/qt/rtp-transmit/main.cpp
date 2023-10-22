#include <glog/logging.h>

#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QTimer>

#include "qt/QtRtp.h"
#include "rtp/rtp_utils.h"
#include "rtp_transmit.h"
#include "version.h"

int main(int argc, char *argv[]) {
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);
  QCoreApplication a(argc, argv);

  mediax::InitRtp(argc, argv);

  QtTransmit rtp;
  // Create a times evert 40 ms
  QTimer::singleShot(40, &rtp, SLOT(sendFrame()));

  // COnvert mediax::kVersion to QString
  QString version = QString::fromStdString(mediax::kVersion);

  // Set version
  a.setApplicationVersion(version);

  return a.exec();
}