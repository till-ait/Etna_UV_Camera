#ifndef CSV_FILE_MANAGER_H
#define CSV_FILE_MANAGER_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CSV_file_manager {
public:
    CSV_file_manager(std::string _name, std::string _path);
    ~CSV_file_manager();
    bool Is_empty();
    void push_colum_without_saving(std::vector<double> datas);
    void push_colum_without_saving(std::vector<std::string> datas);
    void Set_lines_header(std::vector<std::string> datas);
    bool is_saved();
    void save();

private:
    std::string name;
    std::string path;
    bool file_saved;
    
    std::vector<std::string> lines;

    void Open_file();
};

#endif