# Utils

通用的 C++ 工具库，提供 **I/O 输出、文件读写、时间处理与程序优雅退出** 等基础设施，供上位项目直接复用。

## 特性

- **统一输出**：控制台信息 / 错误 / 网络输出走同一套接口
- **文件与日志**：追加、覆写写文件，日志目录自动检查与创建
- **时间工具**：当前时间戳、时间差计算、格式化时间与日期字符串
- **优雅退出**：跨平台（Windows 控制台事件 / POSIX 信号）统一退出流程，支持注册停止回调
- **服务寻址**：`Message.h` 定义全局 `ServiceID` 枚举与名称转换
- **构建友好**：CMake 一键切换静态库 / 动态库，自动生成导出宏头文件，支持 `install` + `find_package`

## 目录结构

```
Utils/
├─ CMakeLists.txt              # 构建脚本
├─ include/                    # 公开头文件（会被安装）
│  ├─ Message.h                # ServiceID 定义与名称转换
│  └─ Utils.h                  # 库主头文件
├─ source/                     # 实现（不安装）
│  └─ Utils.cpp
├─ tests/                      # 单元测试（CTest）
│  ├─ CMakeLists.txt
│  └─ main.cpp
└─ cmake/
   └─ UtilsConfig.cmake.in     # 包配置模板，供 find_package 使用
```

## 环境要求

- C++17 及以上
- CMake 3.16 及以上
- 支持 MSVC / GCC / Clang

## 构建

```shell
# 配置（默认构建静态库）
cmake -S . -B build

# 构建动态库：追加 -DBUILD_SHARED_LIBS=ON
cmake -S . -B build -DBUILD_SHARED_LIBS=ON

# 编译（VS 等多配置生成器需指定 --config）
cmake --build build --config Release
```

主要产物（Windows + VS 生成器）：

| 产物           | 路径                                    |
| -------------- | --------------------------------------- |
| 静态库         | `build/Release/Utils.lib`               |
| 动态库         | `build/Release/Utils.dll` + `Utils.lib` |
| 测试程序       | `build/tests/Release/UtilsTests.exe`    |
| 生成的导出宏头 | `build/include/UtilsExport.h`           |

## 测试

```shell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## 安装与使用

```shell
cmake --install build --prefix out --config Release
```

在上位工程中通过包配置引入：

```cmake
find_package(Utils CONFIG REQUIRED)
target_link_libraries(your_app PRIVATE Utils::Utils)
```

同仓库联调也可直接源码引入：

```cmake
add_subdirectory(path/to/Utils)
target_link_libraries(your_app PRIVATE Utils::Utils)
```

## 模块速览

| 模块     | 命名空间        | 说明                              |
| -------- | --------------- | --------------------------------- |
| 服务标识 | `ServiceID`     | 服务全局唯一 ID 及名称转换        |
| 时间     | `Utils::Time`   | 时间戳、时间差、格式化时间 / 日期 |
| 退出     | `Utils::Exit`   | 优雅退出、停止回调、退出等待      |
| 文件     | `Utils::File`   | 文件读写、日志目录与日志写入      |
| 输出     | `Utils::Out`    | 控制台 / 错误 / 网络输出          |
| 字符串   | `Utils::String` | 字符串分词等工具                  |

## 许可证

本项目基于 [MIT License](LICENSE) 开源，版权所有 (c) 2026 ThornFireSky。
