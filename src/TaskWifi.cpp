#include <WiFi.h>
#include <logger.h>

#include "Task.h"
#include "TaskWifi.h"
#include "project_configuration.h"

WifiTask::WifiTask() : Task(TASK_WIFI, TaskWifi), _oldWifiStatus(WL_IDLE_STATUS) {
}

WifiTask::~WifiTask() {
}

bool WifiTask::setup(std::shared_ptr<Configuration> config, std::shared_ptr<BoardConfig> boardConfig) {
  // WiFi.onEvent(WiFiEvent);
  WiFi.setHostname(config->callsign.c_str());
  _wiFiMulti = std::shared_ptr<WiFiMulti>(new WiFiMulti());
  ;
  for (Configuration::Wifi::AP ap : config->wifi.APs) {
    logPrintD("Looking for AP: ");
    logPrintlnD(ap.SSID);
    _wiFiMulti->addAP(ap.SSID.c_str(), ap.password.c_str());
  }
  return true;
}

bool WifiTask::loop(std::shared_ptr<Configuration> config) {
  const uint8_t wifi_status = _wiFiMulti->run();
  if (wifi_status != WL_CONNECTED) {
    logPrintlnE("WiFi not connected!");
    _oldWifiStatus = wifi_status;
    _stateInfo     = "WiFi not connected";
    _state         = Error;
    return false;
  } else if (wifi_status != _oldWifiStatus) {
    logPrintD("IP address: ");
    logPrintlnD(WiFi.localIP().toString());
    _oldWifiStatus = wifi_status;
    return false;
  }
  _stateInfo = WiFi.localIP().toString();
  _state     = Okay;
  return true;
}
