/**
 * Created Time: 2023.03.21
 * File name:    ini_reader.h
 * Author:       zgy(zhangguangyan@wicri.org)
 * Brief:        一个简单的读取ini文件的工具类，可以将ini转化为map
 * Include:      IniReader{}
 *
 * Usage:        auto ini = IniReader{INI_PATH} // 建议使用ros::package::getPath(node_path)创建绝对路径
 *               auto param = ini.Get<TYPE>(Section, Name); // Section为[]中的字符串，Name为其下key值字符串，返回value。
 *
 * Example:      file in path "src/nodes/mynode/config/sys_config.ini":
 *                    [Common]
 *                    length = 10 # 长度
 *               cpp:
 *                    auto ini = IniReader{ros::package::getPath("mynode") + "/config/sys_config.ini"};
 *                    auto length = ini.Get<double>("Common", "length");
 */

#ifndef __INI_READER__
#define __INI_READER__

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class IniReader {
  private:
    struct IniLine {
        std::string value;
        std::string comment;
    };

  private:
    std::map<std::string, std::map<std::string, IniReader::IniLine>> ini_map_;
    std::map<std::string, std::map<std::string, std::string>>        description_map_;
    std::string                                                      file_name_;
    std::string                                                      option_comment_characters_;

  public:
    IniReader(const std::string& file_name) : option_comment_characters_(";#") {
        if (file_name != "")
            if (not Load(file_name)) std::cerr << "Unable to open the file " + file_name << std::endl;
    }

    ~IniReader() {
        Free();
    }

    auto GetMap() -> std::map<std::string, std::map<std::string, IniReader::IniLine>> {
        return ini_map_;
    }

    void Free() {
        ini_map_.clear();
    }

    void ParasitCar(std::string& str) {
        size_t fin = str.size();
        if (fin < 1) return;
        if (str.at(fin - 1) < ' ') str.erase(fin - 1);
    }

    auto Trim(const std::string& str) -> std::string {
        auto deb = size_t{0};
        auto fin = str.size();
        auto chr = char{};
        while (deb < fin) {
            chr = str.at(deb);
            if ((chr != ' ') && (chr != '\t')) break;
            deb++;
        }
        while (fin > 0) {
            chr = str.at(fin - 1);
            if ((chr != ' ') && (chr != '\t')) break;
            fin--;
        }
        return str.substr(deb, fin - deb);
    }

    auto Load(const std::string& file_name) -> bool {
        auto pos    = size_t{};
        auto pos2   = size_t{};
        auto length = size_t{};

        auto line    = std::string{};
        auto section = std::string{};
        auto key     = std::string{};
        auto comment = std::string{};

        auto    file = std::ifstream{};
        IniLine iniLine;

        Free();
        file_name_ = file_name;

        // open the file
        file.open(file_name_.c_str(), std::ifstream::in);
        if (!file) return false;

        // read each line
        while (getline(file, line)) {
            ParasitCar(line);
            if (line.empty()) continue;
            length          = line.length();
            key             = "";
            iniLine.value   = "";
            iniLine.comment = "";
            // sections
            if (line.at(0) == '[') {
                pos = line.find_first_of(']');
                if (pos == std::string::npos) pos = line.length();
                section = Trim(line.substr(1, pos - 1));
                if (comment != "") {
                    description_map_[section][""] = comment;
                    comment                       = "";
                }
                continue;
            }
            // comments
            pos = std::string::npos;
            for (unsigned int i = 0; i < option_comment_characters_.length(); ++i) {
                pos2 = line.find_first_of(option_comment_characters_[i]);
                if (pos2 == std::string::npos) continue;
                if (pos == std::string::npos) {
                    pos = pos2;
                    continue;
                }
                if (pos > pos2) pos = pos2;
            }
            if (pos != std::string::npos) {
                if (pos > 0) {
                    iniLine.comment = line.substr(pos + 1, length - pos);
                    line.erase(pos, length - pos);
                } else {
                    if (comment != "") comment += '\n';
                    comment += line.substr(pos + 1, length - pos);
                    continue;
                }
            }
            // value
            pos = line.find_first_of('=');
            if (pos != std::string::npos) {
                iniLine.value = Trim(line.substr(pos + 1, length - pos));
                line.erase(pos, length - pos);
            }
            // save to map
            key                    = Trim(line);
            ini_map_[section][key] = iniLine;
            if (comment != "") {
                description_map_[section][key] = comment;
                comment                        = "";
            }
        }
        file.close();
        return true;
    }

    template <class T>
    auto Get(const std::string& section, const std::string& key) -> T {
        auto value      = std::string{";"};
        auto it_section = ini_map_.find(section);
        if (it_section != ini_map_.end()) {
            if (it_section->second.find(key) != it_section->second.end())
                value = it_section->second.find(key)->second.value;
            else
                std::cerr << "key " + key + " not found!" << std::endl;
        } else
            std::cerr << "section " + section + " not found!" << std::endl;

        auto result = T{};
        if (value != std::string(";")) {
            auto ss = std::stringstream{};
            ss << value;
            ss >> result;
        }
        return result;
    }

    auto GetComment(const std::string& section, const std::string& key) -> std::string {
        auto it_section = ini_map_.find(section);
        if (it_section == ini_map_.end()) return "";
        auto it_key = it_section->second.find(key);
        if (it_key == it_section->second.end()) return "";
        return it_key->second.comment;
    }
};

#endif // __INI_READER__