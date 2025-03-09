
# INI-Parser

[简体中文](README_CN.md)

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/CatIsNotFound/INI-Parser/blob/main/LICENSE)

A lightweight C++ INI file parser with basic key-value operations and array extension support.

## Features

- **Core Parsing**  
  ✅ Read/Write/Delete sections and key-value pairs  
  ✅ Automatic trimming of leading/trailing spaces  
  ✅ Exception handling (file load failure, duplicate keys, etc.)  
  ✅ UTF-8 encoding support (no auto line break conversion)

- **Extended Features (IniParserX)**  
  🚀 Array-type keys support (e.g., `key[] = value1`)  
  🚀 Experimental nested section inheritance  
  🚀 Array length queries and index-based access

## Quick Start

### Installation
Add `INI-Parser.h` directly to your project's header directory.

### Basic Usage
```cpp
#include "INI-Parser.h"

// Load config file
IniParser parser("config.ini");

// Read value
std::string& version = parser.value("General/version");

// Modify value
parser.value("Database/port") = "3306";

// Save changes
parser.save_ini_file();
```

### Array Operations
```cpp
#include "INI-Parser.h"

// Use extended parser
IniParserX parser_x("data.ini");

// Add an array
parser_x.add_array("Log", "files", {"app.log", "error.log", "debug.log"});

// Get array length
size_t count = parser_x.size_of_array("Log/files");

// Access array elements
std::string log_file = parser_x.value_of_array("Log", "files", 0);
```

## Exceptions

| Exception Type       | Trigger Scenario                  |
|----------------------|-----------------------------------|
| `FileLoadedError`    | File cannot be opened or lacks permissions |
| `KeyNotFound`        | Accessing non-existent key/section |
| `KeyAlreadyExist`    | Adding a duplicate key           |
| `KeyIsArray`         | Accessing an array as a regular key |
| `KeyNotArray`        | Accessing a regular key as an array |

## Contribution & License
- **Repository**: [https://github.com/CatIsNotFound/INI-Parser](https://github.com/CatIsNotFound/INI-Parser)
- **License**: MIT License
- Issues and Pull Requests are welcome!

---

🐱 Maintained by [CatIsNotFound](mailto:liaoxinkai0408@outlook.com) | Last updated: March 9, 2025
