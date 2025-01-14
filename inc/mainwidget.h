#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include <QTimer>
#include <QIcon>
#include <string>
#include <chrono>
#include <windows.h>
#include <QCloseEvent>
#include <QProcess>
#include <QThread>
#include <psapi.h>
#include <QCloseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

#include "logicMainWidget.h"

struct AppInfo {
    std::string name;
    QPixmap icon;
    QLabel *iconLabel;
    QLabel *appLabel;
    QLabel *timeLabel;
    int totalTimeInSeconds;
};

class MainWidget : public QWidget
{
Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

public:
    void start();

private:
    QLabel* createIconLabel(const QPixmap &icon);
    QLabel* createAppLabel(const std::string &appName);
    QLabel* createTimeLabel(int hours, int minutes, int seconds);
    QString formatTime(int hours, int minutes, int seconds);

private:
    void setupUI();
    void setupHeader(QVBoxLayout *mainLayout);
    void setupSeparator(QVBoxLayout *mainLayout);
    void setupScrollArea(QVBoxLayout *mainLayout);
    void setupPinButton(QVBoxLayout *mainLayout);
    void setupTrayIcon();
    void setupIconApp();

    void addNewAppRow(const std::string &appName, const QPixmap &icon, int hours, int minutes, int seconds);
    void updateExistingAppRow(const std::string &appName, const QPixmap &icon, int hours, int minutes, int seconds);
    void connectSignals();
    void togglePinWindow();
    void highlightActiveApp(const std::string &appName);
    void changeEvent(QEvent *event);

private:
    LogicMainWidget             logic;
    QMap<std::string, AppInfo>  activePrograms;
    QVBoxLayout                 *appListLayout;
    QPushButton                 *pinButton;
    bool                        isPinned;

    QSystemTrayIcon             *trayIcon;
    QMenu                       *trayMenu;
    QAction                     *restoreAction;
    QAction                     *quitAction;
    QHBoxLayout                 *rowLayout;

public slots:
    void activeAppUpdate(const std::string &appName, const QPixmap &icon, int hours, int minutes, int seconds);
};


#endif // MAINWIDGET_H
