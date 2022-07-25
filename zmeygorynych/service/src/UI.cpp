//
// Created by Kurisu on 21/07/2022.
//

#include "UI.h"
UserManager* UI::priv = nullptr;
UserManager* UI::unpriv = nullptr;

std::string UI::getInput(const std::string &prompt, bool isPassword) {
    string s;
    if (isPassword) {
        cout << prompt;
        rl_outstream = stderr;
    }
    char* cs = readline(prompt.c_str());
    s = string{cs};
    free(cs);
    if (isPassword) {
        rl_outstream = stdout;
        cout << endl;
    } else {
        add_history(s.c_str());
    }
    return s;
}

void UI::init() {
    std::cout.setf(std::ios::unitbuf);
    UI::priv = new UserManager(true);
    UI::unpriv = new UserManager();
    UI::asciiPrinter("zmeygorynych");
}


void UI::drawMenu(vector<string> list, int w) {
    cout << "┏" << [w]{string s; for (int i = 0; i < w; i++) s+="━"; return s;}() << "┓" << endl;
    for (auto l = list.begin(); l < list.end(); l++){
        //string tstr = fmt::format("{:>2}: ", std::distance(list.begin(), l)+1) + *l;
        string tstr = *l;
        for (int i = 0; i < tstr.length(); i+=w-2){
            if (tstr.length()-i >= w-2) cout << fmt::format("┃ {} ┃", tstr.substr(i, i+w-2)) << endl;
            else cout << fmt::format("┃ {} ┃", tstr.substr(i)+[z = i+w-2-tstr.length()]{string s; for (int i = 0; i < z; i++) s+=" "; return s;}()) << endl;
        }
        if (std::distance(list.begin(), l)+1 != list.size())
            cout << "┣" << [w]{string s; for (int i = 0; i < w; i++) s+="─"; return s;}() << "┫" << endl;
    }
    cout << "┗" << [w]{string s; for (int i = 0; i < w; i++) s+="━"; return s;}() << "┛" << endl;

}

void UI::drawBox(vector<string> list, int w) {
    cout << "┏" << [w]{string s; for (int i = 0; i < w; i++) s+="━"; return s;}() << "┓" << endl;
    for (auto l = list.begin(); l < list.end(); l++){
        //string tstr = fmt::format("{:>2}: ", std::distance(list.begin(), l)+1) + *l;
        string tstr = *l;
        for (int i = 0; i < tstr.length(); i+=w-2){
            if (tstr.length()-i >= w-2) cout << fmt::format("┃ {} ┃", tstr.substr(i, i+w-2)) << endl;
            else cout << fmt::format("┃ {} ┃", tstr.substr(i)+[z = i+w-2-tstr.length()]{string s; for (int i = 0; i < z; i++) s+=" "; return s;}()) << endl;
        }
    }
    cout << "┗" << [w]{string s; for (int i = 0; i < w; i++) s+="━"; return s;}() << "┛" << endl;

}

void UI::asciiPrinter(string msg) {
    auto res = UI::fixAsciiMsg(msg);
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < msg.length(); j++){
            cout << asciiFont[res[j]][i];
        }
        cout << endl;
    }

}

vector<long> UI::fixAsciiMsg(string msg) {
    //TODO: do it more elegantly
    vector<long> res;
    string fmt = "abcdefghijklmnopqrstuvwxyz ";
    std::transform(msg.begin(), msg.end(), msg.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    for (auto c : msg){
        auto i = std::find(fmt.begin(), fmt.end(), c);
        if (i != fmt.end()) {
            res.push_back(std::distance(fmt.begin(), i));
        }
    }
    return res;
}

void UI::quit() {
    cout << "See you later..." << endl;
    exit(0);
}

int UI::getNumber(const string &prompt) {
    return std::stoi(getInput(prompt, false));
}

