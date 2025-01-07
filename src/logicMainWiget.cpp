#include "LogicMainWidget.h"

LogicMainWidget::LogicMainWidget()
{

}

void LogicMainWidget::startTimer()
{
    mainTimer = new QTimer(this);
    connect(mainTimer, &QTimer::timeout, this, &LogicMainWidget::trackActiveApplication);
    mainTimer->start(1000);
    currentAppPID = GetCurrentProcessId();
}

void LogicMainWidget::trackActiveApplication()
{
    DWORD pid = getFocusedApplicationPID();

    if (pid == currentAppPID) {
        qDebug() << "Skipping tracking for current application.";
        return;
    }

    if (activeTimers.find(pid) == activeTimers.end())
    {
        activeTimers[pid] = new QElapsedTimer();
        activeTimers[pid]->start();
        accumulatedTime[pid] = 0;
        qDebug() << "Started tracking application with PID:" << pid;
    }

    for (auto& pair : activeTimers)
    {
        DWORD trackedPID = pair.first;
        QElapsedTimer* timer = pair.second;

        if (trackedPID == pid)
        {
            // Активное приложение:
            if (timer->isValid())
            {
                int elapsed = timer->elapsed() / 1000;
                accumulatedTime[pid] += elapsed;
                timer->restart();
            }
        }
        else
        {
            // Неактивное приложение:
            if (timer->isValid())
            {
                pauseTimer(trackedPID, timer);
            }
        }
    }

    int totalElapsedTime = accumulatedTime[pid];
    int hours = totalElapsedTime / 3600;
    int minutes = (totalElapsedTime % 3600) / 60;
    int seconds = totalElapsedTime % 60;

    std::string appName = getProcessNameByPid(pid);
    qDebug() << "Active application:" << QString::fromStdString(appName)
             << ", Time:" << hours << "h" << minutes << "m" << seconds << "s";

    getIconForProcess(pid, hours, minutes, seconds);
}

void LogicMainWidget::pauseTimer(DWORD pid, QElapsedTimer* timer)
{
    if (activeTimers.find(pid) != activeTimers.end() && timer->isValid()) {

        accumulatedTime[pid] += timer->elapsed() / 1000;
        timer->invalidate();
        qDebug() << "Paused Timer for PID:" << pid
                 << ", Accumulated Time:" << accumulatedTime[pid] << "seconds";
    }
}

DWORD LogicMainWidget::getFocusedApplicationPID()
{
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) {
        return 0;
    }

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    return pid;
}

void LogicMainWidget::getIconForProcess(DWORD pid, int hours, int minutes, int seconds)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess != NULL)
    {
        HMODULE hMod;
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
        {
            char filePath[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, hMod, filePath, sizeof(filePath)))
            {
                HICON hIcon = ExtractIconA(NULL, filePath, 0);
                if (hIcon != NULL)
                {
                    QPixmap pixmap = QPixmap::fromImage(QImage::fromHICON(hIcon));
                    std::string appName = getProcessNameByPid(pid);
                    std::string suffix = ".exe";
                    appName = appName.substr(0, appName.size() - suffix.size());
                    emit updateUI(appName, pixmap, hours, minutes, seconds);
                }
                else
                {
                    qDebug() << "Empty Icon";
                    DestroyIcon(hIcon);
                }
            }
        }
        CloseHandle(hProcess);
    }
}

std::string LogicMainWidget::getProcessNameByPid(DWORD pid)
{
    char szProcessName[MAX_PATH] = "<unknown>";
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess != NULL)
    {
        HMODULE hMod;
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
        {
            GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(char));
        }
        CloseHandle(hProcess);
    }
    return std::string(szProcessName);
}

LogicMainWidget::~LogicMainWidget()
{
    for (auto& pair : activeTimers)
    {
        delete pair.second;
    }
}



