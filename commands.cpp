#include "commands.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

void get_m() {
    cout << "Введіть id користувача: ";
    int id;
    cin >> id;
    
    int recNumber = findUserIndex(id);
    if(recNumber == -1) {
        cout << "Користувача з id " << id << " не знайдено.\n";
        return;
    }
    
    User user;
    if(!readUser(recNumber, user) || !user.isActive) {
        cout << "Користувача з id " << id << " не знайдено або він видалений.\n";
        return;
    }
    
    cout << "---------------------\n";
    cout << "Дані користувача:\n";
    cout << "ID: " << user.id << "\nUsername: " << user.username 
         << "\nEmail: " << user.email << "\nPassword: " << user.password 
         << "\nPhone: " << user.phone << "\nProductCount: " << user.productCount 
         << "\nПерший товар: " << user.firstProduct << "\n";
    cout << "---------------------\n";
}

void get_s() {
    cout << "Введіть id користувача для пошуку товарів: ";
    int id;
    cin >> id;
    
    int recNumber = findUserIndex(id);
    if(recNumber == -1) {
        cout << "Користувача з id " << id << " не знайдено.\n";
        return;
    }
    
    User user;
    if(!readUser(recNumber, user) || !user.isActive) {
        cout << "Користувача з id " << id << " не знайдено або він видалений.\n";
        return;
    }
    
    cout << "---------------------\n";
    cout << "Товари користувача " << user.username << ":\n";
    int prodPos = user.firstProduct;
    while(prodPos != -1) {
        Product prod;
        if(!readProduct(prodPos, prod)) break;
        if(prod.isActive) {
            cout << "---------------------\n";
            cout << "Product ID: " << prod.id << "\nTitle: " << prod.title 
                 << "\nDescription: " << prod.description << "\nPrice: " << prod.price
                 << "\nStatus: " << prod.status << "\n";
        }
        prodPos = prod.nextProduct;
    }
    cout << "---------------------\n";
}

void insert_m() {
    User newUser;
    cout << "Введіть id користувача: ";
    cin >> newUser.id;
    
    if(findUserIndex(newUser.id) != -1) {
        cout << "Користувач з таким id вже існує. Введіть унікальний id.\n";
        return;
    }
    
    cin.ignore();
    cout << "Введіть username: ";
    cin.getline(newUser.username, 50);
    cout << "Введіть email: ";
    cin.getline(newUser.email, 50);
    cout << "Введіть password: ";
    cin.getline(newUser.password, 50);
    cout << "Введіть phone: ";
    cin.getline(newUser.phone, 20);
    
    newUser.firstProduct = -1;
    newUser.productCount = 0;
    newUser.isActive = true;
    
    int recNumber;
    if(!userGarbage.empty()) {
        recNumber = userGarbage.back();
        userGarbage.pop_back();
        fstream file(USER_FILE, ios::binary | ios::in | ios::out);
        file.seekp(recNumber * sizeof(User), ios::beg);
        file.write(reinterpret_cast<const char*>(&newUser), sizeof(User));
        file.flush();
        file.close();   
    } else {
        recNumber = appendUser(newUser);
    }
    
    userIndex.push_back({ newUser.id, recNumber });
    sort(userIndex.begin(), userIndex.end(), [](const UserIndex &a, const UserIndex &b) {
        return a.id < b.id;
    });
    
    cout << "---------------------\n";
    cout << "Користувача додано на позицію " << recNumber << ".\n";
    cout << "---------------------\n";
}

void insert_s() {
    Product newProd;
    cout << "Введіть id товару: ";
    cin >> newProd.id;
    
    if(productIdExists(newProd.id)) {
        cout << "Товар з таким id вже існує. Введіть унікальний id.\n";
        return;
    }
    
    cin.ignore();
    cout << "Введіть title: ";
    cin.getline(newProd.title, 50);
    cout << "Введіть description: ";
    cin.getline(newProd.description, 100);
    cout << "Введіть price: ";
    cin >> newProd.price;
    cout << "Введіть статус: ";
    cin.ignore();
    cin.getline(newProd.status, 20);
    
    cout << "Введіть id власника (користувача): ";
    cin >> newProd.user_id;
    
    newProd.isActive = true;
    newProd.nextProduct = -1;
    
    int userRec = findUserIndex(newProd.user_id);
    if(userRec == -1) {
        cout << "Користувача з id " << newProd.user_id << " не знайдено.\n";
        return;
    }
    User user;
    if(!readUser(userRec, user) || !user.isActive) {
        cout << "Користувача з id " << newProd.user_id << " не знайдено або він видалений.\n";
        return;
    }
    
    int prodPos;
    if(!productGarbage.empty()) {
        prodPos = productGarbage.back();
        productGarbage.pop_back();
        fstream file(PRODUCT_FILE, ios::binary | ios::in | ios::out);
        file.seekp(prodPos * sizeof(Product), ios::beg);
        file.write(reinterpret_cast<const char*>(&newProd), sizeof(Product));
        file.flush();
        file.close();
    } else {
        prodPos = appendProduct(newProd);
    }
    
    newProd.nextProduct = user.firstProduct;
    user.firstProduct = prodPos;
    user.productCount += 1;
    writeUser(userRec, user);
    
    cout << "---------------------\n";
    cout << "Товар додано на позицію " << prodPos << ".\n";
    cout << "---------------------\n";
}

void del_m() {
    cout << "Введіть id користувача для видалення: ";
    int id;
    cin >> id;
    
    int recNumber = findUserIndex(id);
    if(recNumber == -1) {
        cout << "Користувача з id " << id << " не знайдено.\n";
        return;
    }
    
    User user;
    if(!readUser(recNumber, user) || !user.isActive) {
        cout << "Користувача з id " << id << " вже видалено.\n";
        return;
    }
    
    int totalProducts = getRecordCount(PRODUCT_FILE, sizeof(Product));
    for (int pos = 0; pos < totalProducts; pos++) {
        Product prod;
        if(!readProduct(pos, prod)) continue;
        if(prod.user_id == id && prod.isActive) {
            prod.isActive = false;
            writeProduct(pos, prod);
            productGarbage.push_back(pos);
        }
    }
    
    user.isActive = false;
    writeUser(recNumber, user);
    userGarbage.push_back(recNumber);
    
    auto it = remove_if(userIndex.begin(), userIndex.end(), [id](const UserIndex &ui){ return ui.id == id; });
    userIndex.erase(it, userIndex.end());
    
    cout << "---------------------\n";
    cout << "Користувача та всі його товари видалено.\n";
    cout << "---------------------\n";
}

void del_s() {
    cout << "Введіть id товару для видалення: ";
    int prodId;
    cin >> prodId;
    
    fstream prodFile(PRODUCT_FILE, ios::binary | ios::in | ios::out);
    if(!prodFile) {
        cout << "Файл товарів не відкрито.\n";
        return;
    }
    
    int pos = 0;
    bool found = false;
    Product prod;
    while(prodFile.read(reinterpret_cast<char*>(&prod), sizeof(Product))) {
        if(prod.id == prodId && prod.isActive) {
            found = true;
            break;
        }
        pos++;
    }
    if(!found) {
        cout << "Товар з id " << prodId << " не знайдено.\n";
        prodFile.close();
        return;
    }
    prod.isActive = false;
    prodFile.seekp(pos * sizeof(Product), ios::beg);
    prodFile.write(reinterpret_cast<const char*>(&prod), sizeof(Product));
    prodFile.flush();
    prodFile.close();
    productGarbage.push_back(pos);
    
    int ownerId = prod.user_id;
    int userRec = findUserIndex(ownerId);
    if(userRec != -1) {
        User owner;
        if(readUser(userRec, owner)) {
            int prev = -1;
            int cur = owner.firstProduct;
            while(cur != -1) {
                Product temp;
                readProduct(cur, temp);
                if(cur == pos) {
                    if(prev == -1)
                        owner.firstProduct = temp.nextProduct;
                    else {
                        Product prevProd;
                        readProduct(prev, prevProd);
                        prevProd.nextProduct = temp.nextProduct;
                        writeProduct(prev, prevProd);
                    }
                    owner.productCount--;
                    writeUser(userRec, owner);
                    break;
                }
                prev = cur;
                cur = temp.nextProduct;
            }
        }
    }
    
    cout << "---------------------\n";
    cout << "Товар видалено.\n";
    cout << "---------------------\n";
}

void update_m() {
    cout << "Введіть id користувача для оновлення: ";
    int id;
    cin >> id;
    cin.ignore();
    
    int recNumber = findUserIndex(id);
    if(recNumber == -1) {
        cout << "Користувача не знайдено.\n";
        return;
    }
    User user;
    if(!readUser(recNumber, user) || !user.isActive) {
        cout << "Користувача не знайдено або він видалений.\n";
        return;
    }
    
    cout << "Введіть новий username: ";
    cin.getline(user.username, 50);
    cout << "Введіть новий email: ";
    cin.getline(user.email, 50);
    cout << "Введіть новий password: ";
    cin.getline(user.password, 50);
    cout << "Введіть новий phone: ";
    cin.getline(user.phone, 20);
    
    writeUser(recNumber, user);
    cout << "---------------------\n";
    cout << "Дані користувача оновлено.\n";
    cout << "---------------------\n";
}

void update_s() {
    cout << "Введіть id товару для оновлення: ";
    int prodId;
    cin >> prodId;
    cin.ignore();
    
    fstream prodFile(PRODUCT_FILE, ios::binary | ios::in | ios::out);
    if(!prodFile) {
        cout << "Файл товарів не відкрито.\n";
        return;
    }
    int pos = 0;
    bool found = false;
    Product prod;
    while(prodFile.read(reinterpret_cast<char*>(&prod), sizeof(Product))) {
        if(prod.id == prodId && prod.isActive) {
            found = true;
            break;
        }
        pos++;
    }
    if(!found) {
        cout << "Товар з id " << prodId << " не знайдено.\n";
        prodFile.close();
        return;
    }
    
    cout << "Введіть новий title: ";
    cin.getline(prod.title, 50);
    cout << "Введіть новий description: ";
    cin.getline(prod.description, 100);
    cout << "Введіть новий price: ";
    cin >> prod.price;
    cin.ignore();
    cout << "Введіть новий status: ";
    cin.getline(prod.status, 20);
    
    prodFile.seekp(pos * sizeof(Product), ios::beg);
    prodFile.write(reinterpret_cast<const char*>(&prod), sizeof(Product));
    prodFile.flush();
    prodFile.close();
    cout << "---------------------\n";
    cout << "Дані товару оновлено.\n";
    cout << "---------------------\n";
}

void calc_m() {
    ifstream fin(USER_FILE, ios::binary);
    if(!fin) {
        cout << "Не вдалося відкрити файл користувачів.\n";
        return;
    }
    int count = 0;
    User user;
    while(fin.read(reinterpret_cast<char*>(&user), sizeof(User))) {
        if(user.isActive) count++;
    }
    fin.close();
    cout << "---------------------\n";
    cout << "Активних користувачів: " << count << "\n";
    cout << "---------------------\n";
}

void calc_s() {
    ifstream fin(PRODUCT_FILE, ios::binary);
    if(!fin) {
        cout << "Не вдалося відкрити файл товарів.\n";
        return;
    }
    int count = 0;
    Product prod;
    while(fin.read(reinterpret_cast<char*>(&prod), sizeof(Product))) {
        if(prod.isActive) count++;
    }
    fin.close();
    cout << "---------------------\n";
    cout << "Активних товарів: " << count << "\n";
    cout << "---------------------\n";
    
    cout << "Кількість товарів для кожного користувача:\n";
    for(auto &ui : userIndex) {
        User user;
        if(readUser(ui.recordNumber, user) && user.isActive) {
            cout << "---------------------\n";
            cout << "User id " << user.id << " (" << user.username << "): " << user.productCount << "\n";
            cout << "---------------------\n";
        }
    }
}

void ut_m() {
    ifstream fin(USER_FILE, ios::binary);
    if(!fin) {
        cout << "Не вдалося відкрити файл користувачів.\n";
        return;
    }
    User user;
    int pos = 0;
    cout << "---------------------\n";
    cout << "Всі записи користувачів:\n";
    while(fin.read(reinterpret_cast<char*>(&user), sizeof(User))) {
        cout << "---------------------\n";
        cout << "Record " << pos << ":\n";
        cout << "ID: " << user.id << "\nUsername: " << user.username
             << "\nEmail: " << user.email << "\nPassword: " << user.password
             << "\nPhone: " << user.phone << "\nFirstProduct: " << user.firstProduct 
             << "\nProductCount: " << user.productCount << "\nIsActive: " << user.isActive << "\n";
        cout << "---------------------\n";
        pos++;
    }
    fin.close();
}

void ut_s() {
    ifstream fin(PRODUCT_FILE, ios::binary);
    if(!fin) {
        cout << "Не вдалося відкрити файл товарів.\n";
        return;
    }
    Product prod;
    int pos = 0;
    cout << "---------------------\n";
    cout << "Всі записи товарів:\n";
    while(fin.read(reinterpret_cast<char*>(&prod), sizeof(Product))) {
        cout << "---------------------\n";
        cout << "Record " << pos << ":\n";
        cout << "ID: " << prod.id << "\nTitle: " << prod.title 
             << "\nDescription: " << prod.description << "\nPrice: " << prod.price
             << "\nUser_id: " << prod.user_id << "\nStatus: " << prod.status 
             << "\nNextProduct: " << prod.nextProduct << "\nIsActive: " << prod.isActive << "\n";
        cout << "---------------------\n";
        pos++;
    }
    fin.close();
}

void menu() {
    while(true) {
        cout << "\nКоманди:\n";
        cout << "1. get-m\n2. get-s\n3. insert-m\n4. insert-s\n5. del-m\n6. del-s\n7. update-m\n8. update-s\n9. calc-m\n10. calc-s\n11. ut-m\n12. ut-s\n0. Exit\n";
        cout << "Введіть номер команди: ";
        int cmd;
        cin >> cmd;
        cin.ignore();
        switch(cmd) {
            case 1: get_m(); break;
            case 2: get_s(); break;
            case 3: insert_m(); break;
            case 4: insert_s(); break;
            case 5: del_m(); break;
            case 6: del_s(); break;
            case 7: update_m(); break;
            case 8: update_s(); break;
            case 9: calc_m(); break;
            case 10: calc_s(); break;
            case 11: ut_m(); break;
            case 12: ut_s(); break;
            case 0:
                saveUserIndex();
                saveUserGarbage();
                saveProductGarbage();
                cout << "Завершення програми.\n";
                return;
            default:
                cout << "Невідома команда.\n";
        }
    }
}