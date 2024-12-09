package com.kascr.myapplication.function

import java.io.File

class CPUZ {
    fun getCpuLoad(cpuId: Int): Float? {
        if (cpuId < 0 || cpuId > 7) {
            throw IllegalArgumentException("CPU id must be between 0 and 7")
        }

        val procStat = File("/proc/stat").readLines()

        // 获取指定 CPU 核心的负载信息
        val line = procStat.firstOrNull { it.startsWith("cpu$cpuId") }
        line?.let {
            val cpuStats = line.split(" ").filter { it.isNotEmpty() }// 拆分 CPU 核心信息
            // CPU 的时间信息: user, nice, system, idle, iowait, irq, softirq, steal
            val user = cpuStats[1].toLong()
            val nice = cpuStats[2].toLong()
            val system = cpuStats[3].toLong()
            val idle = cpuStats[4].toLong()

            // 计算总时间和使用时间
            val total = user + nice + system + idle
            val used = user + nice + system

            // 计算负载百分比
            return if (total == 0L) 0f else (used.toFloat() / total.toFloat()) * 100
        }
        return null
    }
}