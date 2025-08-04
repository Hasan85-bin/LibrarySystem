#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <string>
#include <iostream>
#include <limits>

namespace InputValidator {
    // دریافت عدد صحیح از ورودی با اعتبارسنجی
    int getInt(const std::string& prompt, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max()) {
        int value;
        while (true) {
            std::cout << prompt;
            std::string input;
            std::getline(std::cin, input);
            try {
                size_t pos;
                value = std::stoi(input, &pos);
                if (pos != input.size()) throw std::invalid_argument("Extra chars");
                if (value < min || value > max) throw std::out_of_range("Out of range");
                break;
            } catch (...) {
                std::cout << "Invalid input. Please enter a valid number." << std::endl;
            }
        }
        return value;
    }

    // دریافت رشته غیرخالی
    std::string getNonEmptyString(const std::string& prompt) {
        std::string input;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            if (!input.empty()) break;
            std::cout << "Input cannot be empty. Please try again." << std::endl;
        }
        return input;
    }
}

#endif // INPUT_VALIDATOR_H
