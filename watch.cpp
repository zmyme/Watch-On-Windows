#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <ratio>
#include <chrono>

using hclock = std::chrono::high_resolution_clock;
using duration = std::chrono::duration<double>;

int execmd(char* cmd,char* result) {
    char buffer[512];
    FILE* pipe = _popen(cmd, "r");
    if (!pipe)
          return 0;

    while(!feof(pipe)) {
    if(fgets(buffer, 128, pipe)){
            strcat(result,buffer);
        }
    }
    _pclose(pipe);
    return 0;
}

std::string GetCommandOutput(std::string command)
{
    std::string result("");
    char buffer[1024];
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return 0;
    while (!feof(pipe))
    {
        if (fgets(buffer, 1024, pipe)) {
            result += buffer;
        }
    }
    return result;
}

std::vector<std::string> split(std::string& str, std::string delemeter)
{
    std::vector<std::string> strs;
    size_t delsize = delemeter.size();
    while (str.size() > 0) {
        size_t delpos = str.find(delemeter);
        if (delpos == std::string::npos) {
            strs.push_back(str);
        } else {
            strs.push_back(str.substr(0, delpos));
            str = str.substr(delpos + delsize);
        }
    }
    return strs;
}

struct ConsoleSize {
    size_t w = 0;
    size_t h = 0;
};

ConsoleSize get_console_size(std::string& content)
{
    std::vector<std::string> strs = split(content, "\n");
    ConsoleSize sz;
    sz.h = strs.size();
    for (size_t i = 0; i < strs.size(); i++) {
        if (strs[i].size() > sz.w) {
            sz.w = strs[i].size();
        }
    }
    return sz;
}

void Trimmedoutput(std::string& content, size_t max_w, size_t max_h)

{
    std::vector<std::string> strs = split(content, "\n");
    size_t lines = max_h > strs.size()?strs.size():max_h;
    for (size_t i = 0; i < lines; i++) {
        std::string str = strs[i];
        if (str.size() > max_w) {
            str = str.substr(0, max_w);
        }
        else if (str.size() < max_w) {
            str += std::string(max_w - str.size(), ' ');
        }
        std::cout<<str<<"\n";
    }
    if (lines < max_h) {
        std::string empty = std::string(max_w, ' ');
        for (size_t i = 0; i < max_h - lines; i++) {
            std::cout<<empty<<std::endl;
        }
    }
}



int main(int argc, char const *argv[])
{
    // lets parse arguments first.
    if (argc < 3) {
        std::cerr<<"Params not understood, usage: watch interval:float command:str"<<std::endl;
        return 1;
    }

    double interval = std::stod(argv[1]);
    std::string command("");
    for (int i = 2; i < argc; i++) {
        command += argv[i];
        command += " ";
    }


    HANDLE hOutput;
    COORD coord={0,0};
    hOutput=GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = 0;
    cci.dwSize = 1;
    SetConsoleCursorInfo(hOutput, &cci);

    // std::string command = "ipconfig";
    std::string output = GetCommandOutput(command);
    ConsoleSize sz = get_console_size(output);
    if (sz.w < 10) {
        sz.w = 10;
    }
    if (sz.h < 5) {
        sz.h = 5;
    }
    SMALL_RECT rc;
    rc.Left = 0;
    rc.Top = 0;
    rc.Right = sz.w;
    rc.Bottom = sz.h;
    SetConsoleWindowInfo(hOutput, TRUE, &rc);

    size_t prev_w = sz.w;
    size_t prev_h = sz.h;

    while (1)
    {
        hclock::time_point t1 = hclock::now();
        SetConsoleCursorPosition(hOutput, coord);
        CONSOLE_CURSOR_INFO cci;
	    cci.bVisible = 0;
	    cci.dwSize = 1;
	    SetConsoleCursorInfo(hOutput, &cci);
        std::string output = GetCommandOutput(command);
        size_t w = 0;
        size_t h = 0;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        w = csbi.srWindow.Right - csbi.srWindow.Left;
        h = csbi.srWindow.Bottom - csbi.srWindow.Top;
        if (w != prev_w or h != prev_h) {
            system("cls");
            prev_w = w;
            prev_h = h;
        }
        Trimmedoutput(output, w, h);
        hclock::time_point t2 = hclock::now();
        double elasped = std::chrono::duration_cast<duration>(t2 - t1).count();
        double remeaning = interval - elasped;
        if (remeaning > 0) {
            Sleep(int(remeaning * 1000));
        }
    }

    return 0;
}