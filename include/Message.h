/// @brief       服务ID定义
/// @author      jyoushitou
/// @date        2026-09-16
/// @copyright   Copyright (c) 2026

// 防止重复包含
#pragma once

#include <string_view>

/// @brief      服务ID
/// @details    唯一的服务寻址标识，会写入消息头，两端必须一致
/// @note       显式赋值，禁止依赖默认递增
enum ServiceID : int
{
    ServiceID_Invalid         = 0,  ///< 无效/未指定
    ServiceID_RPCGateway      = 1,  ///< RPC网关服务：请求转发与路由
    ServiceID_SQL             = 2,  ///< SQL数据库服务：数据持久化
    ServiceID_Registry        = 3,  ///< 注册中心服务：服务发现与注册
    ServiceID_ConfigCenter    = 4,  ///< 配置中心服务：统一配置管理
    ServiceID_MonitorService  = 5,  ///< 监控服务：运行状态监控
    ServiceID_SecurityService = 6,  ///< 安全服务：访问控制与安全防护
    ServiceID_CertService     = 7,  ///< 证书服务：证书签发与管理
    ServiceID_TracingService  = 8,  ///< 链路追踪服务：分布式链路追踪
    ServiceID_ServiceConsole  = 9,  ///< 服务控制台：服务管理界面
    ServiceID_AdminConsole    = 10, ///< 管理控制台：后台管理界面
    ServiceID_User            = 11, ///< 用户服务：用户信息与认证
    ServiceID_Article         = 12, ///< 文章服务：文章内容管理
    ServiceID_Blog            = 13, ///< 博客服务：博客业务逻辑
    ServiceID_Image           = 14, ///< 图片服务：图片上传与处理
    ServiceID_Video           = 15, ///< 视频服务：视频上传与处理
    ServiceID_Search          = 16, ///< 搜索服务：全文检索
};

/// @brief      服务ID转名称
/// @details    仅用于日志与诊断
/// @param[in]  id 服务ID
/// @return     服务名称，无效/未知ID返回空
/// @warning    新增枚举值后必须在此补充 case
/// @note       编译期可求值，无堆分配
constexpr std::string_view ServiceName(ServiceID id) noexcept
{
    switch (id)
    {
    case ServiceID_Invalid:         return {};
    case ServiceID_RPCGateway:      return "RPCGateway";
    case ServiceID_SQL:             return "SQL";
    case ServiceID_Registry:        return "Registry";
    case ServiceID_ConfigCenter:    return "ConfigCenter";
    case ServiceID_MonitorService:  return "MonitorService";
    case ServiceID_SecurityService: return "SecurityService";
    case ServiceID_CertService:     return "CertService";
    case ServiceID_TracingService:  return "TracingService";
    case ServiceID_ServiceConsole:  return "ServiceConsole";
    case ServiceID_AdminConsole:    return "AdminConsole";
    case ServiceID_User:            return "User";
    case ServiceID_Article:         return "Article";
    case ServiceID_Blog:            return "Blog";
    case ServiceID_Image:           return "Image";
    case ServiceID_Video:           return "Video";
    case ServiceID_Search:          return "Search";
    }
    // 枚举已全覆盖；兜底返回以应对非法强转值
    return {};
}

