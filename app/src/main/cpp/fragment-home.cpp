#include <jni.h>
#include <string>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cstring>
#include <iostream>
#include <sys/statvfs.h>
#include <android/log.h>
#include <vector>
#include <thread>
#include <chrono>
#include <numeric>



using namespace std;

// 解析 /proc/meminfo 文件并提取指定的内存信息
long getMemoryInfo(const string& key) {
    ifstream meminfoFile("/proc/meminfo");//打开文件
    if (!meminfoFile.is_open()) {//判断文件是否打开
        return -1; // 无法打开文件
    }

    string line;
    long value = 0;

    while (getline(meminfoFile, line)) {//读取文件
        istringstream iss(line);//解析文件
        string currentKey;//当前键
        string unit;//单位

        iss >> currentKey >> value >> unit;//解析文件

        if (currentKey == key) {//找到对应键的值并返回
            meminfoFile.close();//关闭文件
            return value; // 找到对应键的值并返回
        }//找到对应键的值并返回
    }

    meminfoFile.close();
    return -1; // 未找到对应的键
}
//获取内存使用率（单位：%）
extern "C" JNIEXPORT jint JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getMemoryUsage(JNIEnv* env, jobject) {
    (void)env;
    long totalMemory = getMemoryInfo("MemTotal:");//获取总内存
    long availableMemory = getMemoryInfo("MemAvailable:");//获取可用内存

    if (totalMemory == -1 || availableMemory == -1) {
        return -1; // 错误，无法读取内存信息
    }

    long usedMemory = totalMemory - availableMemory;
    int usagePercent = static_cast<int>((usedMemory * 100) / totalMemory);

    return usagePercent; // 返回内存使用百分比
}
//获取总内存
extern "C" JNIEXPORT jdouble JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getMaxMemory(JNIEnv* env, jobject) {//获取总内存
    (void)env;
    long totalMemory = getMemoryInfo("MemTotal:");//获取总内存

    if (totalMemory == -1) {//获取总内存
        return -1.0; // 错误，无法读取内存信息
    }

    // 将 KB 转换为 GB
    return static_cast<jdouble>(totalMemory) / (1024 * 1024); // 返回总内存（单位为 GB）
}
//获取已使用的内存（单位：GB）
extern "C" JNIEXPORT jdouble JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getUsedMemory(JNIEnv* env, jobject) {
    (void)env;
    long totalMemory = getMemoryInfo("MemTotal:");//获取总内存
    long availableMemory = getMemoryInfo("MemAvailable:");//获取可用内存

    if (totalMemory == -1 || availableMemory == -1) {
        return -1.0; // 错误，无法读取内存信息
    }

    long usedMemory = totalMemory - availableMemory;
    return static_cast<jdouble>(usedMemory) / (1024 * 1024); // 返回已使用的内存（单位为 GB）
}

//获取交换内存的使用率（单位：%）
extern "C" JNIEXPORT jint JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getSwapMemoryUsage(JNIEnv* env, jobject) {
    (void)env;
    long totalSwap = getMemoryInfo("SwapTotal:");
    long freeSwap = getMemoryInfo("SwapFree:");

    if (totalSwap == -1 || freeSwap == -1) {
        return -1;  // 无法获取交换内存信息
    }

    long usedSwap = totalSwap - freeSwap;
    return static_cast<int>((usedSwap * 100) / totalSwap);  // 返回交换内存使用率
}

// 获取交换内存的最大容量（单位：GB）
extern "C" JNIEXPORT jdouble JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getMaxSwapMemory(JNIEnv* env, jobject) {
    (void)env;
    long totalSwap = getMemoryInfo("SwapTotal:");
    if (totalSwap == -1) {
        return -1.0;  // 无法获取交换内存总容量
    }

    return static_cast<jdouble>(totalSwap) / 1024.0 / 1024.0;  // 将KB转换为GB
}

//获取已使用的交换内存（单位：MB）
extern "C" JNIEXPORT jdouble JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getUsedSwapMemory(JNIEnv* env, jobject) {
    (void)env;
    long totalSwap = getMemoryInfo("SwapTotal:");
    long freeSwap = getMemoryInfo("SwapFree:");

    if (totalSwap == -1 || freeSwap == -1) {
        return -1.0;  // 无法获取交换内存信息
    }

    long usedSwap = totalSwap - freeSwap;
    return static_cast<jdouble>(usedSwap) / 1024.0/1024.0;  // 将KB转换为GB
}

// 获取指定目录的总容量，返回单位为GB
double getTotalSpaceGB(const std::string& path) {
    struct statvfs stat{};

    // 获取文件系统信息
    if (statvfs(path.c_str(), &stat) != 0) {
        return -1.0; // 获取信息失败
    }

    // 文件系统的总容量，单位是字节
    int64_t totalSpaceBytes = static_cast<int64_t>(stat.f_frsize) * stat.f_blocks;

    // 转换为GB
    double totalSpaceGB = static_cast<double>(totalSpaceBytes) / (1024.0 * 1024.0 * 1024.0);
    return totalSpaceGB;
}
//获取目录的总容量
extern "C" JNIEXPORT jdouble JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getDataTotalSpace(JNIEnv* env, jobject) {
    (void)env; // 忽略未使用的参数

    // 获取 /data 目录的总容量
    double totalSpaceGB = getTotalSpaceGB("/data");

    if (totalSpaceGB == -1.0) {
        // 错误，无法获取文件系统信息，添加日志信息
        __android_log_print(ANDROID_LOG_ERROR, "JNI", "无法获取 /data 的文件系统统计信息");
        return -1.0;
    }

    // 返回总容量（单位：GB）
    return totalSpaceGB;
}

// 获取指定目录的已使用空间，返回单位为GB
double getUsedSpaceGB(const std::string& path) {
    struct statvfs stat{};

    // 获取文件系统信息
    if (statvfs(path.c_str(), &stat) != 0) {
        return -1.0; // 获取信息失败
    }

    // 文件系统的总容量和可用容量，单位是字节
    int64_t totalSpaceBytes = static_cast<int64_t>(stat.f_frsize) * stat.f_blocks;
    int64_t freeSpaceBytes = static_cast<int64_t>(stat.f_frsize) * stat.f_bfree;

    // 计算已使用空间
    int64_t usedSpaceBytes = totalSpaceBytes - freeSpaceBytes;

    // 转换为GB
    double usedSpaceGB = static_cast<double>(usedSpaceBytes) / (1024.0 * 1024.0 * 1024.0);
    return usedSpaceGB;
}
// 获取 /data 目录的已使用空间
extern "C" JNIEXPORT jdouble JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getDataUsedSpace(JNIEnv* env, jobject) {
    (void)env; // 忽略未使用的参数

    // 获取 /data 目录的已使用空间
    double usedSpaceGB = getUsedSpaceGB("/data");

    if (usedSpaceGB == -1.0) {
        // 错误，无法获取文件系统信息，添加日志信息
        __android_log_print(ANDROID_LOG_ERROR, "JNI", "无法获取 /data 的文件系统统计信息");
        return -1.0;
    }

    // 返回已使用空间（单位：GB）
    return usedSpaceGB;
}
//返回data目录的使用率
extern "C" JNIEXPORT jint JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getDataUsage(JNIEnv* env, jobject) {
    (void)env; // 忽略未使用的参数

    // 获取 /data 目录的总容量和已使用空间
    double totalSpaceGB = getTotalSpaceGB("/data");
    double usedSpaceGB = getUsedSpaceGB("/data");

    if (totalSpaceGB == -1.0 || usedSpaceGB == -1.0) {
        // 错误，无法获取文件系统信息，添加日志信息
        __android_log_print(ANDROID_LOG_ERROR, "JNI", "无法获取 /data 的文件系统统计信息");
        return -1;
    }

    // 计算使用率
    int usagePercent = static_cast<int>((usedSpaceGB * 100) / totalSpaceGB);
    return usagePercent;
}
//获取GPUMhz

extern "C" JNIEXPORT jint JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getGPUMhz(JNIEnv* env, jobject) {
    (void)env;
    // 通过 su 命令读取 GPU 频率
    FILE *fp = popen("su -c cat /sys/class/kgsl/kgsl-3d0/gpuclk", "r");
    if (fp == nullptr) {
        return -1; //报错，无法获取GPU频率
    }
    int gpuMhz = 0;
    fscanf(fp, "%d", &gpuMhz);
    pclose(fp);
    return gpuMhz / 1000000; // 返回GPU频率（单位：MHz）
}
//获取gpu负载
extern "C" JNIEXPORT jint JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getGPULoad(JNIEnv* env, jobject) {
    (void)env;

    // 通过 su 命令读取 GPU 负载，文件路径为 /sys/class/kgsl/kgsl-3d0/gpu_busy_percentage
    FILE *fp = popen("su -c cat /sys/class/kgsl/kgsl-3d0/gpu_busy_percentage", "r");

    if (fp == nullptr) {
        return -1; // 报错，无法获取GPU负载'

    }
    // 创建一个字符数组来存储读取的内容
    char gpuLoadStr[32];
    // 读取文件内容到字符数组
    if (fscanf(fp, "%s", gpuLoadStr) != 1) {
        pclose(fp);
        return -1; // 如果读取失败
    }
    pclose(fp);
    // 将字符串转换为整数
    int gpuLoad = atoi(gpuLoadStr);
    return gpuLoad; // 返回 GPU 负载（单位：%）
}
//获取gpu温度
extern "C" JNIEXPORT jdouble JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getGPUTemperature(JNIEnv* env, jobject) {
    (void)env;

    // 打开 shell 命令，获取 GPU 温度
    FILE* fp = popen("cat /sys/class/kgsl/kgsl-3d0/temp", "r");
    if (fp == nullptr) {
        return -1;  // 出错，无法获取温度
    }

    // 读取温度值
    int gpuTemp = 0;
    fscanf(fp, "%d", &gpuTemp);

    // 关闭文件流
    fclose(fp);

    return gpuTemp;  // 返回 GPU 温度（单位：摄氏度）
}



/**
 * 读取cpu文件的内容并返回当前最大核心频率，最小核心频率和当前核心频率
 */




// 读取cpu文件并返回频率值（单位：kHz），若失败返回-1
long readFrequencyFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return -1;
    std::string line;
    std::getline(file, line);
    return std::stol(line);  // 返回频率（单位：kHz）
}
#define TAG "CpuUsageMonitor"

std::vector<std::string> extractValues(const std::string& input) {
    std::istringstream ss(input);
    std::vector<std::string> values{std::istream_iterator<std::string>{ss}, std::istream_iterator<std::string>{}};
    values.erase(values.begin());  // 移除 "cpuX" 部分
    return values;
}

long sumValues(const std::vector<std::string>& values) {
    return std::accumulate(values.begin(), values.end(), 0L, [](long sum, const std::string& val) {
        return sum + std::stol(val);
    });
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getCpuCoreLoad(JNIEnv *env, jobject, jint coreIndex) {
    FILE* file = popen("su -c cat /proc/stat", "r");
    if (!file) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "错误：无法使用 root 权限打开 /proc/stat 文件。");
        return -1;  // 返回 -1 并打印错误提示
    }

    long Totaljiffies[2] = {0};
    long totalIdle[2] = {0};
    double rate = 0.0;

    for (int i = 0; i < 2; ++i) {
        char line[256];
        bool found = false;

        while (fgets(line, sizeof(line), file)) {
            std::string lineStr(line);
            if (lineStr.rfind("cpu" + std::to_string(coreIndex), 0) == 0) {
                found = true;
                auto values = extractValues(lineStr);

                // 打印解析后的值，帮助调试
                __android_log_print(ANDROID_LOG_DEBUG, TAG, "提取的 cpu%d 值：%s", coreIndex, lineStr.c_str());

                if (values.size() < 4) {
                    __android_log_print(ANDROID_LOG_ERROR, TAG, "错误：/proc/stat 中的 cpu%d 数据无效", coreIndex);
                    pclose(file);
                    return -1;  // 如果解析的数据不完整，返回 -1
                }

                Totaljiffies[i] = sumValues(values);
                totalIdle[i] = std::stol(values[3]);  // 空闲时间
            }
        }

        if (!found) {
            __android_log_print(ANDROID_LOG_ERROR, TAG, "错误：在 /proc/stat 中找不到 cpu%d 核心。", coreIndex);
            pclose(file);
            return -1;  // 如果没有找到指定的 CPU 核心，返回 -1
        }

        if (i == 0) std::this_thread::sleep_for(std::chrono::milliseconds(500));  // 等待系统更新

        rewind(file);  // 重置文件指针
    }

    pclose(file);

    if (Totaljiffies[1] > Totaljiffies[0]) {
        rate = static_cast<double>((Totaljiffies[1] - totalIdle[1]) - (Totaljiffies[0] - totalIdle[0])) /
               (Totaljiffies[1] - Totaljiffies[0]);
    }

    __android_log_print(ANDROID_LOG_DEBUG, TAG, "CPU 核心 %d 的负载: %.2f%%", coreIndex, rate * 100);

    if (rate < 0) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "错误：计算 CPU 使用率时出错。");
        return -1;  // 如果计算出的 CPU 使用率小于 0，返回 -1
    }

    return static_cast<jint>(rate * 100);  // 返回百分比
}
// 获取指定核心的最小频率、最大频率和当前频率（单位：MHz）
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_kascr_myapplication_fragment_HomeFragment_getCpuCoreFrequencies(JNIEnv *env, jobject, jint coreIndex) {
    jintArray result = env->NewIntArray(3);
    if (result == nullptr) return nullptr;  // 内存分配失败

    std::string basePath = "/sys/devices/system/cpu/cpu" + std::to_string(coreIndex) + "/cpufreq/";
    long minFreq = readFrequencyFromFile(basePath + "scaling_min_freq") / 1000;
    long maxFreq = readFrequencyFromFile(basePath + "scaling_max_freq") / 1000;
    long currentFreq = readFrequencyFromFile(basePath + "scaling_cur_freq") / 1000;

    jint freqArray[3] = {static_cast<jint>(minFreq), static_cast<jint>(maxFreq), static_cast<jint>(currentFreq)};
    env->SetIntArrayRegion(result, 0, 3, freqArray);

    return result;
}
