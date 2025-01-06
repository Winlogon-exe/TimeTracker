#include "LogicMainWidget.h"

LogicMainWidget::LogicMainWidget()
{
    QTimer *mainTimer = new QTimer(this);
    connect(mainTimer, &QTimer::timeout, this, &LogicMainWidget::trackActiveApplication);
    mainTimer->start(1000);  // Проверка каждую секунду
}

LogicMainWidget::~LogicMainWidget()
{
}

void LogicMainWidget::trackActiveApplication()
{
    DWORD pid = getFocusedApplicationPID();  // Получаем PID активного приложения

    if (pid == 0) {
        qDebug() << "No active window";
        return;
    }

    // Если приложение отслеживается в первый раз
    if (activeTimers.find(pid) == activeTimers.end())
    {
        activeTimers[pid] = std::chrono::steady_clock::now();  // Запоминаем время активации
        accumulatedTime[pid] = 0;  // Сбрасываем накопленное время
    }
    else
    {
        // Если приложение уже было
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - activeTimers[pid]).count();

        // Если приложение вернулось в фокус
        if (elapsedTime > 0) {
            accumulatedTime[pid] += elapsedTime;
        }

        activeTimers[pid] = currentTime;

        int totalElapsedTime = accumulatedTime[pid];

        int hours = totalElapsedTime / 3600;
        int minutes = (totalElapsedTime % 3600) / 60;
        int seconds = totalElapsedTime % 60;
        std::string appName = getProcessNameByPid(pid);

        qDebug() << "Active application:" << QString::fromStdString(appName)
                 << ", Time:" << hours << "h" << minutes << "m" << seconds << "s";
        getIconForProcess(pid, hours, minutes, seconds);
    }
}


void LogicMainWidget::stopTrackingApplication(DWORD pid) {

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
            // Путь к исполнимому файлу процесса
            char filePath[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, hMod, filePath, sizeof(filePath)))
            {
                HICON hIcon = ExtractIconA(NULL, filePath, 0);
                if (hIcon != NULL)
                {
                    QPixmap pixmap = QPixmap::fromImage(QImage::fromHICON(hIcon));
                    std::string appName = getProcessNameByPid(pid);
                    emit updateUI(appName, pixmap, hours, minutes, seconds);
                }
                else
                {
                    qDebug() << "Empty Icon";
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
