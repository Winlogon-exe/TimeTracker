#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    connectSignals();
}

MainWidget::~MainWidget()
{
}

void MainWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Active Applications:", this);
    mainLayout->addWidget(titleLabel);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *scrollWidget = new QWidget(scrollArea);
    scrollArea->setWidget(scrollWidget);

    appListLayout = new QVBoxLayout(scrollWidget);
    appListLayout->setContentsMargins(0, 0, 0, 0);
    appListLayout->setSpacing(10);

    mainLayout->addWidget(scrollArea, 1); // Вес = 1 (занимает доступное пространство)

    pinButton = new QPushButton("Pin Window", this);
    mainLayout->addWidget(pinButton, 0, Qt::AlignBottom); // Вес = 0 (фиксированный размер)

    isPinned = false;
    connect(pinButton, &QPushButton::clicked, this, &MainWidget::togglePinWindow);
}

void MainWidget::connectSignals()
{
    connect(&logic, &LogicMainWidget::updateUI, this, &MainWidget::activeAppUpdate);
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    // Логика при закрытии окна
    event->accept();
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void MainWidget::activeAppUpdate(const std::string &appName, const QPixmap &icon, int hours, int minutes, int seconds)
{
    if (activeProgram.find(appName) == activeProgram.end())
    {
        QHBoxLayout *rowLayout = new QHBoxLayout();

        QLabel *appLabel = new QLabel(QString::fromStdString(appName), this);
        appLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2E86C1;");
        rowLayout->addWidget(appLabel, 2);

        QLabel *iconLabel = new QLabel(this);
        iconLabel->setPixmap(icon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        rowLayout->addWidget(iconLabel, 1);

        QLabel *timeLabel = new QLabel(this);
        timeLabel->setText(QString("%1:%2:%3")
                                   .arg(hours, 2, 10, QChar('0'))
                                   .arg(minutes, 2, 10, QChar('0'))
                                   .arg(seconds, 2, 10, QChar('0')));
        timeLabel->setStyleSheet("font-size: 12px; color: #34495E;");
        timeLabel->setAlignment(Qt::AlignCenter);
        rowLayout->addWidget(timeLabel, 1);

        appListLayout->addLayout(rowLayout);

        activeProgram[appName] = QIcon(icon);
        appLabels[appName] = appLabel;
        iconLabels[appName] = iconLabel;
        timeLabels[appName] = timeLabel;
    }
    else
    {
        QLabel *timeLabel = timeLabels[appName];
        QLabel *iconLabel = iconLabels[appName];

        iconLabel->setPixmap(icon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        timeLabel->setText(QString("%1:%2:%3")
                                   .arg(hours, 2, 10, QChar('0'))
                                   .arg(minutes, 2, 10, QChar('0'))
                                   .arg(seconds, 2, 10, QChar('0')));
    }
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
