#include <iostream>
#include "../INI-Parser.h"
using namespace std;

void load_ini_file() {
    IniParser iniParser("Demo.ini");
    for (auto &i : iniParser.all_sections()) {
        cout << "[" << i << "]\n";
        for (auto &j : iniParser.all_keys(i)) {
            cout << j << ": " << iniParser.value(i, j) << endl;
        }
    }
}

void save_ini_file() {
    IniParser iniParser("Output.ini", false);
    iniParser.add_key("Profile", "Name", "John");
    iniParser.add_key("Profile", "Age", "25");
    iniParser.add_key("Profile", "Gender", "Male");
    iniParser.add_key("Profile", "Address", "123 Main St");
    iniParser.add_key("Config", "Language", "English");
    iniParser.add_key("Config", "Theme", "Dark");
    iniParser["Config/save_passwd"] = "false";
    iniParser.value("Config/save_passwd") = "true";
    iniParser.save_ini_file();
    cout << "Save " << iniParser.file_path() << " successfully.\n";
}

void load_ini_file_X() {
    IniParserX iniParserX;
    iniParserX.load_ini_file("Complex.ini");
    for (auto &i : iniParserX.all_sections()) {
        cout << "[" << i << "]\n";
        for (auto &j : iniParserX.all_keys(i)) {
            if (iniParserX.is_array(i, j)) {
                for (int k = 0; k < iniParserX.size_of_array(i, j); ++k) {
                    std::string arr = j.substr(0, j.find("[]"));
                    cout << arr << "[" << k << "]:" << iniParserX.value_of_array(i, j, k) << endl;
                }
            } else {
                cout << j << ": " << iniParserX.value(i, j) << endl;
            }
        }
    }
}

void save_ini_file_X() {
    IniParserX ini("Test.ini", false);
    std::vector<std::string> items = {"C", "C++", "Java", "Python", "Go"};
    std::vector<std::string> hobbies = {"Running", "Football", "Basketball"};
    ini.add_array("Language", "items", items);
    ini.add_array("Hobby", "items", hobbies);
    ini.remove_array("Language", "items");
    ini.save_ini_file();
    std::cout << "File Test.ini saved successfully!\n";
}

int main() {
    load_ini_file();
    save_ini_file();
    load_ini_file_X();
    save_ini_file_X();
    return 0;
}
