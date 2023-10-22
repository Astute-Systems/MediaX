#include <glog/logging.h>

#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QTimer>

#include "qt/QtRtp.h"
#include "rtp/rtp_utils.h"
#include "rtp_transmit.h"
#include "version.h"

/// [Create a Qt application]
int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  // Initalise the RTP library
  mediax::InitRtp(argc, argv);
  // Create a QtTransmit object
  QtTransmit rtp;
  // Create a timer 40 ms to start the first frame
  QTimer::singleShot(40, &rtp, SLOT(sendFrame()));
  return a.exec();
}
/// [Create a Qt application]
