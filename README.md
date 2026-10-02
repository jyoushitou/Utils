# Utils

通用的 C++ 工具库，提供 **I/O 输出、文件读写、时间处理与程序优雅退出** 等基础设施，供上位项目直接复用。

## 特性

- **统一输出**：控制台信息 / 错误 / 网络输出走同一套接口
- **文件与日志**：追加、覆写写文件，日志目录自动检查与创建
- **时间工具**：当前时间戳、时间差计算、格式化时间与日期字符串
- **优雅退出**：跨平台（Windows 控制台事件 / POSIX 信号）统一退出流程，支持注册停止回调
- **服务寻址**：`Message.h` 定义全局 `ServiceID` 枚举与名称转换
- **构建友好**：CMake 一键切换静态库 / 动态库，自动生成导出宏头文件，支持 `install` + `find_package`
- **开箱即用**：构建的同时自动把「公开头文件 + 生成的导出宏头」复制到库产物同级 `include/`，免安装即可直接引用

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

| 产物             | 路径                                             |
| ---------------- | ------------------------------------------------ |
| 静态库           | `build/Release/Utils.lib`                        |
| 动态库           | `build/Release/Utils.dll` + `Utils.lib`          |
| 测试程序         | `build/tests/Release/UtilsTests.exe`             |
| 生成的导出宏头   | `build/include/UtilsExport.h`                    |
| 复制的公开头文件 | `build/include/Message.h`、`Utils.h`（构建时自动） |

> 构建完成后，`build/include/` 下即为「公开头文件 + 生成的 `UtilsExport.h`」一整套头，可直接连同库文件使用。

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

## 快速开始（示例）

下面是一个最小可用示例，涵盖初始化、输出、时间、日志与优雅退出：

```cpp
// demo.cpp
#include "Utils.h"   // 库主头文件（会自动包含 Message.h 与 UtilsExport.h）

int main()
{
    // 1) 初始化：搭建控制台并注册退出相关处理，应在程序早期调用
    Utils::init();

    // 2) 设置本进程的服务ID（全局唯一，供日志/寻址使用）
    Utils::serviceID = User;

    // 3) 统一输出（信息 / 错误 / 网络）
    Utils::Out::outMsg("服务启动");
    Utils::Out::outErr("这是一条错误提示");
    Utils::Out::outNetMsg(1001, "网络输出示例");   // 参数：消息ID + 内容

    // 4) 时间工具
    time_t t0 = Utils::Time::nowTime();                          // 当前时间戳
    Utils::Out::outMsg("当前时间：" + Utils::Time::getNowtime()); // 格式化时间
    Utils::Out::outMsg("当前日期：" + Utils::Time::getNowDay()); // 格式化日期
    // ... 业务处理 ...
    Utils::Out::outMsg("本次处理耗时(秒)：" + std::to_string(Utils::Time::computeTime(t0)));

    // 5) 文件与日志
    Utils::File::setLogsDir("logs");        // 自定义日志目录（默认即 "logs"）
    Utils::File::checkLogsDir();            // 目录不存在则创建
    Utils::File::outLog("写入一条日志");     // 写入日志文件
    Utils::File::outFileAdd("data.txt", "追加一行\n"); // 追加写文件
    Utils::File::outFileWirte("data.txt", "覆盖内容\n"); // 覆写文件

    // 6) 优雅退出：注册停止回调，阻塞等待退出信号
    Utils::Exit::registerStopCallback([](){
        Utils::Out::outMsg("收到退出信号，开始清理资源...");
    });

    // 主线程在此阻塞，直到收到系统退出信号或外部调用 recviceExit()
    Utils::Exit::waitExit();

    // 退出流程（waitExit 返回后触发，或手动调用）
    Utils::Exit::gracefulShutdown();
    return 0;
}
```

> 编译时请确保 `UtilsExport.h` 与 `Message.h`、`Utils.h` 在同一包含目录（构建后位于 `build/include/`）。

- 完整示例可以参考tests/main.cpp文件

## 直接使用头文件 + 库文件

不使用 CMake 时，手动把「公开头文件 + 生成的导出宏头 + 库文件」组合起来即可。

### 需要的文件

| 类别       | 来源                                                                                  |
| ---------- | ------------------------------------------------------------------------------------- |
| 公开头文件 | `include/Message.h`、`include/Utils.h`                                                |
| 导出宏头   | 构建目录 `build/include/UtilsExport.h`（或安装后的 `out/include/UtilsExport.h`）      |
| 库文件     | 静态库 `Utils.lib` / `libUtils.a`，或动态库 `Utils.dll` + `Utils.lib` / `libUtils.so` |

> 注意：`UtilsExport.h` 由 `generate_export_header` 生成，不在源码树里，必须从构建/安装目录取。
> 头文件里 `#include "UtilsExport.h"`，因此二者要在同一包含目录中。
>
> 省事做法：构建完成后 `build/include/` 已经同时包含「公开头文件 + 生成的 `UtilsExport.h`」一整套，
> 直接把这个目录整体拷到你的工程即可（见上面「主要产物」表）。

### 推荐目录摆放

```
myapp/
├─ include/                # 直接把 build/include 整个目录拷贝过来即可
│  ├─ Message.h
│  ├─ Utils.h
│  └─ UtilsExport.h        # 构建时自动生成于 build/include
├─ lib/
│  └─ Utils.lib            # 静态库；动态库再从 build/Release 补 Utils.dll
└─ src/
   └─ main.cpp
```

### MSVC 命令行示例（静态库）

```bat
cl /std:c++17 /EHsc /I include src\main.cpp /link lib\Utils.lib
```

### GCC / Clang 命令行示例（静态库）

```shell
g++ -std=c++17 -I include src/main.cpp -L lib -lUtils -pthread -o myapp
```

### 动态库注意事项

- **Windows**：程序运行时需能找到 `Utils.dll`，把它放到 exe 同目录或加入 `PATH`。
- **Linux**：运行时需能找到 `libUtils.so`，可通过 `-Wl,-rpath` 指定路径，或设置 `LD_LIBRARY_PATH`。
- 动态库使用方**不要**定义 `Utils_STATIC_DEFINE`；链接静态库时才需要该宏（CMake 会自动处理，手动引用时按静态库默认即可）。

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
