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

public:
    LogicMainWidget();
    ~LogicMainWidget();

public:
    void trackActiveApplication();
    QPixmap getIconForProcess(DWORD pid);
    void pauseTimer(DWORD pid,QElapsedTimer* timer);
    void startTimer();
    void startTrackingForNewApplication(DWORD pid);
    void updateTrackingForActiveApplications(DWORD pid);
    void printApplicationTime(DWORD pid);
    void updateApplicationIcon(DWORD pid);

    std::string getProcessNameByPid(DWORD pid);
    DWORD getFocusedApplicationPID();

signals:
     void updateUI(const std::string& appName,QPixmap &icon, int hours, int minutes, int seconds);

private:
    std::unordered_map<DWORD, QElapsedTimer*> activeTimers;
    std::unordered_map<DWORD, int> accumulatedTime;
    std::unordered_map<DWORD, bool> isAppInFocus;
    std::unordered_map<DWORD, QPixmap> iconCache;
    QMap<DWORD, QPixmap> processIcons;
    DWORD currentAppPID;
    QTimer *mainTimer;
};

#endif //WINWIDGET_LOGICMAINWIDGET_H
