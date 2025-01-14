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

class LogicMainWidget final : public QObject
{
Q_OBJECT

public:
    LogicMainWidget();
    ~LogicMainWidget();

public:
    void startTimer();

private:
    DWORD       getFocusedApplicationPID();
    QPixmap     getIconForProcess(DWORD pid);
    std::string getProcessNameByPid(DWORD pid);

private:
    void trackActiveApplication();
    void printApplicationTime(DWORD pid);
    void updateApplicationIcon(DWORD pid);
    void startTrackingForNewApplication(DWORD pid);
    void pauseTimer(DWORD pid,QElapsedTimer* timer);
    void updateTrackingForActiveApplications(DWORD pid);

signals:
     void updateUI(const std::string& appName,QPixmap &icon, int hours, int minutes, int seconds);

private:
    QTimer                                      *mainTimer;
    DWORD                                       currentAppPID;
    QMap<DWORD, QPixmap>                        processIcons;       // заменить на struct
    std::unordered_map<DWORD, QPixmap>          iconCache;          // заменить на struct
    std::unordered_map<DWORD, QElapsedTimer*>   activeTimers;       // заменить на struct
    std::unordered_map<DWORD, bool>             isAppInFocus;       // заменить на struct
    std::unordered_map<DWORD, int>              accumulatedTime;    // заменить на struct
};

#endif //WINWIDGET_LOGICMAINWIDGET_H
