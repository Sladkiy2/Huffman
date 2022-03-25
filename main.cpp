#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>
using namespace std;

class Node
{
public:
    int a;
    char c;
    Node *left, *right;

    Node()
    {
        left = right = nullptr;
    }

    Node(Node *L, Node *R)
    {
        left =  L;
        right = R;
        a = L->a + R->a;
    }
};

struct Compare
{
    bool operator()(const Node* l, const Node* r) const
    {
        return l->a < r->a;
    }
};


string code;
map<char, string > table;

void CreateTable(Node *root)
{
    if (root->left != nullptr)
    {
        code.push_back('0');
        CreateTable(root->left);
    }

    if (root->right != nullptr)
    {
        code.push_back('1');
        CreateTable(root->right);
    }

    if (root->left == nullptr && root->right == nullptr) table[root->c] = code;

    if(!code.empty()) code.pop_back();
}

bool compareFiles(const std::string& p1, const std::string& p2) {
    std::ifstream f1(p1, std::ifstream::binary|std::ifstream::ate);
    std::ifstream f2(p2, std::ifstream::binary|std::ifstream::ate);

    if (f1.fail() || f2.fail()) {
        return false;
    }

    if (f1.tellg() != f2.tellg()) {
        return false;
    }

    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);
    return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(f2.rdbuf()));
}

int main (int argc, char *argv[])
{
    ifstream in("C:/Users/marsi/CLionProjects/Huffman/text.txt", ios::out | ios::binary);

    map<char, int> freq;
    while (true)
    {
        char c = in.get();
        if (c == EOF) break;
        freq[c]++;
    }

    list<Node*> symbols;
    for(map<char,int>::iterator itr = freq.begin(); itr != freq.end(); ++itr)
    {
        Node *p = new Node;
        p->c = itr->first;
        p->a = itr->second;
        symbols.push_back(p);
//        cout << itr->first << ' ' << itr->second << endl;
    }

    while (symbols.size() != 1)
    {
        symbols.sort(Compare());

        Node *SonL = symbols.front();
        symbols.pop_front();
        Node *SonR = symbols.front();
        symbols.pop_front();

        Node *parent = new Node(SonL,SonR);
        symbols.push_back(parent);
    }

    Node *root = symbols.front();
    CreateTable(root);

//encode
    in.clear();
    in.seekg(0);
    ofstream out;
    out.open(R"(C:/Users/marsi/CLionProjects/Huffman/output.txt)");

    out << ' ';
    out << ' ';
    if(freq[' ']) out << table[' '] << ' ';
    for(auto& itr : table){
        if(itr.first == ' ' || itr.first == '\n' || itr.first == '\r') continue;
//        cout << itr.first << ' ' << itr.second << ' ';
        out << itr.first << ' ' << itr.second << ' ';

    }
    if(freq['\r']) out << table['\r'] << ' ';
    if(freq['\n']) out << table['\n'] << ' ';
    out << '\n';

    int count = 0;
    uint8_t buf = 0;
    while (!in.eof())
    {
        char c = in.get();
        string x = table[c];
        for(int n = 0; n < x.size(); n++)
        {
            if(x[n] == '1') buf |= 1 << (7-count);
            count++;
            if (count == 8)
            {
                count = 0;
                out << buf;
                buf = 0;
            }
        }
    }
    out << buf;

    out.seekp(0);
    out << (8 - count) << ' ';

    in.close();
    out.close();

//decode
    ifstream IN("C:/Users/marsi/CLionProjects/Huffman/output.txt", ios::in | ios::binary);
    ofstream OUT("C:/Users/marsi/CLionProjects/Huffman/decoded.txt", ios::in | ios::binary);

//    setlocale(LC_ALL,"Russian");

    //считываем шапку
    map <string, char> symbolsCodes;
    map <std::string, char>::iterator itr;
    int lastMaxBit;
    string line;
    uint8_t symbol;
    string code;
    getline(IN, line);
//    cout << line << endl;
    int i = 2;


    lastMaxBit = line[0] - '0';
    if(lastMaxBit == 8) lastMaxBit = 0;

    if(line[i] == '1' || line[i] == '0')      // если есть пробел в тексте
    {
        while (line[i] != ' ') {
            code.push_back(line[i]);
            i++;
        }
        symbolsCodes[code] = ' ';

        i++;
    }

    while(i < (line.size() - 1))
    {
        symbol = line[i];
        if((line[i] == '0' || line[i] == '1') && (line[i+1] == '0' || line[i+1] == '1'))
        {
            code.clear();
            code.push_back(line[i]);
            for(i = i + 1; line[i] != ' '; i++) code.push_back(line[i]);
            symbolsCodes[code] = '\r';
            code.clear();
            for(i = i + 1; i < (line.size() - 2); i++) code.push_back(line[i]);
            symbolsCodes[code] = '\n';
            break;
        }
        i += 2;
        code.clear();
        while(line[i] != ' ' && i < (line.size() - 1))
        {
            code.push_back(line[i]);
            i++;
        }
        symbolsCodes[code] = symbol;
        i++;
    }

//    for(auto& itr : symbolsCodes){
//        cout << itr.first << ' ' << itr.second << '\n';
////        out << itr.first << ' ' << itr.second << ' ';
//
//    }

    int maxBit = 8;
    count = 0;
    uint8_t byte;
    byte = IN.get();
    while(!IN.eof())
    {
        code.clear();
        while(count <= maxBit)
        {
            bool b = byte & (1 << (7-count));
            if (b) code.push_back('1');
            else code.push_back('0');

            count++;
            if (count == 8)
            {
                count = 0;
                byte = IN.get();
                if(IN.peek() == EOF) maxBit = 7 - lastMaxBit;
            }

            itr = symbolsCodes.find(code);
            if(itr != symbolsCodes.end())
            {
                OUT << itr->second;
                break;
            }
        }
    }

    IN.close();
    OUT.close();

    string inputPath = R"(C:\Users\marsi\CLionProjects\Huffman\text.txt)";
    string decodedPath = R"(C:\Users\marsi\CLionProjects\Huffman\decoded.txt)";

    if(compareFiles(inputPath, decodedPath))
        cout << "Equal" << endl;
    else
        cout << "Not equal" << endl;
    return 0;
}