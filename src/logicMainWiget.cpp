#include "LogicMainWidget.h"

LogicMainWidget::LogicMainWidget()
{
}

void LogicMainWidget::startTimer()
{
    mainTimer = new QTimer(this);
    connect(mainTimer, &QTimer::timeout, this, &LogicMainWidget::trackActiveApplication);
    mainTimer->start(1001);
    currentAppPID = GetCurrentProcessId();
}

void LogicMainWidget::trackActiveApplication()
{
    DWORD pid = getFocusedApplicationPID();

    if (pid == currentAppPID) {
        qDebug() << "Skipping tracking for current application.";
        return;
    }

    startTrackingForNewApplication(pid);
    updateTrackingForActiveApplications(pid);
    printApplicationTime(pid);
    updateApplicationIcon(pid);
}

void LogicMainWidget::startTrackingForNewApplication(DWORD pid)
{
    if (activeTimers.find(pid) == activeTimers.end())
    {
        activeTimers[pid] = new QElapsedTimer();
        activeTimers[pid]->start();
        accumulatedTime[pid] = 0;
        qDebug() << "Started tracking application with PID:" << pid;
    }
}

void LogicMainWidget::updateTrackingForActiveApplications(DWORD pid)
{
    for (auto& pair : activeTimers)
    {
        DWORD trackedPID = pair.first;
        QElapsedTimer* timer = pair.second;

        if (trackedPID == pid)
        {
            if (timer->isValid())
            {
                int elapsed = timer->elapsed() / 1000;
                accumulatedTime[pid] += elapsed;
                timer->restart();
            }
            else
            {
                timer->start();
            }
        }
        else if (timer->isValid())
        {
            pauseTimer(trackedPID, timer);
        }
    }
}

void LogicMainWidget::printApplicationTime(DWORD pid)
{
    int totalElapsedTime = accumulatedTime[pid];
    int hours = totalElapsedTime / 3600;
    int minutes = (totalElapsedTime % 3600) / 60;
    int seconds = totalElapsedTime % 60;

    std::string appName = getProcessNameByPid(pid);
    qDebug() << "Active application:" << QString::fromStdString(appName)
             << ", Time:" << hours << "h" << minutes << "m" << seconds << "s";
}

void LogicMainWidget::updateApplicationIcon(DWORD pid)
{
    if (iconCache.find(pid) == iconCache.end())
    {
        int hours = accumulatedTime[pid] / 3600;
        int minutes = (accumulatedTime[pid] % 3600) / 60;
        int seconds = accumulatedTime[pid] % 60;
        iconCache[pid] = getIconForProcess(pid);
        emit updateUI(getProcessNameByPid(pid), iconCache[pid], hours, minutes, seconds);
    }
    else
    {
        int hours = accumulatedTime[pid] / 3600;
        int minutes = (accumulatedTime[pid] % 3600) / 60;
        int seconds = accumulatedTime[pid] % 60;
        emit updateUI(getProcessNameByPid(pid), iconCache[pid], hours,minutes, seconds);
    }
}

void LogicMainWidget::pauseTimer(DWORD pid, QElapsedTimer* timer)
{
    if (activeTimers.find(pid) != activeTimers.end() && timer->isValid())
    {
        int elapsed = timer->elapsed() / 1000;
        accumulatedTime[pid] += elapsed;
        timer->invalidate();
        qDebug() << "Paused Timer for PID:" << pid
                 << ", Elapsed:" << elapsed
                 << ", Total Accumulated Time:" << accumulatedTime[pid] << "seconds";
    }
}

DWORD LogicMainWidget::getFocusedApplicationPID()
{
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL)
    {
        return 0;
    }

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    return pid;
}

QPixmap LogicMainWidget::getIconForProcess(DWORD pid)
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
                    DestroyIcon(hIcon);
                    CloseHandle(hProcess);
                    return pixmap;
                }
            }
        }
        CloseHandle(hProcess);
    }
    return QPixmap();
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



