# ini_reader

Usage:

```cpp
auto ini = IniReader{INI_PATH} // 建议使用ros::package::getPath(node_path)创建绝对路径
auto param = ini.Get<TYPE>(Section, Name); // Section为[]中的字符串，Name为其下key值字符串，返回value。
```

Example:  

```text
[Common]
length = 10 # 长度
```

cpp:

```cpp
auto ini = IniReader{"path/to/config.ini"};
auto length = ini.Get<double>("Common", "length");
```
