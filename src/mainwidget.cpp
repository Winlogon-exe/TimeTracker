#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent): QWidget(parent)
{
    setupUI();
    connectSignals();
}

MainWidget::~MainWidget()
{
}

void MainWidget::setupUI()
{
    layout = new QVBoxLayout(this);
    setFixedSize(300, 200);
    layout->addWidget(new QLabel("Active Applications:"));

    // Кнопка для закрепления окна поверх других
    pinButton = new QPushButton("Pin Window", this);
    layout->addWidget(pinButton);

    // Изначально окно не закреплено
    isPinned = false;
    connect(pinButton, &QPushButton::clicked, this, &MainWidget::togglePinWindow);
}

void MainWidget::connectSignals()
{
    connect(&logic,&LogicMainWidget::updateUI,this,&MainWidget::activeAppUpdate);
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
    // Проверяем, если приложение уже в списке
    if (activeProgram.find(appName) == activeProgram.end()) {
        // Если приложение не найдено, добавляем его

        // Название приложения
        QLabel *appLabel = new QLabel(QString::fromStdString(appName), this);
        layout->addWidget(appLabel);

        // Иконка приложения
        QLabel *iconLabel = new QLabel(this);
        iconLabel->setPixmap(icon.scaled(32, 32)); // Иконка
        layout->addWidget(iconLabel);

        // Время
        QLabel *timeLabel = new QLabel(this);
        timeLabel->setText(QString("%1:%2:%3")
                                   .arg(hours, 2, 10, QChar('0'))
                                   .arg(minutes, 2, 10, QChar('0'))
                                   .arg(seconds, 2, 10, QChar('0')));
        layout->addWidget(timeLabel);

        // Сохраняем указатели на метки для обновления в дальнейшем
        activeProgram[appName] = QIcon(icon);
        appLabels[appName] = appLabel;
        iconLabels[appName] = iconLabel;
        timeLabels[appName] = timeLabel;
    } else {
        // Если приложение уже отслеживается, обновляем только время и иконку
        QLabel *timeLabel = timeLabels[appName];
        QLabel *iconLabel = iconLabels[appName];

        // Обновляем иконку
        iconLabel->setPixmap(icon.scaled(32, 32));

        // Обновляем время
        timeLabel->setText(QString("%1:%2:%3")
                                   .arg(hours, 2, 10, QChar('0'))
                                   .arg(minutes, 2, 10, QChar('0'))
                                   .arg(seconds, 2, 10, QChar('0')));
    }
}


void MainWidget::togglePinWindow()
{
    // Переключаем режим закрепления окна
    if (isPinned)
    {
        // Убираем флаг закрепления
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        pinButton->setText("Pin Window");
    }
    else
    {
        // Устанавливаем флаг закрепления
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        pinButton->setText("Unpin Window");
    }

    // Применяем измененные флаги и обновляем окно
    show(); // Это необходимо, чтобы изменения флагов окна вступили в силу
    isPinned = !isPinned;
}







