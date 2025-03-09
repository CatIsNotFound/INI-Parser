
/*******************************************************************
 * INI-Parser ver 1.0                                              *
 * Copyright (c) 2025 CatIsNotFound <liaoxinkai0408@outlook.com>   *
 * Repository: https://github.com/CatIsNotFound/INI-Parser.git     *
 *******************************************************************/

#ifndef MYPARSER_INI_PARSER_H
#define MYPARSER_INI_PARSER_H
#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <unordered_map>

class KeyNotFound : public std::exception {
public:
    const char * what() const noexcept override {
        return "The specified key is not found!";
    }
};

class KeyAlreadyExist : public std::exception {
public:
    const char * what() const noexcept override {
        return "The specified key is already exist!";
    }
};

class FileLoadedError : public std::exception {
public:
    const char * what() const noexcept override {
        return "The specified file does not exist or does not have read/write permissions.";
    }
};

class KeyNotArray : public std::exception {
public:
    const char * what() const noexcept override {
        return "The specified key is not an array!";
    }
};

class KeyIsArray : public std::exception {
public:
    const char * what() const noexcept override {
        return "The specified key is an array!";
    }
};

class CanNotArray : public std::exception {
public:
    const char * what() const noexcept override {
        return "The specified key can not be an array!";
    }
};

/**
 * @class IniParser
 * @brief Tool class for parsing and manipulating INI format files.
 *
 * Supports basic key-value pair reading, writing, and deleting operations, and provides an exception handling mechanism.
 * @note Default encoding is UTF-8, does not support cross-platform line breaks automatic conversion.
 * @note **The parser does not support array types, use the class "IniParserX" is the best choice.**
 */
class IniParser {
public:
    IniParser() : builder() {}

    /**
     * @brief Loads and parses an INI file from the specified path.
     * @param file_path Path to the target file (with extension).
     * @param read_mode If `true`, the file is parsed immediately; otherwise, only the path is recorded.
     * @throw File_Loaded_Error Thrown when a file cannot be opened or has insufficient permissions.
     * @note Multiple calls overwrite loaded content.
     * @code{.cpp}
     * IniParser parser(“config.ini”);        // load and parse immediately.
     * IniParser parser(“config.ini”, false); // just record the file
     * @endcode
     */
    explicit IniParser(const std::string &file_path, bool read_mode = true) {
        if (!validate_file(file_path) && read_mode) throw FileLoadedError();
        filePath = file_path;
        if (read_mode) load_ini_file(file_path);
    }

    /**
     * @brief Load and parse the specified ini format file
     * @param file_path Specify the files to be loaded and parsed
     * @return Returns true or false to indicate whether the file was successfully loaded and parsed.
     * @notes e.g: If you want to load config file `D:\\MyApp\\config.ini`, just add it:
     * @code{.cpp}
     * iniParser.load_ini_file("D:\\MyApp\\config.ini");
     * @endcode
     */
    virtual bool load_ini_file(const std::string &file_path) {
        if (!validate_file(file_path)) return false;
        set_file_path(file_path);
        std::ifstream file(file_path);
        std::string buf;
        while (std::getline(file, buf)) parse_inline(buf);
        file.close();
        return true;
    }

    virtual void save_ini_file() {
        save_ini_file(filePath);
    }

    /**
     * @brief Save all configurations as INI format files.
     * @param file_path Path of the file to be saved.
     * @exception FileLoadedError Thrown when the parser can not write in the file.
     */
    virtual void save_ini_file(const std::string &file_path) {
        std::ofstream file(file_path);
        if (!file.is_open()) throw FileLoadedError();
        for (auto& i : builder) {
            file << "[" << i.first << "]\n";
            for (auto& j : i.second) file << j.first << " = " << j.second << std::endl;
            file << std::endl;
            file.flush();
        }
        file.close();
    }

    /// @brief Set a new file path for saving or loading
    /// @param file_path Path of the file to be record.
    void set_file_path(const std::string &file_path) { filePath = file_path; }

    /// @brief Get the current file path
    /// @return Returns the current file path
    std::string file_path() const { return filePath; }

    /// @brief Check if the specified key exists
    /// @param key Formatting keywords, e.g. `Config/path`.
    /// @return Returns true or false if the specified key exists.
    bool include(const std::string &key) const {
        auto x = key.find('/');
        if (x == std::string::npos) return builder.find(key) != builder.end();
        else {
            std::string section = key.substr(0, x);
            std::string subsection = key.substr(x + 1);
            if (builder.find(section) == builder.end()) return false;
            return (builder.at(section).find(subsection) != builder.at(section).end());
        }
    }

    /**
     * @brief Get the value of the specified key
     * @param key Formatting keywords, e.g. `Config/path`.
     * @exception KeyNotFound Thrown when the specified key is not found
     *
     * @note If you want to get the `version` keyword from the `General` section, just do it:
     * @code{.cpp}
     * iniParser.value("General/version");
     * @endcode
     */
    virtual std::string& value(const std::string &key) {
        auto x = key.find('/');
        if (x == std::string::npos) throw KeyNotFound();
        if (!include(key))  throw KeyNotFound();
        std::string section = key.substr(0, x);
        std::string subsection = key.substr(x + 1);
        return builder[section][subsection];
    }

    /**
     * @brief Get the value of the specified key
     * @param key Formatting keywords, e.g. `Config/path`.
     * @exception KeyNotFound Thrown when the specified key is not found
     *
     * @note If you want to get the `version` keyword from the `General` section, just do it:
     * @code{.cpp}
     * iniParser.value("General/version");
     * @endcode
     */
    virtual std::string& value(const std::string &section, const std::string &key) {
        if (!include(section)) throw KeyNotFound();
        if (!include(section + "/" + key)) throw KeyNotFound();
        return builder[section][key];
    }

    virtual std::string& operator[](const std::string &key) {
        auto x = key.find('/');
        if (x == std::string::npos) throw KeyNotFound();
        if (!include(key.substr(0, x))) throw KeyNotFound();
        std::string section = key.substr(0, x);
        std::string subsection = key.substr(x + 1);
        return builder[section][subsection];
    }

    /**
     * @brief Get all sections in the current file
     * 
     * @return std::vector<std::string> Return all sections in the current file.
     */
    std::vector<std::string> all_sections() const {
        std::vector<std::string> sections;
        for (auto &i : builder)
            sections.push_back(i.first);
        return sections;
    }

    /**
     * @brief Get all keys in the specified section
     * 
     * @param section Section name
     * @return std::vector<std::string> Return all keys in the specified section.
     * 
     * @exception KeyNotFound Thrown when the specified section is not found
     */
    std::vector<std::string> all_keys(const std::string &section) const {
        if (!include(section)) throw KeyNotFound();
        std::vector<std::string> children;
        for (auto &i : builder.at(section))
            children.push_back(i.first);
        return children;
    }

    /**
     * @brief Add a new key to the specified section
     * @param section Section name
     * @param key Key name
     * @param value Key value
     * @exception KeyAlreadyExist Thrown when the specified key already exists.
     * @exception CanNotArray Thrown when the specified key is an array.
     *
     * @notes e.g: If you want to add "path" key to the "Config" section, just add it:
     * @code{.cpp}
     * iniParser.add_key("Config", "path", "/home/me/my_config.ini");
     * @endcode
     */
    virtual void add_key(const std::string &section, const std::string &key, const std::string &value) {
        if (key.find("[]") != std::string::npos) throw CanNotArray();
        if (!include(section)) { builder[section][key] = value; return; }
        if (!include((section + "/" + key))) { builder[section][key] = value; return; }
        throw KeyAlreadyExist();
    }

    /**
     * @brief Remove a key from the specified section
     * @param section Section name
     * @param key Key name
     * @exception KeyNotFound Thrown when the specified key is not found.
     */
    virtual void remove_key(const std::string &section, const std::string &key) {
        if (include((section + "/" + key))) {
            builder[section].erase(key);
            if (builder[section].empty()) builder.erase(section);
        } else throw KeyNotFound();
    }

protected:
    virtual bool parse_inline(const std::string &buffer) {
        static std::string section;
        if (buffer.find(';') == 0) return false;
        auto sp = buffer.find('=');
        auto sq = buffer.find(';');
        std::string value;
        if (sp != std::string::npos) {
            std::string key = buffer.substr(0, sp);
            if (sq != std::string::npos) value = buffer.substr(sp + 1, sq - sp - 1);
            else value = buffer.substr(sp + 1);
            key = strip(key); value = strip(value);
            builder[section][key] = value;
            return true;
        }
        auto st = buffer.find('[');
        if (st != std::string::npos) {
            section = buffer.substr(st + 1, buffer.find(']') - st - 1); return true;
        }
        return false;
    }

    static std::string strip(std::string &s) {
        size_t st = s.find_first_not_of(' '), ed = s.find_last_not_of(' ');
        return s.substr(st, ed - st + 1);
    }
    static bool validate_file (const std::string path) { return std::fstream(path).is_open(); }

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> builder;
    std::string filePath;
};

/**
 * @class IniParserX
 * @brief INI Parser with extended features.
 * 
 * Add support for array keys.
 * @note The array keys are not supported in the original INI parser.
 */
class IniParserX : public IniParser {
public:
    IniParserX() = default;
    explicit IniParserX(const std::string &file_path, const bool read_mode = true) {
        if (!validate_file(file_path) && read_mode) throw FileLoadedError();
        IniParser::filePath = file_path;
        if (read_mode) load_ini_file(file_path);
    }

    bool load_ini_file(const std::string &file_path) final {
        if (!validate_file(file_path)) return false;
        set_file_path(file_path);
        std::ifstream file(file_path);
        std::string buf;
        while (std::getline(file, buf)) parse_inline(buf);
        file.close();
        return true;
    }

    void save_ini_file() final {
        save_ini_file(IniParser::filePath);
    }

    void save_ini_file(const std::string &file_path) final {
        std::ofstream file(file_path);
        if (!file.is_open()) throw FileLoadedError();
        for (auto &i : builder) {
            file << "[" << i.first << "]\n";
            for (auto &j : i.second) {
                auto p = j.first.find("[]");
                if (p != std::string::npos) {
                    std::string k = i.first + "/" + j.first.substr(0, p);
                    for (size_t idx = 0; idx < size_of_array(i.first, j.first); idx++) {
                        file << j.first << " = " << value_of_array(i.first, j.first, idx) << std::endl;
                    }
                } else {
                    file << j.first << " = " << j.second << std::endl;
                }
            }
            file << std::endl;
            file.flush();
        }
        file.close();
    }

    /** 
     * @brief Decide if the specified key is an array or not
     * 
     * @param key Formatting keywords, e.g. `Config/path`.
     * @return true if the key is an array.
     */
    bool is_array(const std::string &key) const {
        std::string real_key = get_array_name(key);
        return arrays.find(real_key) != arrays.end();
    }

    /** 
     * @brief Decide if the specified key is an array or not
     * 
     * @param section Section name
     * @param key Key name
     * @return true if the key is an array.
     */
    bool is_array(const std::string &section, const std::string &key) const {
        std::string real_key = get_array_name(key);
        return arrays.find(section + "/" + real_key) != arrays.end();
    }

    /**
     * @brief Get the length of the array which is associated with the specified key
     * 
     * @param key Formatting keywords, e.g. `Config/path`.
     * @return size_t Return the length of the array.
     */
    size_t size_of_array(const std::string &key) const {
        std::string real_key = get_array_name(key);
        return ((is_array(real_key)) ? arrays.at(key).size() : throw KeyNotArray());
    }

    /**
     * @brief Get the length of the array which is associated with the specified key
     * 
     * @param section Section name.
     * @param key Key name.
     * @return size_t Return the length of the array.
     */
    size_t size_of_array(const std::string &section, const std::string &key) const {
        std::string real_key = get_array_name(key);
        std::string k = section + "/" + real_key;
        return ((is_array(k)) ? arrays.at(k).size() : throw KeyNotArray());
    }

    /**
     * @brief Get the value of an array element by index.
     * 
     * @param section Section name
     * @param key Key name
     * @param index Index of the array element.
     * @return std::string Return the value of the array element.
     * 
     * @exception KeyNotArray Thrown if the key is not an array.
     */
    std::string value_of_array(const std::string &section, const std::string &key, const size_t &index) const {
        std::string real_key = get_array_name(key);
        std::string k = section + "/" + real_key;
        return ((is_array(k)) ? arrays.at(k).at(index) : throw KeyNotArray());
    }

    /**
     * @brief Get the value of an array element by index.
     * 
     * @param key Formatting keywords, e.g. `Config/path`.
     * @param index Index of the array element.
     * @return std::string Return the value of the array element.
     * 
     * @exception KeyNotArray Thrown if the key is not an array.
     */
    std::string value_of_array(const std::string &key, const size_t &index) const {
        std::string real_key = get_array_name(key);
        return ((is_array(real_key)) ? arrays.at(real_key).at(index) : throw KeyNotArray());
    }

    /**
     * @brief Add an array to the specified key
     * 
     * @param section Section name
     * @param key Key name
     * @param array Array to add, must be a vector of strings.
     * 
     * @exception KeyAlreadyExist Thrown If the specified key already exists.
     */
    void add_array (const std::string& section, const std::string& key, const std::vector<std::string> &array) {
        std::string real_key = get_array_name(key);
        std::string k = section + "/" + real_key;
        if (!include(k + "[]") && !include(k)) {
            IniParser::builder[section][key + "[]"] = array.back();
            arrays[k].assign(array.begin(), array.end());
        } else {
            throw KeyAlreadyExist();
        }
    }

    /**
     * @brief Remove an array from the specified key
     * 
     * @param section Section name
     * @param key Key name
     * 
     * @exception KeyNotArray Thrown if the specified key is not an array
     */
    void remove_array (const std::string& section, const std::string& key) {
        std::string real_key = get_array_name(key);
        if (is_array(section, key)) {
            std::string k = section + "/" + real_key;
            arrays.erase(k);
            IniParser::builder[section].erase(key + "[]");
            if (IniParser::builder[section].empty()) IniParser::builder.erase(section);
        } else {
            throw KeyNotArray();
        }
    }

    /**
     * @brief Add a new key to the specified section
     * @param section Section name
     * @param key Key name
     * @param value Key value
     * @exception KeyAlreadyExist Thrown when the specified key already exists
     * @exception CanNotArray Thrown when the specified key is an array
     */
    void add_key(const std::string &section, const std::string &key, const std::string &value) override {
        if (key.find("[]") != std::string::npos) throw CanNotArray();
        if (!include(section)) { builder[section][key] = value; return; }
        if (!include((section + "/" + key))) { builder[section][key] = value; return; }
        throw KeyAlreadyExist();
    }

    /**
     * @brief Remove a key from the specified section
     * @param section Section name
     * @param key Key name
     * @exception KeyNotFound Thrown when the specified key is not found
     */
    void remove_key(const std::string &section, const std::string &key) override {
        if (include((section + "/" + key))) {
            builder[section].erase(key);
            if (builder[section].empty()) builder.erase(section);
        } else throw KeyNotFound();
    }

    /**
     * @brief Get the value of a specified key
     * 
     * @param key Formatting keywords, e.g. `Config/path`.
     * @return std::string& Return the value of the specified key.
     * 
     * @exception KeyNotFound Thrown when the specified key is not found.
     * @exception KeyIsArray Thrown when the specified key is an array.
     */
    std::string& value(const std::string &key) override {
        auto x = key.find('/');
        if (x == std::string::npos) throw KeyNotFound();
        if (!include(key))  throw KeyNotFound();
        if (is_array(key))  throw KeyIsArray();
        std::string section = key.substr(0, x);
        std::string subsection = key.substr(x + 1);
        return builder[section][subsection];
    }

    /**
     * @brief Get the value of a specified key
     * 
     * @param section Section name
     * @param key Key name
     * @return std::string& Return the value of the specified key.
     * 
     * @exception KeyNotFound Thrown when the specified key is not found.
     * @exception KeyIsArray Thrown when the specified key is an array.
     */
    std::string& value(const std::string &section, const std::string &key) override {
        if (!include(section)) throw KeyNotFound();
        if (!include(section + "/" + key)) throw KeyNotFound();
        if (is_array(section + "/" + key)) throw KeyIsArray();
        return builder[section][key];
    }

    std::string& operator[](const std::string &key) {
        auto x = key.find('/');
        if (x == std::string::npos) throw KeyNotFound();
        if (!include(key.substr(0, x))) throw KeyNotFound();
        if (is_array(key)) throw KeyIsArray();
        std::string section = key.substr(0, x);
        std::string subsection = key.substr(x + 1);
        return builder[section][subsection];
    }

protected:
    bool parse_inline(const std::string& buffer) final {
        static std::string section;
        if (buffer.find(';') == 0) return false;
        auto sp = buffer.find('=');
        auto sq = buffer.find(';');
        std::string value;
        if (sp != std::string::npos) {
            std::string key = buffer.substr(0, sp);
            if (sq != std::string::npos) value = buffer.substr(sp + 1, sq - sp - 1);
            else value = buffer.substr(sp + 1);
            key = strip(key); value = strip(value);
            auto point = key.find("[]");
            if (point != std::string::npos) {
                arrays[section + "/" + key.substr(0, point)].push_back(value);
            }
            builder[section][key] = value;
            return true;
        }
        auto st = buffer.find('[');
        if (st != std::string::npos) {
            std::string tmp_section = buffer.substr(st + 1, buffer.find(']') - st - 1);
            if (tmp_section[0] == '.') {
                connection[section].push_back(tmp_section.substr(1));
                tmp_section = section + tmp_section;
            } else {
                auto point = tmp_section.find('.');
                if (point != std::string::npos) {
                    connection[tmp_section.substr(0, point)].push_back(tmp_section.substr(point + 1));
                }
            }
            section = tmp_section;
        }
        return false;
    }

    static std::string get_array_name(const std::string& old_name) {
        size_t pos = old_name.find("[]");
        return ((pos != std::string::npos) ? old_name.substr(0, pos) : old_name);
    }

private:
    std::unordered_map<std::string, std::vector<std::string>> arrays;
    std::unordered_map<std::string, std::vector<std::string>> connection;
};

#endif //MYPARSER_INI_PARSER_H
