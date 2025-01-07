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
    this->resize(400, 300);
    this->setMinimumSize(200, 200);
    this->setStyleSheet("background-color: #1e1e1e; color: #ffffff;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Заголовок
    QLabel *titleLabel = new QLabel("Active Applications", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #f0f0f0; padding: 10px;");
    mainLayout->addWidget(titleLabel);

    // Разделитель
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("color: #2c2c2c; margin: 0 10px;");
    mainLayout->addWidget(separator);

    // Прокручиваемая область
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("border: none; background-color: #252525;");

    QWidget *scrollWidget = new QWidget(scrollArea);
    scrollArea->setWidget(scrollWidget);

    appListLayout = new QVBoxLayout(scrollWidget);
    appListLayout->setContentsMargins(10, 10, 10, 10);
    appListLayout->setSpacing(10);

    mainLayout->addWidget(scrollArea, 1);

    pinButton = new QPushButton("Pin Window", this);
    pinButton->setStyleSheet(
            "QPushButton {"
            "  font-size: 14px; font-weight: bold; color: #ffffff; background-color: #2e86c1;"
            "  border: none; padding: 8px 15px; border-radius: 5px;"
            "}"
            "QPushButton:hover { background-color: #1f669a; }"
            "QPushButton:pressed { background-color: #14496d; }");
    mainLayout->addWidget(pinButton, 0, Qt::AlignCenter);

    isPinned = false;
    connect(pinButton, &QPushButton::clicked, this, &MainWidget::togglePinWindow);
}

void MainWidget::connectSignals()
{
    connect(&logic, &LogicMainWidget::updateUI, this, &MainWidget::activeAppUpdate);
}

void MainWidget::activeAppUpdate(const std::string &appName, const QPixmap &icon, int hours, int minutes, int seconds)
{
    if (activeProgram.find(appName) == activeProgram.end())
    {
        QHBoxLayout *rowLayout = new QHBoxLayout();

        // Иконка приложения
        QLabel *iconLabel = new QLabel(this);
        iconLabel->setPixmap(icon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        iconLabel->setMinimumSize(40, 40); // Устанавливаем минимальный размер
        iconLabel->setAlignment(Qt::AlignCenter);
        rowLayout->addWidget(iconLabel, 0);

        // Название приложения
        QLabel *appLabel = new QLabel(QString::fromStdString(appName), this);
        appLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #6cb4f0;");
        appLabel->setMinimumHeight(32); // Минимальная высота текста
        appLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Расширение текста
        rowLayout->addWidget(appLabel, 2);

        // Время
        QLabel *timeLabel = new QLabel(this);
        timeLabel->setText(QString("%1:%2:%3")
                                   .arg(hours, 2, 10, QChar('0'))
                                   .arg(minutes, 2, 10, QChar('0'))
                                   .arg(seconds, 2, 10, QChar('0')));
        timeLabel->setStyleSheet("font-size: 12px; color: #c4c4c4;");
        timeLabel->setAlignment(Qt::AlignCenter);
        timeLabel->setMinimumWidth(30); // Минимальная ширина для отображения времени
        rowLayout->addWidget(timeLabel, 0);

        appListLayout->addLayout(rowLayout);

        activeProgram[appName] = QIcon(icon);
        iconLabels[appName] = iconLabel;
        appLabels[appName] = appLabel;
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

void MainWidget::start()
{
    logic.startTimer();
}
