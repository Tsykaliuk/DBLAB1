#include "commands.h"
#include "utils.h"
#include <iostream>
using namespace std;

int main() {
    loadUserIndex();
    loadUserGarbage();
    loadProductGarbage();
    menu();
}
