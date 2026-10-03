#include <iostream>

void test_world_generation();
void test_physics_fall();
void test_clock();
void test_serialization();

int main() {
    test_world_generation();
    test_physics_fall();
    test_clock();
    test_serialization();
    std::cout << "Origin tests passed.\n";
}
