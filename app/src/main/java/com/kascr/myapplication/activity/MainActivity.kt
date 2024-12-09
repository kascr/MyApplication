package com.kascr.myapplication.activity

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.activity.enableEdgeToEdge
import androidx.fragment.app.Fragment
import com.google.android.material.bottomnavigation.BottomNavigationView
import com.kascr.myapplication.R
import com.kascr.myapplication.fragment.AboutFragment
import com.kascr.myapplication.fragment.HomeFragment
import com.kascr.myapplication.fragment.SetFragment
import com.kascr.myapplication.databinding.ActivityMainBinding
import com.kascr.myapplication.base.BaseActivity



class MainActivity : BaseActivity<ActivityMainBinding>() {
    private var currentFragment: Fragment? = null
    override fun getViewBinding(): ActivityMainBinding {
        return ActivityMainBinding.inflate(layoutInflater)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        // 启用边缘到边缘显示
        enableEdgeToEdge()

        // 初始化底部导航栏
        initializeBottomNavigation()

        // 恢复Fragment或加载默认Fragment
        initializeFragment(savedInstanceState)

    }






    private fun initializeBottomNavigation() {
        val navigation: BottomNavigationView = findViewById(R.id.navigation)
        navigation.selectedItemId = R.id.page_2 // 设置默认选中的菜单项
        navigation.setOnItemSelectedListener { item ->
            // 根据点击的菜单项切换Fragment
            val targetFragment = when (item.itemId) {
                R.id.page_1 -> SetFragment()
                R.id.page_2 -> HomeFragment()
                R.id.page_3 -> AboutFragment()
                else -> null
            }

            targetFragment?.let {
                // 如果当前Fragment与目标Fragment不一样，切换Fragment
                if (it::class.java != currentFragment?.javaClass) {
                    switchFragment(it)
                }
            }
            true
        }
    }

    private fun initializeFragment(savedInstanceState: Bundle?) {
        if (savedInstanceState != null) {
            // 恢复保存的Fragment状态
            currentFragment = supportFragmentManager.getFragment(savedInstanceState, "currentFragment")
        } else {
            // 初次启动时加载默认Fragment
            currentFragment = HomeFragment()
            supportFragmentManager.beginTransaction()
                .add(R.id.fragment_home, currentFragment!!, currentFragment!!::class.java.simpleName)
                .commitAllowingStateLoss()
        }
    }

    private fun switchFragment(targetFragment: Fragment) {
        val transaction = supportFragmentManager.beginTransaction()

        // 如果当前Fragment不为空，移除当前Fragment
        currentFragment?.let {
            transaction.remove(it) // 使用 remove 而不是 hide
        }

        // 添加或显示目标Fragment
        if (!targetFragment.isAdded) {
            transaction.add(R.id.fragment_home, targetFragment, targetFragment.javaClass.simpleName)
        } else {
            transaction.show(targetFragment)
        }

        currentFragment = targetFragment

        // 提交事务
        transaction.commitAllowingStateLoss()
    }

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)
        currentFragment?.let {
            supportFragmentManager.putFragment(outState, "currentFragment", it)
        }
    }




}
