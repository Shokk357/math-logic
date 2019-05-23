#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

string expression(string s);

string negativ(string s) {
    int balance = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '(')
            balance++;
        if (s[i] == ')')
            balance--;
        if (balance == 0 && s[i] == '!') {
            return "(!" + negativ(s.substr(i + 1, s.length() - i - 1)) + ")";
        }
    }
    if (s.length() > 0 && s[0] == '(') {
        return expression(s.substr(1,s.length() - 2));
    } else
        return s;
}

string kon(string s) {
    int balance = 0;
    for (int i = s.length() - 1; i >= 0; i--) {
        if (s[i] == '(')
            balance++;
        if (s[i] == ')')
            balance--;
        if (balance == 0 && s[i] == '&') {
            return "(&," + kon(s.substr(0, i)) + "," + negativ(s.substr(i + 1, s.length() - i)) + ")";
        }
    }
    return negativ(s);
}

string diz(string s) {
    int balance = 0;
    for (int i = s.length() - 1; i >= 0; i--) {
        if (s[i] == '(')
            balance++;
        if (s[i] == ')')
            balance--;
        if (balance == 0 && s[i] == '|') {
            return  "(|," + diz(s.substr(0, i)) + "," + kon(s.substr(i + 1, s.length() - i - 1)) + ")";
        }
    }
    return kon(s);
}

string expression(string s) {
    int balance = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '(')
            balance++;
        if (s[i] == ')')
            balance--;
        if (balance == 0 && s[i] == '-') {
            return "(->," + diz(s.substr(0, i)) + "," + expression(s.substr(i + 2, s.length() - i - 2)) + ")";
        }
    }
    return diz(s);
}

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    string s, tmp, final;
    while(getline(cin, s)){
        tmp += s;
    }
    for (int i = 0; i < tmp.length(); i++) {
        if (tmp[i] != ' ' && tmp[i] != '\t' && tmp[i] != '\n')
            final += tmp[i];
    }
    cout << expression(final);
    return 0;
}