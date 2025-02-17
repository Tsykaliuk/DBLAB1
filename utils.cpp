#include "utils.h"
#include <fstream>
#include <algorithm>
#include <iostream>

using namespace std;

const char* USER_FILE         = "users.dat";
const char* USER_INDEX_FILE   = "user_index.dat";
const char* USER_GARBAGE_FILE = "user_garbage.dat";
const char* PRODUCT_FILE         = "products.dat";
const char* PRODUCT_GARBAGE_FILE = "product_garbage.dat";

vector<UserIndex> userIndex;
vector<int> userGarbage;
vector<int> productGarbage;

int getRecordCount(const char* filename, size_t recordSize) {
    ifstream in(filename, ios::binary | ios::ate);
    if (!in) return 0;
    streampos size = in.tellg();
    in.close();
    return static_cast<int>(size / recordSize);
}

bool readUser(int recNumber, User &user) {
    fstream file(USER_FILE, ios::binary | ios::in);
    if(!file) return false;
    file.seekg(recNumber * sizeof(User), ios::beg);
    file.read(reinterpret_cast<char*>(&user), sizeof(User));
    file.close();
    return true;
}

bool writeUser(int recNumber, const User &user) {
    fstream file(USER_FILE, ios::binary | ios::in | ios::out);
    if(!file) return false;
    file.seekp(recNumber * sizeof(User), ios::beg);
    file.write(reinterpret_cast<const char*>(&user), sizeof(User));
    file.flush();
    file.close();
    return true;
}

int appendUser(const User &user) {
    int pos = getRecordCount(USER_FILE, sizeof(User));
    ofstream fout(USER_FILE, ios::binary | ios::app);
    fout.write(reinterpret_cast<const char*>(&user), sizeof(User));
    fout.flush();
    fout.close();
    return pos;
}

bool readProduct(int recNumber, Product &prod) {
    fstream file(PRODUCT_FILE, ios::binary | ios::in);
    if(!file) return false;
    file.seekg(recNumber * sizeof(Product), ios::beg);
    file.read(reinterpret_cast<char*>(&prod), sizeof(Product));
    file.close();
    return true;
}

bool writeProduct(int recNumber, const Product &prod) {
    fstream file(PRODUCT_FILE, ios::binary | ios::in | ios::out);
    if(!file) return false;
    file.seekp(recNumber * sizeof(Product), ios::beg);
    file.write(reinterpret_cast<const char*>(&prod), sizeof(Product));
    file.flush();
    file.close();
    return true;
}

int appendProduct(const Product &prod) {
    int pos = getRecordCount(PRODUCT_FILE, sizeof(Product));
    ofstream fout(PRODUCT_FILE, ios::binary | ios::app);
    fout.write(reinterpret_cast<const char*>(&prod), sizeof(Product));
    fout.flush();
    fout.close();
    return pos;
}

void loadUserIndex() {
    userIndex.clear();
    ifstream fin(USER_INDEX_FILE, ios::binary);
    if(!fin) return;
    
    UserIndex ui;
    while(fin.read(reinterpret_cast<char*>(&ui), sizeof(UserIndex))) {
        userIndex.push_back(ui);
    }
    fin.close();
    
    sort(userIndex.begin(), userIndex.end(), [](const UserIndex &a, const UserIndex &b) {
        return a.id < b.id;
    });
}

void saveUserIndex() {
    ofstream fout(USER_INDEX_FILE, ios::binary | ios::trunc);
    for(auto &ui : userIndex) {
        fout.write(reinterpret_cast<char*>(&ui), sizeof(UserIndex));
    }
    fout.close();
}

void loadUserGarbage() {
    userGarbage.clear();
    ifstream fin(USER_GARBAGE_FILE, ios::binary);
    if(!fin) return;
    int pos;
    while(fin.read(reinterpret_cast<char*>(&pos), sizeof(int))) {
        userGarbage.push_back(pos);
    }
    fin.close();
}

void saveUserGarbage() {
    ofstream fout(USER_GARBAGE_FILE, ios::binary | ios::trunc);
    for(auto &pos : userGarbage) {
        fout.write(reinterpret_cast<char*>(&pos), sizeof(int));
    }
    fout.close();
}

void loadProductGarbage() {
    productGarbage.clear();
    ifstream fin(PRODUCT_GARBAGE_FILE, ios::binary);
    if(!fin) return;
    int pos;
    while(fin.read(reinterpret_cast<char*>(&pos), sizeof(int))) {
        productGarbage.push_back(pos);
    }
    fin.close();
}

void saveProductGarbage() {
    ofstream fout(PRODUCT_GARBAGE_FILE, ios::binary | ios::trunc);
    for(auto &pos : productGarbage) {
        fout.write(reinterpret_cast<char*>(&pos), sizeof(int));
    }
    fout.close();
}

bool productIdExists(int id) {
    ifstream fin(PRODUCT_FILE, ios::binary);
    if(!fin) return false;
    Product prod;
    while(fin.read(reinterpret_cast<char*>(&prod), sizeof(Product))) {
        if(prod.id == id && prod.isActive) {
            fin.close();
            return true;
        }
    }
    fin.close();
    return false;
}

int findUserIndex(int id) {
    int low = 0, high = static_cast<int>(userIndex.size()) - 1;
    while(low <= high) {
        int mid = (low + high) / 2;
        if(userIndex[mid].id == id)
            return userIndex[mid].recordNumber;
        else if(userIndex[mid].id < id)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}
