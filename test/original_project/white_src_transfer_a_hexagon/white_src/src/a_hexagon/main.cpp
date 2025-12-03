
#include <iostream>



int main() {
    try {
        std::cout << "application is starting..." << std::endl;
        
       
        
    } catch (const std::exception& e) {
        std::cerr << "application main error " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
