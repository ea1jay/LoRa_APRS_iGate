#include <SPIFFS.h>

#include <logger.h>

#include "project_configuration.h"

std::shared_ptr<Configuration> ProjectConfigurationManagement::readProjectConfiguration(DynamicJsonDocument &data) {
  std::shared_ptr<Configuration> conf = std::shared_ptr<Configuration>(new Configuration);
  if (data.containsKey("callsign"))
    conf->callsign = data["callsign"].as<String>();

  JsonArray aps = data["wifi"]["AP"].as<JsonArray>();
  for (JsonVariant v : aps) {
    Configuration::Wifi::AP ap;
    ap.SSID     = v["SSID"].as<String>();
    ap.password = v["password"].as<String>();
    conf->wifi.APs.push_back(ap);
  }
  if (data.containsKey("beacon") && data["beacon"].containsKey("message"))
    conf->beacon.message = data["beacon"]["message"].as<String>();
  conf->beacon.positionLatitude  = data["beacon"]["position"]["latitude"] | 0.0;
  conf->beacon.positionLongitude = data["beacon"]["position"]["longitude"] | 0.0;
  conf->beacon.timeout           = data["beacon"]["timeout"] | 15;
  if (data.containsKey("aprs_is") && data["aprs_is"].containsKey("passcode"))
    conf->aprs_is.passcode = data["aprs_is"]["passcode"].as<String>();
  if (data.containsKey("aprs_is") && data["aprs_is"].containsKey("server"))
    conf->aprs_is.server = data["aprs_is"]["server"].as<String>();
  conf->aprs_is.port = data["aprs_is"]["port"] | 14580;

  conf->lora.frequencyRx     = data["lora"]["frequency_rx"] | 433775000;
  conf->lora.frequencyTx     = data["lora"]["frequency_tx"] | 433775000;
  conf->lora.power           = data["lora"]["power"] | 20;
  conf->lora.spreadingFactor = data["lora"]["spreading_factor"] | 12;
  conf->lora.signalBandwidth = data["lora"]["signal_bandwidth"] | 125000;
  conf->lora.codingRate4     = data["lora"]["coding_rate4"] | 5;
  conf->display.alwaysOn     = data["display"]["always_on"] | true;
  conf->display.timeout      = data["display"]["timeout"] | 10;
  conf->display.overwritePin = data["display"]["overwrite_pin"] | 0;
  conf->display.turn180      = data["display"]["turn180"] | true;

  conf->ftp.active = data["ftp"]["active"] | false;
  JsonArray users  = data["ftp"]["user"].as<JsonArray>();
  for (JsonVariant u : users) {
    Configuration::Ftp::User us;
    us.name     = u["name"].as<String>();
    us.password = u["password"].as<String>();
    conf->ftp.users.push_back(us);
  }
  if (conf->ftp.users.empty()) {
    Configuration::Ftp::User us;
    us.name     = "ftp";
    us.password = "ftp";
    conf->ftp.users.push_back(us);
  }
  if (data.containsKey("ntp_server"))
    conf->ntpServer = data["ntp_server"].as<String>();

  if (data.containsKey("board"))
    conf->board = data["board"].as<String>();

  return conf;
}

void ProjectConfigurationManagement::writeProjectConfiguration(std::shared_ptr<Configuration> conf, DynamicJsonDocument &data) {
  data["callsign"] = conf->callsign;
  JsonArray aps    = data["wifi"].createNestedArray("AP");
  for (Configuration::Wifi::AP ap : conf->wifi.APs) {
    JsonObject v  = aps.createNestedObject();
    v["SSID"]     = ap.SSID;
    v["password"] = ap.password;
  }
  data["beacon"]["message"]               = conf->beacon.message;
  data["beacon"]["position"]["latitude"]  = conf->beacon.positionLatitude;
  data["beacon"]["position"]["longitude"] = conf->beacon.positionLongitude;
  data["beacon"]["timeout"]               = conf->beacon.timeout;
  data["aprs_is"]["passcode"]             = conf->aprs_is.passcode;
  data["aprs_is"]["server"]               = conf->aprs_is.server;
  data["aprs_is"]["port"]                 = conf->aprs_is.port;
  data["lora"]["frequency_rx"]            = conf->lora.frequencyRx;
  data["lora"]["frequency_tx"]            = conf->lora.frequencyTx;
  data["lora"]["power"]                   = conf->lora.power;
  data["lora"]["spreading_factor"]        = conf->lora.spreadingFactor;
  data["lora"]["signal_bandwidth"]        = conf->lora.signalBandwidth;
  data["lora"]["coding_rate4"]            = conf->lora.codingRate4;
  data["display"]["always_on"]            = conf->display.alwaysOn;
  data["display"]["timeout"]              = conf->display.timeout;
  data["display"]["overwrite_pin"]        = conf->display.overwritePin;
  data["display"]["turn180"]              = conf->display.turn180;
  data["ftp"]["active"]                   = conf->ftp.active;
  JsonArray users                         = data["ftp"].createNestedArray("user");
  for (Configuration::Ftp::User u : conf->ftp.users) {
    JsonObject v  = users.createNestedObject();
    v["name"]     = u.name;
    v["password"] = u.password;
  }
  data["ntp_server"] = conf->ntpServer;

  data["board"] = conf->board;
}

std::shared_ptr<Configuration> load_config(std::shared_ptr<BoardConfig> boardConfig) {
  ProjectConfigurationManagement confmg;
  std::shared_ptr<Configuration> config = confmg.readConfiguration();
  if (config->callsign == "NOCALL-10") {
    logPrintlnE("You have to change your settings in 'data/is-cfg.json' and upload it via \"Upload File System image\"!");
    // show_display("ERROR", "You have to change your settings in 'data/is-cfg.json' and upload it via \"Upload File System image\"!");
    while (true) {
    }
  }

  /*if(KEY_BUILTIN != 0 && Config->display.overwritePin == 0)
  {
          Config->display.overwritePin = KEY_BUILTIN;
  }*/
  logPrintlnI("Configuration loaded!");
  return config;
}
