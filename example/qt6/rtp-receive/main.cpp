#include <glog/logging.h>

#include <QCoreApplication>
#include <QDebug>
#include <QRtp>
#include <QString>
#include <QTimer>

#include "rtp/rtp_utils.h"
#include "rtp_receive.h"
#include "version.h"

/// [Create a Qt application]
int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  // Initalise the RTP library
  mediax::InitRtp(argc, argv);
  // Create a QtReceive object
  QtReceive rtp;
  return a.exec();
  // Cleanuo the RTP library
  mediax::RtpCleanup();
}
/// [Create a Qt application]
