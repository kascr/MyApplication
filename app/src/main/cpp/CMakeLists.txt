cmake_minimum_required(VERSION 3.4.1)
project(MyApplication)
# 创建共享库
add_library(fragment-home SHARED fragment-home.cpp)

# 查找并链接 log 库
find_library(log-lib log)

# 链接 log 库
target_link_libraries(fragment-home ${log-lib})
