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
#include <psapi.h>
#include <QCloseEvent>
#include <QPushButton>
#include <QScrollArea>

#include "logicMainWidget.h"

class MainWidget : public QWidget
{
Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

public:
    void start();

private:
    void setupUI();
    void connectSignals();
    void togglePinWindow();
private:
    LogicMainWidget logic;
    QMap<std::string, QIcon> activeProgram;
    QMap<std::string, QLabel*> appLabels;
    QMap<std::string, QLabel*> iconLabels;
    QMap<std::string, QLabel*> timeLabels;
    QVBoxLayout* appListLayout;
    QPushButton* pinButton;
    bool isPinned;

public slots:
    void activeAppUpdate(const std::string &appName, const QPixmap &icon, int hours, int minutes, int seconds);

};

#endif // MAINWIDGET_H
