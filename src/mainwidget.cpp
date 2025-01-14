#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    connectSignals();
}

MainWidget::~MainWidget()
{
}

void MainWidget::start()
{
    logic.startTimer(); //мб логику передавать в конструктор?
}

void MainWidget::setupUI()
{
    this->resize(400, 300);
    this->setMinimumSize(200, 200);
    this->setStyleSheet("background-color: #1e1e1e; color: #ffffff;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setupHeader(mainLayout);
    setupSeparator(mainLayout);
    setupScrollArea(mainLayout);
    setupPinButton(mainLayout);
    setupIconApp();
    setupTrayIcon();
}

void MainWidget::setupScrollArea(QVBoxLayout *layout)
{
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("border: none; background-color: #252525;");

    QWidget *scrollWidget = new QWidget(scrollArea);
    scrollArea->setWidget(scrollWidget);

    appListLayout = new QVBoxLayout(scrollWidget);
    appListLayout->setContentsMargins(10, 10, 10, 10);
    appListLayout->setSpacing(10);

    layout->addWidget(scrollArea, 1);
}

void MainWidget::setupPinButton(QVBoxLayout *layout)
{
    pinButton = new QPushButton("Pin Window", this);
    pinButton->setStyleSheet(
            "QPushButton {"
            "  font-size: 14px; font-weight: bold; color: #ffffff; background-color: #2e86c1;"
            "  border: none; padding: 8px 15px; border-radius: 5px;"
            "}"
            "QPushButton:hover { background-color: #1f669a; }"
            "QPushButton:pressed { background-color: #14496d; }"); // сделать норм css не в коде
    layout->addWidget(pinButton, 0, Qt::AlignCenter);
}

void MainWidget::setupSeparator(QVBoxLayout *layout)
{
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("color: #2c2c2c; margin: 0 10px;");
    layout->addWidget(separator);
}

void MainWidget::setupHeader(QVBoxLayout *layout)
{
    QLabel *titleLabel = new QLabel("Active Applications", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #f0f0f0; padding: 10px;");
    layout->addWidget(titleLabel);
}

void MainWidget::connectSignals()
{
    connect(&logic, &LogicMainWidget::updateUI, this, &MainWidget::activeAppUpdate); // activeAppUpdate вызывается каждую секунду
    connect(pinButton, &QPushButton::clicked, this, &MainWidget::togglePinWindow);
}

void MainWidget::activeAppUpdate(const std::string &appName, const QPixmap &icon, int hours, int minutes, int seconds)
{
    if (activePrograms.find(appName) == activePrograms.end())
    {
        addNewAppRow(appName, icon, hours, minutes, seconds);
    }
    else
    {
        updateExistingAppRow(appName, icon, hours, minutes, seconds);
    }
    highlightActiveApp(appName);
}

void MainWidget::sortAppsByTime()
{
    QVector<AppInfo> sortedApps = activePrograms.values().toVector();
    std::sort(sortedApps.begin(), sortedApps.end(), [](const AppInfo &a, const AppInfo &b)
    {
        return a.totalTime > b.totalTime;
    });

    while (QLayoutItem *item = appListLayout->takeAt(0))
    {
        delete item->widget();
        delete item;
    }

    for (const AppInfo &appInfo : sortedApps)
    {
        rowLayout = new QHBoxLayout();
        rowLayout->addWidget(appInfo.iconLabel, 0);
        rowLayout->addWidget(appInfo.appLabel, 2);
        rowLayout->addWidget(appInfo.timeLabel, 0);

        appListLayout->addLayout(rowLayout);
    }
}

void MainWidget::addNewAppRow(const std::string &appName, const QPixmap &icon, int hours, int minutes, int seconds)
{
    rowLayout = new QHBoxLayout();

    QLabel *iconLabel = createIconLabel(icon);
    QLabel *appLabel = createAppLabel(appName);
    QLabel *timeLabel = createTimeLabel(hours, minutes, seconds);

    rowLayout->addWidget(iconLabel, 0);
    rowLayout->addWidget(appLabel, 2);
    rowLayout->addWidget(timeLabel, 0);

    appListLayout->addLayout(rowLayout);

    AppInfo appInfo = { appName, icon, iconLabel, appLabel, timeLabel };
    activePrograms[appName] = appInfo;
}

void MainWidget::updateExistingAppRow(const std::string &appName, const QPixmap &icon, int hours, int minutes, int seconds)
{
    AppInfo &appInfo = activePrograms[appName];

    int timeInSeconds = hours  + minutes  + seconds;
    int elapsedTime = timeInSeconds - appInfo.totalTime;

    appInfo.totalTime = timeInSeconds;
    appInfo.iconLabel->setPixmap(icon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    appInfo.timeLabel->setText(formatTime(hours, minutes, seconds));

    if (elapsedTime > 0)
    {
        sortAppsByTime();
    }
}

QLabel* MainWidget::createIconLabel(const QPixmap &icon)
{
    QLabel *iconLabel = new QLabel(this);
    iconLabel->setPixmap(icon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setMinimumSize(40, 40);
    iconLabel->setAlignment(Qt::AlignCenter);
    return iconLabel;
}

QLabel* MainWidget::createAppLabel(const std::string &appName)
{
    QLabel *appLabel = new QLabel(QString::fromStdString(appName), this);
    appLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #6cb4f0;");
    appLabel->setMinimumHeight(32);
    appLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    return appLabel;
}

QLabel* MainWidget::createTimeLabel(int hours, int minutes, int seconds)
{
    QLabel *timeLabel = new QLabel(this);
    timeLabel->setText(formatTime(hours, minutes, seconds));
    timeLabel->setStyleSheet("font-size: 12px; color: #c4c4c4;");
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setMinimumWidth(30);
    return timeLabel;
}

QString MainWidget::formatTime(int hours, int minutes, int seconds)
{
    return QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
}

void MainWidget::togglePinWindow()
{
    if (isPinned)
    {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        pinButton->setText("Pin Window");
    }
    else
    {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        pinButton->setText("Unpin Window");
    }

    show();
    isPinned = !isPinned;
}

void MainWidget::highlightActiveApp(const std::string &appName)
{
    // Снимаем выделение со всех приложений
    for (auto it = activePrograms.begin(); it != activePrograms.end(); ++it)
    {
        QLabel *label = it.value().appLabel;
        label->setStyleSheet("font-size: 14px; font-weight: bold; color: #6cb4f0;");
    }

    if (activePrograms.contains(appName))
    {
        QLabel *appLabel = activePrograms[appName].appLabel;
        appLabel->setStyleSheet(
                "font-size: 14px; font-weight: bold; color: #ffffff; "
                "background-color: #2e86c1; padding: 5px; border: 2px solid #1f669a; border-radius: 5px;");
    }
}

void MainWidget::setupIconApp()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString iconPath = appPath + "/icon/icon.png";
    QIcon appIcon(iconPath);
    this->setWindowIcon(appIcon);
    QApplication::setWindowIcon(appIcon);
}

void MainWidget::setupTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(windowIcon());
    trayIcon->setToolTip(nameApp);

    trayMenu = new QMenu(this);
    restoreAction = new QAction("Открыть", this);
    quitAction = new QAction("Выход", this);

    trayMenu->addAction(restoreAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayMenu);

    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::Trigger)
        {
            this->showNormal();
        }
    });

    trayIcon->show();
}

void MainWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if (this->windowState() & Qt::WindowMinimized)
        {
            hide();
        }
    }
    QWidget::changeEvent(event);
}

