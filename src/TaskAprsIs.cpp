#include <TimeLib.h>
#include <logger.h>

#include "Task.h"
#include "TaskAprsIs.h"
#include "project_configuration.h"

String create_lat_aprs(double lat);
String create_long_aprs(double lng);

AprsIsTask::AprsIsTask() : Task(TASK_APRS_IS, TaskAprsIs) {
}

AprsIsTask::~AprsIsTask() {
}

bool AprsIsTask::setup(std::shared_ptr<Configuration> config, std::shared_ptr<BoardConfig> boardConfig) {
  _beacon_timer.setTimeout(minutesToTime_t(config->beacon.timeout));
  _aprs_is = std::shared_ptr<APRS_IS>(new APRS_IS(config->callsign, config->aprs_is.passcode, "ESP32-APRS-IS", "0.2"));

  _beaconMsg = std::shared_ptr<APRSMessage>(new APRSMessage());
  _beaconMsg->setSource(config->callsign);
  _beaconMsg->setDestination("APLG1");
  String lat = create_lat_aprs(config->beacon.positionLatitude);
  String lng = create_long_aprs(config->beacon.positionLongitude);
  _beaconMsg->getBody()->setData(String("=") + lat + "L" + lng + "&" + config->beacon.message);

  return true;
}

bool AprsIsTask::loop(std::shared_ptr<Configuration> config) {
  if (!_aprs_is->connected()) {
    if (!connect(config)) {
      _stateInfo = "not connected";
      _state     = Error;
      return false;
    }
    _stateInfo = "connected";
    _state     = Okay;
    return false;
  }

  _aprs_is->getAPRSMessage();

  if (!inputQueue.empty()) {
    std::shared_ptr<APRSMessage> msg = inputQueue.getElement();
    _aprs_is->sendMessage(msg);
  }

  if (_beacon_timer.check()) {
    logPrintD("[" + timeString() + "] ");
    logPrintlnD(_beaconMsg->encode());
    _aprs_is->sendMessage(_beaconMsg);
    Display::instance().addFrame(std::shared_ptr<DisplayFrame>(new TextFrame("BEACON", _beaconMsg->toString())));
    _beacon_timer.start();
  }
  time_t diff = _beacon_timer.getTriggerTime() - now();
  _stateInfo  = "beacon " + String(minute(diff)) + ":" + String(second(diff));
  _state      = Okay;
  return true;
}

bool AprsIsTask::connect(std::shared_ptr<Configuration> config) {
  logPrintI("connecting to APRS-IS server: ");
  logPrintI(config->aprs_is.server);
  logPrintI(" on port: ");
  logPrintlnI(String(config->aprs_is.port));
  if (!_aprs_is->connect(config->aprs_is.server, config->aprs_is.port)) {
    logPrintlnE("Connection failed.");
    return false;
  }
  logPrintlnI("Connected to APRS-IS server!");
  return true;
}
