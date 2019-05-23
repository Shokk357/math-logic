#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

vector<set<int> > road;
vector<set<int> > unroad;
vector<vector<string> > edges;
vector<vector<int> > realedges;
vector<vector<int> > unrealedges;
int nulPoint, onePoint;
vector<bool> visited;
int cnt = 0;

void DFS(int x, int to) {
    if (visited[x]) {
        return;
    }
    visited[x] = true;
    for (int i = 0; i < realedges[x].size(); i++) {
            road[to].insert(realedges[x][i]);
            DFS(realedges[x][i], to);
    }
}

void UNDFS(int x, int to) {
    if (visited[x]) {
        return;
    }
    visited[x] = true;
    for (int i = 0; i < unrealedges[x].size(); i++) {
            unroad[to].insert(unrealedges[x][i]);
            UNDFS(unrealedges[x][i], to);
    }
}

int checkPlus(int a, int b) {
    vector<int> obsh;
    obsh.clear();
    obsh.resize(0);
    for (auto i = road[a].begin(); i != road[a].end(); i++) {
        if (road[b].count(*i) > 0)
            obsh.push_back(*i);
    }
    for (int i = 0; i < obsh.size(); i++) {
        int ch = 0;
        for (int j = 0; j < obsh.size(); j++) {
            if (i == j) continue;
            if (road[obsh[i]].count(obsh[j]) > 0) ch++;
        }
        if (ch == obsh.size() - 1) return obsh[i];
    }
    return -1;
}

int checkMul(int a, int b) {
    vector<int> obsh;
    obsh.clear();
    obsh.resize(0);
    for (auto i = unroad[a].begin(); i != unroad[a].end(); i++) {
        if (unroad[b].count(*i) > 0)
            obsh.push_back(*i);
    }
    for (int i = 0; i < obsh.size(); i++) {
        int ch = 0;
        for (int j = 0; j < obsh.size(); j++) {
            if (i == j) continue;
            if (unroad[obsh[i]].count(obsh[j]) > 0) ch++;
        }
        if (ch == obsh.size() - 1) return obsh[i];
    }
    return -1;
}

bool checkDistr(int a, int b, int c) {
    if (checkMul(a, checkPlus(b, c)) == checkPlus(checkMul(a, b), checkMul(a, c)))
        return true;
    else
        return false;
}

int checkImpl(int a, int b) {
    vector<int> iNeedC;
    iNeedC.clear();
    iNeedC.resize(0);
    for (int c = 0; c < cnt; c++) {
        int mul = checkMul(a, c);
        if (road[mul].count(b) > 0)
            iNeedC.push_back(c);
    }
    for (int i = 0; i < iNeedC.size(); i++) {
        int ch = 0;
        for (int j = 0; j < iNeedC.size(); j++) {
            if (i == j) continue;
            if (unroad[iNeedC[i]].count(iNeedC[j]) > 0)
                ch++;
            else
                break;
        }
        if (ch == iNeedC.size() - 1) return iNeedC[i];
    }
    return -1;
}

bool checkBool(int a) {
    int notA = checkImpl(a, nulPoint);
    if (checkPlus(a, notA) == onePoint)
        return true;
    return false;
}

int main() {
    string n;
    getline(cin, n);
    string s = "";
    for (int i = 0; i < n.length(); i++) {
        if (n[i] != ' ' && n[i] != '\t' && n[i] != '\r') {
            s += n[i];
        }
    }
    int decyat = 1;
    for (int i = s.length() - 1; i >= 0; i--) {
        cnt = cnt + (s[i] - '0') * decyat;
        decyat *= 10;
    }
    edges.resize(cnt);
    realedges.resize(cnt);
    unrealedges.resize(cnt);
    road.resize(cnt);
    unroad.resize(cnt);
    for (int i = 0; i < cnt; i++) {
        string str = "";
        s = "";
        getline(cin, str);
        for (int j = 0; j < str.length(); j++) {
            if ((str[j] != ' ' && str[j] != '\t' && str[j] != '\r') &&
                (str[j - 1] == ' ' || str[j - 1] == '\t' || str[j - 1] == '\r') && s != "") {
                edges[i].push_back(s);
                s = "";
            }
            if (str[j] != ' ' && str[j] != '\t' && str[j] != '\r') {
                s += str[j];
            }
        }
        edges[i].push_back(s);
    }
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < edges[i].size(); j++) {
            string str = edges[i][j];
            int tmpcount = 0;
            int desyat = 1;
            for (int i = str.length() - 1; i >= 0; i--) {
                tmpcount = tmpcount + (str[i] - '0') * desyat;
                desyat *= 10;
            }
            realedges[i].push_back(tmpcount - 1);
        }
        if (find(realedges[i].begin(), realedges[i].end(), i) == realedges[i].end())
            realedges[i].push_back(i);
    }
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < realedges[i].size(); j++) {
            unrealedges[realedges[i][j]].push_back(i);
        }
    }
    for (int i = 0; i < cnt; i++) {
        visited.clear();
        visited.resize(cnt, false);
        road[i].insert(i);
        DFS(i, i);
    }
    for (int i = 0; i < cnt; i++) {
        visited.clear();
        visited.resize(cnt, false);
        unroad[i].insert(i);
        UNDFS(i, i);
    }
    for (int i = 0; i < cnt; i++) {
        if (realedges[i].size() == 1)
            onePoint = i;
    }
    for (int i = 0; i < cnt; i++) {
        if (unrealedges[i].size() == 1)
            nulPoint = i;
    }
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < cnt; j++) {
            if (checkPlus(i, j) == -1) {
                cout << "Операция '+' не определена: " << i + 1 << '+' << j + 1;
                return 0;
            }
        }
    }
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < cnt; j++) {
            if (checkMul(i, j) == -1) {
                cout << "Операция '*' не определена: " << i + 1 << '*' << j + 1;
                return 0;
            }
        }
    }
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < cnt; j++) {
            for (int k = 0; k < cnt; k++) {
                if (!checkDistr(i, j, k)) {
                    cout << "Нарушается дистрибутивность: " << i + 1 << "*(" << j + 1 << '+' << k + 1 << ")";
                    return 0;
                }
            }
        }
    }
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < cnt; j++) {
            if (checkImpl(i, j) == -1) {
                cout << "Операция '->' не определена: " << i + 1 << "->" << j + 1;
                return 0;
            }
        }
    }
    for (int i = 0; i < cnt; i++) {
        if (!checkBool(i)) {
            cout << "Не булева алгебра: " << i + 1 << "+~" << i + 1;
            return 0;
        }
    }
    cout << "Булева алгебра";
    return 0;
}