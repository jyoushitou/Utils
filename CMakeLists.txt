# 要求 CMake 最低版本为 3.15，保证所用特性可用
cmake_minimum_required(VERSION 3.15)

# 定义项目名称 Utils
project(Utils
    VERSION 1.0.0                                              # 项目版本号
    DESCRIPTION "I/O、时间与优雅退出的工具库"                    # 项目描述
    HOMEPAGE_URL ""                                            # 项目主页（暂空）
    LANGUAGES CXX                                              # 仅启用 C++ 语言
)

# ----------------------------------------------------------------------------
# 全局设置
# ----------------------------------------------------------------------------
# 判断 BUILD_SHARED_LIBS 是否已被定义
if(NOT DEFINED BUILD_SHARED_LIBS)
    # 未定义时默认关闭，即构建静态库
    set(BUILD_SHARED_LIBS OFF)
# 结束 if 判断
endif()

# 判断当前源码目录是否等于顶层源码目录
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # 相等则说明是顶层构建，置 ON
    set(Utils_IS_TOP_LEVEL ON)
# 否则不是顶层构建
else()
    # 置 OFF
    set(Utils_IS_TOP_LEVEL OFF)
# 结束 if 判断
endif()

# 生成 compile_commands.json，便于 clangd 等工具使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------------------------------------------------------
# 语言标准 / 编译选项
# ----------------------------------------------------------------------------
# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
# 强制要求编译器支持所设定标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 关闭编译器扩展，使用纯标准语法
set(CMAKE_CXX_EXTENSIONS OFF)

# 判断编译器是否为 MSVC
if(MSVC)
    # MSVC：开启较严格警告、符合标准模式、按 UTF-8 处理源码
    add_compile_options(/W4 /permissive- /utf-8)
    # 定义 MSVC 需要的若干宏
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS                                # 关闭 CRT 安全警告
        NOMINMAX                                               # 禁止 windows.h 定义 min/max 宏
        WIN32_LEAN_AND_MEAN                                    # 精简 windows.h 头文件
    )
# 非 MSVC（GCC/Clang 等）
else()
    # 开启常见警告：全部、额外、严格标准
    add_compile_options(-Wall -Wextra -Wpedantic)
# 结束 if 判断
endif()

# ----------------------------------------------------------------------------
# 源文件
# ----------------------------------------------------------------------------
# 声明对外公开头文件列表变量
set(Utils_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Utils.h                 # 公开头文件 Utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Message.h               # 公开头文件 Message.h
)

# 递归收集 source/ 下的实现文件，CONFIGURE_DEPENDS 保证新增文件自动生效
file(GLOB_RECURSE Utils_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp                    # 匹配 .cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cc                     # 匹配 .cc
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cxx                    # 匹配 .cxx
)

# ----------------------------------------------------------------------------
# 库目标
# ----------------------------------------------------------------------------
# 创建库目标（不带关键字，遵循 BUILD_SHARED_LIBS，默认静态库）
add_library(Utils ${Utils_SOURCES} ${Utils_PUBLIC_HEADERS})

# 创建命名空间别名 Utils::Utils
add_library(Utils::Utils ALIAS Utils)

# 设置包含目录
target_include_directories(Utils
    # PUBLIC：自身与使用方均可见
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  # 构建期头文件路径
        $<INSTALL_INTERFACE:include>                            # 安装后头文件路径
)

# 查找线程库（Utils.h 中用到 std::thread/std::mutex 等）
find_package(Threads REQUIRED)
# 以 PUBLIC 方式链接线程库
target_link_libraries(Utils PUBLIC Threads::Threads)

# ----------------------------------------------------------------------------
# 安装 / 导出
# ----------------------------------------------------------------------------
# 引入 GNU 目录变量（lib、include 等标准路径）
include(GNUInstallDirs)
# 引入包配置辅助模块
include(CMakePackageConfigHelpers)

# 安装库目标
install(TARGETS Utils
    EXPORT  UtilsTargets                                       # 导出目标集合名
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}                # 静态库安装路径
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}                # 动态库安装路径
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}                # 可执行/运行库安装路径
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}           # 附带包含目录
)

# 安装公开头文件
install(FILES ${Utils_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}                    # 头文件安装路径
)

# 导出目标文件，供 find_package 使用
install(EXPORT UtilsTargets
    FILE      UtilsTargets.cmake                               # 导出的文件名
    NAMESPACE Utils::                                          # 导出目标命名空间
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Utils            # 导出文件安装路径
)

# 由模板生成包配置文件
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/UtilsConfig.cmake.in     # 模板输入文件
    ${CMAKE_CURRENT_BINARY_DIR}/UtilsConfig.cmake              # 生成的输出文件
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Utils    # 安装目标路径
)

# 生成包版本文件
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/UtilsConfigVersion.cmake       # 版本文件输出路径
    VERSION ${PROJECT_VERSION}                                 # 使用项目版本号
    COMPATIBILITY SameMajorVersion                             # 主版本兼容策略
)

# 安装包配置与版本文件
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/UtilsConfig.cmake              # 包配置文件
    ${CMAKE_CURRENT_BINARY_DIR}/UtilsConfigVersion.cmake       # 包版本文件
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Utils            # 安装目标路径
)

# ----------------------------------------------------------------------------
# 测试（可选）
# ----------------------------------------------------------------------------
# 定义是否构建测试的选项，默认关闭
option(Utils_BUILD_TESTS "是否构建 Utils 单元测试" OFF)
# 仅在开启测试且为顶层构建时加入测试子目录
if(Utils_BUILD_TESTS AND Utils_IS_TOP_LEVEL)
    # 启用 CTest 测试
    enable_testing()
    # 加入 tests 子目录
    add_subdirectory(tests)
# 结束 if 判断
endif()