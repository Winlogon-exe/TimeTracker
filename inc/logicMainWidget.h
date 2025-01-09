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
    std::unordered_map<DWORD, QElapsedTimer*> activeTimers; // Используем time_point
    std::unordered_map<DWORD, int> accumulatedTime;
    std::unordered_map<DWORD, bool> isAppInFocus;
    QTimer *mainTimer;
    DWORD currentAppPID;
    QMap<DWORD, QPixmap> processIcons;
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
    void pauseTimer(DWORD pid,QElapsedTimer* timer);
    std::string getProcessNameByPid(DWORD pid);
    DWORD getFocusedApplicationPID();
    void startTimer();
    void startTrackingForNewApplication(DWORD pid);
    void updateTrackingForActiveApplications(DWORD pid);
    void printApplicationTime(DWORD pid);
    void updateApplicationIcon(DWORD pid);
};

#endif //WINWIDGET_LOGICMAINWIDGET_H
