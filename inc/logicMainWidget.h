//
// Created by winlogon on 04.01.2025.
//

#ifndef WINWIDGET_LOGICMAINWIDGET_H
#define WINWIDGET_LOGICMAINWIDGET_H

#include <QLabel>
#include <QMap>
#include <QTimer>
#include <string>
#include <chrono>
#include <windows.h>
#include <QCloseEvent>
#include <QProcess>
#include <QFileInfo>
#include <QIcon>
#include <QPixmap>
#include <psapi.h>

class LogicMainWidget : public QObject
{
Q_OBJECT

private:
    std::unordered_map<DWORD, std::chrono::steady_clock::time_point> activeTimers; // Используем time_point
    std::unordered_map<DWORD, int> accumulatedTime;

public:
    LogicMainWidget();
    ~LogicMainWidget();

signals:
     void updateUI(const std::string& appName,QPixmap &icon, int hours, int minutes, int seconds);

public:
    std::chrono::steady_clock::time_point lastAppTime;

public:
    void trackActiveApplication();
    void getIconForProcess(DWORD pid, int hours, int minutes, int seconds);
    std::string getProcessNameByPid(DWORD pid);
    void stopTrackingApplication(DWORD pid);
    DWORD getFocusedApplicationPID();

};

#endif //WINWIDGET_LOGICMAINWIDGET_H
