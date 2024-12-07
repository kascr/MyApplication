package com.kascr.myapplication.fragment

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import kotlinx.coroutines.*
import android.util.Log
import androidx.lifecycle.lifecycleScope
import com.kascr.myapplication.databinding.FragmentHomeBinding
import com.kascr.myapplication.R
import com.kascr.myapplication.base.BaseFragment


class HomeFragment : BaseFragment<FragmentHomeBinding>(R.layout.fragment_home) {

    //c++函数声明
    private external fun getMemoryUsage(): Int// 获取内存使用率
    private external fun getUsedMemory(): Double// 获取已使用内存
    private external fun getMaxMemory(): Double// 获取最大内存
    private external fun getSwapMemoryUsage(): Int// 获取交换内存使用率
    private external fun getMaxSwapMemory(): Double// 获取最大交换内存
    private external fun getUsedSwapMemory(): Double// 获取已使用交换内存
    private external fun getDataTotalSpace(): Double// 获取数据分区总空间
    private external fun getDataUsage(): Int// 获取数据分区已使用空间
    private external fun getDataUsedSpace(): Double// 获取数据分区已使用空间
    private external fun getGPUMhz(): Int// 获取 GPU 频率
    private external fun getGPULoad(): Int// 获取 GPU 负载
    private external fun getCpuCoreUsage(coreIndex: Int): Int
    private external fun getCpuCoreFrequencies(coreIndex: Int): IntArray






    private var jobList: List<Job> = listOf()// 协程任务列表

    companion object {
        init {
            System.loadLibrary("fragment-home")
        }
    }

    override fun createBinding(view: View): FragmentHomeBinding {//绑定视图
        return FragmentHomeBinding.bind(view)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        startUpdatingProgressBars()// 启动多个协程任务来分别更新进度条
        Log.d("cpu", "onViewCreated: ${getCpuCoreFrequencies(0)}")



    }

    // 启动多个协程任务来分别更新进度条
    private fun startUpdatingProgressBars() {
        val updateJobs = listOf(
            launchUpdateJob(::UpdateMemoryProgressBar, 1000),  // 每秒更新内存
            launchUpdateJob(::UpdateSwapProgressBar, 1000),    // 每2秒更新交换内存
            launchUpdateJob(::UpdateGpuProgressBar, 500),     // 每0.5秒更新GPU
            launchUpdateJob(::UpdateDataprogressBar, 5000),    // 每`5秒更新数据存储
        )

        jobList = updateJobs
    }


    // 启动协程来更新进度条，delay 可以设置不同的更新频率
    private fun launchUpdateJob(updateFunction: suspend () -> Unit, delayTime: Long): Job {
        return viewLifecycleOwner.lifecycleScope.launch {
            while (isActive) {
                withContext(Dispatchers.Main) {
                    updateFunction()
                }
                delay(delayTime) // 根据不同的任务设置不同的更新频率
            }
        }
    }

    // 取消所有的协程任务，避免内存泄漏
    override fun onDestroy() {
        super.onDestroy()
        // 取消所有协程任务
        jobList.forEach { it.cancel() }
    }






    //更新物理内存
    private fun UpdateMemoryProgressBar() {
        binding.tvPhysicalMemoryPercentage.text="${getMemoryUsage()}%"// 显示内存使用率
        binding.physicalMemoryProgress.setProgress(getMemoryUsage(), true)// 设置进度条进度
        binding.tvMemoryUsage.text = String.format("%.2f/%.2f GB", getUsedMemory().toFloat(), getMaxMemory().toFloat())// 显示已使用内存和最大内存
    }
    //更新交换内存
    private fun UpdateSwapProgressBar() {
        binding.tvSwapMemoryPercentage.text="${getSwapMemoryUsage()}%"// 显示交换内存使用率
        binding.swapMemoryProgress.setProgress(getSwapMemoryUsage(), true)// 设置进度条进度
        binding.tvSwapDetails.text = String.format("%.2f/%.2f GB", getUsedSwapMemory().toFloat(), getMaxSwapMemory().toFloat())// 显示已使用交换内存和最大交换内存

    }

    private fun UpdateGpuProgressBar() {
        val cpuUsage = getCpuCoreUsage(0)
        val cpuFrequencies = getCpuCoreFrequencies(0)
        binding.gpuProgress.setProgress(getGPULoad(), true)// 设置进度条进度
        binding.gpuProgressText.text="${getGPULoad()}%"// 显示 GPU 负载
        binding.gpuUsage.text = "${getGPUMhz()} MHz"// 显示 GPU 频率
        Log.d("CPU Info", "Core 0 Usage: $cpuUsage%")
        Log.d("CPU Info", "Core 0 Frequencies (MHz): Min = ${cpuFrequencies[0]}, Max = ${cpuFrequencies[1]}, Current = ${cpuFrequencies[2]}")
    }

    private fun UpdateCpuProgressBar() {
        // 假设这里是更新 CPU 使用率进度条
    }
    //更新数据存储
    private fun UpdateDataprogressBar() {
        binding.dataProgressText.text="${getDataUsage()}%"// 显示数据存储使用率
        binding.dataUsage.text =String.format("%.2f/%.2f GB", getDataUsedSpace().toFloat(), getDataTotalSpace().toFloat())// 显示已使用数据存储和最大数据存储
        binding.dataProgress.setProgress(getDataUsage(), true)// 设置进度条进度

    }

}

