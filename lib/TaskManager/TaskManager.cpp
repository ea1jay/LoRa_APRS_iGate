#include "TaskManager.h"
#include <FontConfig.h>
#include <logger.h>

TaskManager::TaskManager() {
}

void TaskManager::addTask(std::shared_ptr<Task> task) {
  _tasks.push_back(task);
}

std::shared_ptr<Task> TaskManager::getTask(const char *name) {
  std::_List_iterator<std::shared_ptr<Task>> elem = std::find_if(_tasks.begin(), _tasks.end(), [&](std::shared_ptr<Task> task) {
    return task->getName() == name;
  });
  if (elem == _tasks.end()) {
    return 0;
  }
  return *elem;
}

std::list<std::shared_ptr<Task>> TaskManager::getTasks() {
  return _tasks;
}

bool TaskManager::setup(std::shared_ptr<Configuration> config, std::shared_ptr<BoardConfig> boardConfig) {
  logPrintlnV("will setup all tasks...");
  for (std::shared_ptr<Task> &elem : _tasks) {
    logPrintW("call setup from ");
    logPrintlnW(elem->getName());
    if (!elem->setup(config, boardConfig)) {
      return false;
    }
  }
  return true;
}

bool TaskManager::loop(std::shared_ptr<Configuration> config) {
  // logPrintlnD("will loop all tasks...");
  for (std::shared_ptr<Task> &elem : _tasks) {
    // logPrintD("call loop from ");
    // logPrintlnD(elem->getName());
    if (!elem->loop(config)) {
      return false;
    }
  }
  return true;
}

void StatusFrame::drawStatusPage(Bitmap &bitmap) {
  int y = 0;
  for (std::shared_ptr<Task> task : _tasks) {
    int x = bitmap.drawString(0, y, (task->getName()).substring(0, task->getName().indexOf("Task")));
    x     = bitmap.drawString(x, y, ": ");
    if (task->getStateInfo() == "") {
      switch (task->getState()) {
      case Error:
        bitmap.drawString(x, y, "Error");
        break;
      case Warning:
        bitmap.drawString(x, y, "Warning");
      default:
        break;
      }
      bitmap.drawString(x, y, "Okay");
    } else {
      bitmap.drawString(x, y, task->getStateInfo());
    }
    y += getSystemFont()->heightInPixel;
  }
}

bool StatusFrame::isPrio() const {
  return std::any_of(_tasks.begin(), _tasks.end(), [](std::shared_ptr<Task> task) {
    return task->getState() != Okay;
  });
}
