# 要求 CMake 最低版本为 3.16，保证所用特性可用
cmake_minimum_required(VERSION 3.16) # 版本低于此值时直接报错终止

# 定义项目名称 Utils
project(Utils # 项目名，决定 Utils_SOURCE_DIR 等变量
    VERSION 1.0.0 # 项目版本号
    DESCRIPTION "I/O、时间与优雅退出的工具库" # 项目描述
    HOMEPAGE_URL "" # 项目主页（暂空）
    LANGUAGES CXX # 仅启用 C++ 语言
)

# 引入 GNU 标准安装目录变量（CMAKE_INSTALL_LIBDIR / BINDIR / INCLUDEDIR 均由它提供）
include(GNUInstallDirs)

# 判断是否是独立构建
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR) # 顶层源目录等于当前源目录即为独立构建
    set(LIB_IS_TOP_LEVEL ON) # 独立构建：可开启测试、示例等自带目标
else()
    set(LIB_IS_TOP_LEVEL OFF) # 被父工程 add_subdirectory 引入：只出库本体
endif()

# 链接库类型(OFF为静态库，ON为动态库)
if(NOT DEFINED BUILD_SHARED_LIBS) # 未显式传入时给默认值，避免变量为空
    set(BUILD_SHARED_LIBS OFF) # 默认构建静态库
endif()

if(BUILD_SHARED_LIBS) # 依据开关决定最终库类型
    set(UTILS_LIBRARY_TYPE SHARED) # 动态库
else()
    set(UTILS_LIBRARY_TYPE STATIC) # 静态库
endif()

# 根据是否为独立构建
if(LIB_IS_TOP_LEVEL) # 仅独立构建时设置全局默认，避免污染父工程
    # 生成 compile_commands.json，clangd / IDE 靠它做补全
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 打开编译数据库导出

    # 单配置生成器（Ninja / Makefile）不指定就没有构建类型，这里统一兜底为 Release
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES) # 多配置生成器自带配置，无需在此兜底
        set(CMAKE_BUILD_TYPE Release CACHE STRING "构建类型" FORCE) # 写入缓存并强制覆盖为 Release
    endif()

    # 语言标准兜底；真正对外生效的要求写在下面 target_compile_features()
    if(NOT CMAKE_CXX_STANDARD) # 父工程已指定标准时尊重其设置
        set(CMAKE_CXX_STANDARD 17) # 兜底采用 C++17
        set(CMAKE_CXX_STANDARD_REQUIRED ON) # 不允许编译器回退到更低标准
        set(CMAKE_CXX_EXTENSIONS OFF) # 关闭 gnu++17 之类的编译器扩展
    endif()
endif()

# 公开头文件：参与 IDE 分组，同时用于安装
set(UTILS_PUBLIC_HEADERS # 头文件列表变量
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Message.h # 消息相关公开头
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Utils.h # 工具函数公开头
)

set(UTILS_SOURCES # 编译源文件列表变量
    ${CMAKE_CURRENT_SOURCE_DIR}/source/Utils.cpp # 库的唯一实现源文件
)

add_library(Utils # 定义库目标 Utils
    ${UTILS_LIBRARY_TYPE} # 库类型（STATIC / SHARED）
    ${UTILS_SOURCES} # 参与编译的源文件
    ${UTILS_PUBLIC_HEADERS} # 头文件不参与编译，只是让 IDE 显示出来
)

add_library(Utils::Utils ALIAS Utils) # 别名目标，方便使用方统一写 Utils::Utils

include(GenerateExportHeader) # 引入自动生成导出宏头文件的模块
generate_export_header(Utils # 为目标 Utils 生成导出宏头
    BASE_NAME Utils # 影响默认宏名/文件名；导出条件宏仍是 <目标名>_EXPORTS（CMake 自动定义）
    EXPORT_MACRO_NAME Utils_API # 头文件里书写的修饰宏：class MYLIB_API Foo
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/UtilsExport.h # 生成文件路径（避免与源码树 include/Utils.h 重名冲突）
    STATIC_DEFINE Utils_STATIC_DEFINE # 一旦定义此宏，Utils_API 展开为空（静态库场景）
)

if(NOT BUILD_SHARED_LIBS) # 静态库时额外定义静态宏
    target_compile_definitions(Utils PUBLIC Utils_STATIC_DEFINE) # PUBLIC 让使用方也一致地看到该宏
endif()

target_include_directories(Utils # 设置目标 Utils 的头文件搜索路径
    PUBLIC # PUBLIC 同时作用于自身与使用方
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> # 源码树里的头文件
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include> # 生成的导出宏头文件
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> # 安装后头文件目录（相对安装前缀）
)

target_compile_features(Utils PUBLIC cxx_std_17) # 对外声明需 C++17；PUBLIC 会传导给使用方

# 多个选项必须整体加引号，否则分号会被当成列表分隔符而拆坏生成器表达式
target_compile_options(Utils PRIVATE # 编译选项，仅作用于本目标
    "$<$<CXX_COMPILER_ID:MSVC>:/W4;/permissive-;/utf-8>" # MSVC：严格告警 + 标准符合模式 + UTF-8 源码
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall;-Wextra;-Wpedantic>" # GCC / Clang：常用告警集
)

target_compile_definitions(Utils PRIVATE # 编译宏定义，仅作用于本目标
    "$<$<PLATFORM_ID:Windows>:NOMINMAX;WIN32_LEAN_AND_MEAN>" # 禁止 min/max 宏、精简 windows.h
    "$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>" # 关闭 MSVC CRT 安全告警
)

set_target_properties(Utils PROPERTIES # 设置目标的各类属性
    VERSION ${PROJECT_VERSION} # 动态库全版本：libMyLib.so.1.0.0 / MyLib.dll 资源版本
    SOVERSION ${PROJECT_VERSION_MAJOR} # 动态库 soname 主版本：libMyLib.so.1
    POSITION_INDEPENDENT_CODE ON # 静态库也带 -fPIC，方便被其它动态库引用
    CXX_VISIBILITY_PRESET hidden # 非 Windows：默认隐藏符号，只导出标了 MYLIB_API 的
    VISIBILITY_INLINES_HIDDEN ON # 连带隐藏内联函数符号，减小动态符号表
)

# 用到 std::thread / std::mutex 时需要线程库；链接方式见下
find_package(Threads REQUIRED) # 查找系统线程库
# PUBLIC：使用方也要链接（头文件里出现了 std::mutex 等类型时必须是 PUBLIC）
target_link_libraries(Utils PUBLIC Threads::Threads) # PUBLIC 链接线程库
# 注意：凡是以 PUBLIC / INTERFACE 方式链接的依赖，都必须在附录 1 的 Config.cmake.in 里 find_dependency 一次，
# 否则使用方 find_package(MyLib) 时会因找不到 Threads::Threads 而报「目标不存在」

include(CMakePackageConfigHelpers) # 引入生成包配置/版本文件的模块


install(TARGETS Utils # 安装的目标
    EXPORT UtilsTargets # 归入导出集合，供下面 install(EXPORT) 使用
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # 静态库：.a / .lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # 动态库：.so / .dylib
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # Windows 的 .dll 归 RUNTIME，必须写，否则 DLL 不会被安装
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # 把包含目录写进导出目标的 INTERFACE_INCLUDE_DIRECTORIES
)

# 安装公开头文件（源码树 include/ 下的所有 .h / .hpp，保持子目录结构）



install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ # 头文件所在源目录
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # 安装到包含目录
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" # 仅安装 .h / .hpp
)

# 单独安装生成的导出宏头文件（它位于构建目录，不是源码文件，必须显式安装）


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/UtilsExport.h # 生成的导出宏头（与 generate_export_header 输出一致）
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # 安装到包含目录
)

# 导出目标：生成 MyLibTargets.cmake，其中定义导入目标 MyLib::MyLib




install(EXPORT UtilsTargets # 导出前面的安装目标集合
    FILE UtilsTargets.cmake # 生成的目标描述文件名
    NAMESPACE Utils:: # 导入目标前缀，使用方写 Utils::Utils
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Utils # 存放在 lib/cmake/Utils 下
)

# 由 .in 模板生成包配置文件（内容见附录 1）




configure_package_config_file( # 生成 Config.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/UtilsConfig.cmake.in # 模板文件
    ${CMAKE_CURRENT_BINARY_DIR}/UtilsConfig.cmake # 输出文件
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Utils # 安装位置，用于计算相对路径
)

# 生成版本文件并声明兼容策略：主版本相同即可满足 find_package(MyLib 1.x)




write_basic_package_version_file( # 生成 ConfigVersion.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/UtilsConfigVersion.cmake # 输出文件
    VERSION ${PROJECT_VERSION} # 版本号
    COMPATIBILITY SameMajorVersion # 主版本一致即兼容
)





install(FILES # 安装上面生成的两个配置文件
    ${CMAKE_CURRENT_BINARY_DIR}/UtilsConfig.cmake # 包配置
    ${CMAKE_CURRENT_BINARY_DIR}/UtilsConfigVersion.cmake # 包版本
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Utils # 与导出目标同目录
)

# 可选：同时导出「构建树」，让别的工程在未安装时也能 find_package(MyLib)
# （把本工程的构建目录加进 CMAKE_PREFIX_PATH 即可，常用于同仓库多工程联调）
# export(EXPORT MyLibTargets FILE ${CMAKE_CURRENT_BINARY_DIR}/MyLibTargets.cmake NAMESPACE MyLib::)

# 注意：当前仓库未提供 tests/ 与 examples/ 目录，默认关闭；创建目录后可改为 ${LIB_IS_TOP_LEVEL} 开启
option(Utils_BUILD_TESTS "构建 Utils 单元测试" OFF)
option(Utils_BUILD_EXAMPLES "构建 Utils 示例程序" OFF)

if(Utils_BUILD_TESTS)
    enable_testing() # 在顶层目录调用才会生成 CTest 元数据
    add_subdirectory(tests) # GoogleTest 的接入方式见附录 2
endif()

if(Utils_BUILD_EXAMPLES)
    add_subdirectory(examples) # 见附录 3
endif()
