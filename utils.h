#pragma once
#include <vector>
using namespace std;

extern const char* USER_FILE;
extern const char* USER_INDEX_FILE;
extern const char* USER_GARBAGE_FILE;
extern const char* PRODUCT_FILE;
extern const char* PRODUCT_GARBAGE_FILE;

struct User {
    int id;                     // unique key
    char username[50];
    char email[50];
    char password[50];
    char phone[20];
    int firstProduct;           // номер першого товару. -1, якщо немає
    int productCount;           // кількість товарів
    bool isActive;              // 1 – активний, 0 – видалений
};

struct Product {
    int id;                     // unique key
    char title[50];
    char description[100];
    float price;
    int user_id;                // id власника
    char status[20];
    int nextProduct;            // номер наступного товару. -1, якщо немає
    bool isActive;              // 1 – активний, 0 – видалений
};

struct UserIndex {
    int id;             // id користувача
    int recordNumber;   // номер запису в файлі users.dat
};

//тут зберігаються номери видалених записів
extern vector<UserIndex> userIndex;
extern vector<int> userGarbage; 
extern vector<int> productGarbage;  


// визначення кількості записів у файлі
int getRecordCount(const char* filename, size_t recordSize);

// читання/запис user
bool readUser(int recNumber, User &user);
bool writeUser(int recNumber, const User &user);
int appendUser(const User &user);

// читання/запис product
bool readProduct(int recNumber, Product &prod);
bool writeProduct(int recNumber, const Product &prod);
int appendProduct(const Product &prod);

// завантаження/збереження індексу та сміття
void loadUserIndex();
void saveUserIndex();
void loadUserGarbage();
void saveUserGarbage();
void loadProductGarbage();
void saveProductGarbage();

// перевірка унікальності id товару
bool productIdExists(int id);

// пошук користувача в індексі
int findUserIndex(int id);