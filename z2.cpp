#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

struct tree {
    string value = "";
    tree *left = nullptr;
    tree *right = nullptr;
};

map<string, int> pred;
map<string, tree *> *save;
vector<string> firstarg, firstargnotmod, gyp, guf;

string axiomy[10] = {
        "A->(B->A)",
        "(A->B)->(A->(B->C))->(A->C)",
        "A->B->(A&B)",
        "(A&B)->A",
        "(A&B)->B",
        "A->(A|B)",
        "B->(A|B)",
        "(A->C)->(B->C)->((A|B)->C)",
        "(A->B)->(A->!B)->!A",
        "!!A->A"
};

vector<string> leftargs, rightargs;

tree *modaxiomy[10];

string expression(string s);

void findParts(string s) {
    bool flag = false;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '-') {
            int j = i + 2;
            int balance = 0;
            for (int k = j + 1; k < s.length(); k++) {
                if (s[k] == '(')
                    balance++;
                if (s[k] == ')')
                    balance--;
                if (s[k] == ',' && balance == 0) {
                    leftargs.push_back(s.substr(j + 1, k - j - 1));
                    rightargs.push_back(s.substr(k + 1, s.length() - k - 2));
                    flag = true;
                    break;
                }
            }
            if (flag) break;
        }
    }
    if (!flag) {
        leftargs.push_back("");
        rightargs.push_back("");
    }
}

pair<int, int> checkmp(string s) {
    for (int i = rightargs.size() - 2; i >= 0; i--) {
        if (s == rightargs[i]) {
            for (int k = 0; k < firstarg.size() - 1; k++) {
                if (firstarg[k] == leftargs[i]) {
                    return make_pair(i + 1, k + 1);
                }
            }
        }
    }
    return make_pair(-1, -1);
}

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
        return expression(s.substr(1, s.length() - 2));
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
            return "(|," + diz(s.substr(0, i)) + "," + kon(s.substr(i + 1, s.length() - i - 1)) + ")";
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

void buildtree(string s, tree *node) {
    bool flag = false;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '&' or s[i] == '|' or s[i] == '-') {
            node->value = s[i];
            int j = 0;
            if (s[i + 1] == '>') j = i + 2;
            else
                j = i + 1;
            int balance = 0;
            for (int k = j + 1; k < s.length(); k++) {
                if (s[k] == '(') balance++;
                if (s[k] == ')') balance--;
                if (s[k] == ',' && balance == 0) {
                    node->left = new tree();
                    node->right = new tree();
                    buildtree(s.substr(j + 1, k - j - 1), node->left);
                    buildtree(s.substr(k + 1, s.length() - k - 2), node->right);
                    flag = true;
                    break;
                }
            }
            if (flag) break;
        } else {
            if (s[i] == '!') {
                node->value = s[i];
                node->left = new tree();
                buildtree(s.substr(i + 1, s.length() - 2 - i), node->left);
                break;
            }
            int j = i;
            while (j < s.length() && s[j] != '(' && s[j] != ')') {
                node->value += s[j];
                j++;
            }
        }
    }
}

void writetree(tree *node) {
    cout << node->value << endl;
    if (node->left != nullptr) writetree(node->left);
    if (node->right != nullptr) writetree(node->right);
}

bool checkTres(tree *r1, tree *r2) {
    if (r1 == nullptr && r2 == nullptr) {
        return true;
    }
    if ((r1 == nullptr && r2 != nullptr) || (r1 != nullptr && r2 == nullptr)) {
        return false;
    }
    if (r1->value == r2->value) {
        return checkTres(r1->left, r2->left) & checkTres(r1->right, r2->right);
    } else {
        return false;
    }

}


bool chechaxi(tree *nodeaxi, tree *nodepotentialaxi) {
    if (nodeaxi->value == "A" | nodeaxi->value == "B" | nodeaxi->value == "C") {
        if (save->count(nodeaxi->value) > 0) {
            return checkTres(save->at(nodeaxi->value), nodepotentialaxi);
        } else {
            tree *tmp = new tree();
            tmp->value = nodepotentialaxi->value;
            tmp->right = nodepotentialaxi->right;
            tmp->left = nodepotentialaxi->left;
            save->insert(make_pair(nodeaxi->value, tmp));
            return true;
        }
    } else {
        if (nodeaxi->value.size() > 0 &&
            (nodeaxi->value[0] == '&' | nodeaxi->value[0] == '-' | nodeaxi->value[0] == '|') &&
            nodeaxi->value == nodepotentialaxi->value &&
            (nodeaxi->left != nullptr && nodeaxi->right != nullptr && nodepotentialaxi->left != nullptr &&
             nodepotentialaxi->right != nullptr)) {
            return chechaxi(nodeaxi->left, nodepotentialaxi->left) & chechaxi(nodeaxi->right, nodepotentialaxi->right);
        } else if (nodeaxi->value == nodepotentialaxi->value)
            return chechaxi(nodeaxi->left, nodepotentialaxi->left);
        else
            return false;
    }
}
bool isPredpl(string s) {
    for (int i = 0; i < guf.size(); i++){
        if (s == guf[i]) return true;
    }
    return false;
}

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    string s, final, ss;
    string predpl;
    getline(cin, predpl);
    s = "";
    for (int i = 0; i < predpl.length(); i++) {
        if (predpl[i] != ' ' && predpl[i] != '\t' && predpl[i] != '\r') {
            s += predpl[i];
        }
    }
    predpl = s;
    string tmpredpl = "";
    string vivod = "";
    int i = 0;
    int j = 1;


    int indzap = 0;
    int indpalki = 0;

    for (int ch = 0; ch < predpl.length(); ch++) {
        if (predpl[ch] == ',') {
            indzap = ch;
        }
        if (predpl[ch] == '|') {
            indpalki = ch;
            break;
        }
    }

    while (predpl[i] != '-') {
        string tmp = "";
        while (predpl[i] != ',' && predpl[i] != '|') {
            tmp += predpl[i];
            i++;
        }
        gyp.push_back(tmp);
        i++;
    }
    guf.resize(gyp.size() - 1);

    for (int i = 0; i < gyp.size() - 1; i++){
        guf[i] = expression(gyp[i]);
    }
    tmpredpl = gyp[gyp.size() - 1];

    for (int i = 0; i < indzap; i++)
        vivod += predpl[i];

    vivod += "|-(" + tmpredpl + ")->(";

    for (int i = indpalki + 2; i < predpl.length(); i++)
        vivod += predpl[i];

    vivod += ")";

    cout << vivod << endl;

    for (int i = 0; i < 10; i++) {
        modaxiomy[i] = new tree();
        buildtree(expression(axiomy[i]), modaxiomy[i]);
    }

    int qq = 1;

    while (getline(cin, ss)) {
        if (ss == "") continue;
        s = "";
        for (int i = 0; i < ss.length(); i++) {
            if (ss[i] != ' ' && ss[i] != '\t' && ss[i] != '\r') {
                s += ss[i];
            }
        }

        firstargnotmod.push_back(s);
        string exps = expression(s);
        findParts(exps);
        firstarg.push_back(exps);

        if (exps == expression(tmpredpl)) {
            cout << "(" + s + ")" << "->(" << "(" + s + ")" << "->" << "(" + s + ")" << ")" << endl;
            cout << "(" << "(" + s + ")" << "->(" << "(" + s + ")" << "->" << "(" + s + ")" << "))->(" << "(" + s + ")" << "->((" << "(" + s + ")" << "->" << "(" + s + ")" << ")->" << "(" + s + ")" << "))->(" << "(" + s + ")" << "->" << "(" + s + ")" << ")" << endl;
            cout << "(" << "(" + s + ")" << "->((" << "(" + s + ")" << "->" << "(" + s + ")" << ")->" << "(" + s + ")" << "))->(" << "(" + s + ")" << "->" << "(" + s + ")" << ")" << endl;
            cout << "(" << "(" + s + ")" << "->((" << "(" + s + ")" << "->" << "(" + s + ")" << ")->" << "(" + s + ")" << "))" << endl;
            cout << "(" + s + ")" << "->" << "(" + s + ")" << endl;
            continue;
        }
        if (isPredpl(exps)) {
            cout << "(" + s + ")" << endl;
            cout << "(" + s + ")" << "->(" << "(" + tmpredpl + ")" << "->" << "(" + s + ")"
                 << ")" << endl;
            cout << "(" + tmpredpl + ")" << "->" << "(" + s + ")" << endl;
            qq++;
            continue;
        }
        tree *node = new tree();
        buildtree(exps, node);
        bool axiomsflag = false;
        for (int i = 0; i < 10; i++) {
            save = new map<string, tree *>;
            if (chechaxi(modaxiomy[i], node)) {
                cout << "(" + s + ")" << endl;
                cout << "(" + s + ")" << "->(" << "(" + tmpredpl + ")" << "->" << "(" + s + ")"
                     << ")" << endl;
                cout << "(" + tmpredpl + ")" << "->" << "(" + s + ")" << endl;
                qq++;
                axiomsflag = true;
                break;
            }
        }
        if (axiomsflag) continue;
        bool mp = false;
        pair<int, int> checkpos = checkmp(exps);
        if (checkpos.first != -1) {
            int pos = checkpos.second - 1;
            mp = true;
            cout << "(" << "(" + tmpredpl + ")" << "->" << "(" + firstargnotmod[pos] + ")" << ")->((" << "(" + tmpredpl + ")" << "->(" << "(" + firstargnotmod[pos] + ")" << "->" << "(" + s + ")" << "))->(" << "(" + tmpredpl + ")" << "->" << "(" + s + ")" << "))" << endl;
            cout << "((" << "(" + tmpredpl + ")" << "->(" << "(" + firstargnotmod[pos] + ")" << "->" << "(" + s + ")" << "))->(" << "(" + tmpredpl + ")" << "->" << "(" + s + ")" << "))" << endl;
            cout << "(" + tmpredpl + ")" << "->" << "(" + s + ")" << endl;
            qq++;
            continue;
        }
        qq++;
    }
    return 0;
}