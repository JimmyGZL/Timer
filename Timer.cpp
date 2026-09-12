#include <chrono>
#include <conio.h>   // _kbhit, _getch（Windows 专用）
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

class Timer {
public:
    Timer() : totalSeconds_(0), remainingSeconds_(0), paused_(false), running_(false) {}

    void setDuration(int hours, int minutes, int seconds) {
        totalSeconds_ = hours * 3600 + minutes * 60 + seconds;
        remainingSeconds_ = totalSeconds_;
        paused_ = false;
        running_ = false;
    }

    void start() {
        if (totalSeconds_ <= 0) {
            std::cout << "错误：请先设置一个大于 0 的时长。\n";
            return;
        }
        running_ = true;
        paused_ = false;

        std::chrono::steady_clock::time_point lastTick = std::chrono::steady_clock::now();

        while (remainingSeconds_ > 0 && running_) {
            // 处理键盘输入（非阻塞）
            while (_kbhit()) {
                char ch = _getch();
                if (ch == 'p' || ch == 'P') { paused_ = true;  std::cout << "\n[已暂停]\n"; }
                if (ch == 'r' || ch == 'R') { paused_ = false; std::cout << "\n[已继续]\n"; }
                if (ch == 'q' || ch == 'Q') { running_ = false; std::cout << "\n[已退出]\n"; }
            }

            if (!running_) break;

            if (!paused_) {
                std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                std::chrono::milliseconds elapsed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);

                if (elapsed.count() >= 1000) {
                    lastTick += std::chrono::seconds(1);
                    --remainingSeconds_;
                    printRemaining();
                }
            } else {
                // 暂停时也要更新时间基准，避免继续后瞬间扣秒
                lastTick = std::chrono::steady_clock::now();
            }

            // 短睡眠，避免占满 CPU（10ms）
            std::chrono::milliseconds sleepTime(10);
            std::chrono::steady_clock::time_point wake =
                std::chrono::steady_clock::now() + sleepTime;
            while (std::chrono::steady_clock::now() < wake) { /* busy wait */ }
        }

        if (remainingSeconds_ == 0) {
            printRemaining();
            std::cout << "\n时间到！\n";
        }
        running_ = false;
    }

    void reset() {
        remainingSeconds_ = totalSeconds_;
        paused_ = false;
        running_ = false;
    }

    bool isRunning() const { return running_; }
    int  remaining() const { return remainingSeconds_; }

private:
    int  totalSeconds_;
    int  remainingSeconds_;
    bool paused_;
    bool running_;

    void printRemaining() const {
        int h = remainingSeconds_ / 3600;
        int m = (remainingSeconds_ % 3600) / 60;
        int s = remainingSeconds_ % 60;
        std::cout << "\r剩余时间："
                  << std::setfill('0') << std::setw(2) << h << ":"
                  << std::setfill('0') << std::setw(2) << m << ":"
                  << std::setfill('0') << std::setw(2) << s << std::flush;
    }
};

int readNonNegativeInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= 0) {
            return value;
        }
        std::cout << "输入无效，请输入一个非负整数。\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main() {
    Timer timer;

    std::cout << "===== 简单计时器 =====\n";
    std::cout << "请输入倒计时时长：\n";
    int hours   = readNonNegativeInt("  小时：");
    int minutes = readNonNegativeInt("  分钟：");
    int seconds = readNonNegativeInt("  秒数：");

    timer.setDuration(hours, minutes, seconds);

    if (timer.remaining() == 0) {
        std::cout << "时长为 0，程序退出。\n";
        return 0;
    }

    std::cout << "\n操作说明：p=暂停  r=继续  q=退出\n";
    std::cout << "按任意键开始...\n";
    _getch();

    timer.start();
    return 0;
}
