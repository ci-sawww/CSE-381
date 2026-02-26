// "Copyright [2026] <Maria Kunigk-Bakalar>"

#include <iostream>
#include <string>
#include <cstdint>
#include <iomanip>

struct Person {
    std::string name;
    std::string position;
    std::string idNum;
    std::uint32_t balance;
};

void printPeople(Person p) {
    std::cout << std::setw(18) << std::left << "Name:" << p.name << std::endl;
    std::cout << std::setw(18) << std::left <<
        "Position:" << p.position << std::endl;
    std::cout << std::setw(18) << std::left <<
        "ID Number:" << p.idNum << std::endl;
    std::cout << std::setw(18) << std::left <<
        "Vacation Balance:" << p.balance << std::endl;
    std::cout << std::endl;
}

int main() {
    Person p1 = {"Hubert J. Farnsworth", "Professor", "B0SS", 2000};
    printPeople(p1);
    Person p2 = {.name = "Bender B. Rodriguez", .position = "Bender",
            .idNum = "BMSMA", .balance = 0};
    printPeople(p2);
    Person p3;
    p3.name = "Turanga Leela";
    p3.position = "Spaceship Pilot";
    p3.idNum = "1BDI";
    p3.balance = 1234;
    printPeople(p3);
    return 0;
}
