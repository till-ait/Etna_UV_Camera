#include "CSV_file_manager.h"

CSV_file_manager::CSV_file_manager(std::string _name, std::string _path) {
    name = _name;
    path = _path;
    file_saved = true;

    Open_file();
}

CSV_file_manager::~CSV_file_manager() {
}

bool CSV_file_manager::Is_empty() {
    if(lines.size() == 0) {
        return true;
    }

    return false;
}

void CSV_file_manager::Open_file() {
    std::ifstream fileIn(path+name);
    std::string line;

    if(fileIn.is_open()) {
        while(std::getline(fileIn, line)) {
            lines.push_back(line);
        }
        fileIn.close();
    }
}

void CSV_file_manager::push_colum_without_saving(std::vector<double> datas) {
    int i = 0;
    file_saved = false;
    for(double data : datas) {
        if(i >= lines.size()) {
            break;
        }

        lines[i] = lines[i] + "," + std::to_string(data);
        
        i++;
    }
}

void CSV_file_manager::push_colum_without_saving(std::vector<std::string> datas) {
    int i = 0;
    file_saved = false;
    for(std::string data : datas) {
        if(i >= lines.size()) {
            break;
        }

        lines[i] = lines[i] + "," + data;
        
        i++;
    }
}

void CSV_file_manager::Set_lines_header(std::vector<std::string> datas) {
    file_saved = false;
    for(std::string data : datas) {
        lines.push_back(data);
    }
}

void CSV_file_manager::save() {
    std::ofstream fileOut(path+name);
    for(std::string line : lines) {
        fileOut << line;
        fileOut << "\n";
    }
    fileOut.close();
    file_saved = true;
}

bool CSV_file_manager::is_saved() {
    return file_saved;
}