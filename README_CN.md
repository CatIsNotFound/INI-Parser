# INI-Parser

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/CatIsNotFound/INI-Parser/blob/main/LICENSE)

轻量级 C++ INI 文件解析器，支持基础键值对操作和数组扩展功能。

## 功能特性

- **基础解析功能**  
  ✅ 支持 Section/Key-Value 的读写和删除  
  ✅ 自动去除值的首尾空格  
  ✅ 异常处理机制（文件加载失败、键重复等）  
  ✅ 支持 UTF-8 编码（不自动处理换行符）

- **扩展功能（IniParserX）**  
  🚀 支持数组类型键（如 `key[] = value1`）  
  🚀 嵌套 Section 继承（正在完善功能中...）  
  🚀 数组长度查询和按索引访问

## 快速开始

### 安装
直接将 `INI-Parser.h` 添加到项目头文件目录。

### 基础用法
```cpp
#include "INI-Parser.h"

// 加载配置文件
IniParser parser("config.ini");

// 读取值
std::string& version = parser.value("General/version");

// 修改值
parser.value("Database/port") = "3306";

// 保存修改
parser.save_ini_file();
```

### 数组操作
```cpp
#include "INI-Parser.h"

// 使用扩展解析器
IniParserX parser_x("data.ini");

// 添加数组
parser_x.add_array("Log", "files", {"app.log", "error.log", "debug.log"});

// 获取数组长度
size_t count = parser_x.size_of_array("Log/files");

// 读取数组元素
std::string log_file = parser_x.value_of_array("Log", "files", 0);
```

## 异常列表

| 异常类型           | 触发场景                          |
|--------------------|----------------------------------|
| `FileLoadedError`  | 文件无法打开或权限不足            |
| `KeyNotFound`      | 访问不存在的键/节                 |
| `KeyAlreadyExist`  | 重复添加已存在的键                |
| `KeyIsArray`       | 尝试以普通键方式访问数组          |
| `KeyNotArray`      | 尝试以数组方式访问普通键          |

## 贡献与许可
- **仓库地址**: [https://github.com/CatIsNotFound/INI-Parser](https://github.com/CatIsNotFound/INI-Parser)
- **许可证**: MIT License
- 欢迎提交 Issue 或 Pull Request

---

🐱 由 [CatIsNotFound](mailto:liaoxinkai0408@outlook.com) 维护 | 文档更新于 2025.03.09
