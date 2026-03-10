#pragma once
#include <string>

class Customer {
private:
    std::string customerId;
    std::string name;
    std::string email;
    std::string phone;

public:
    // Default Constructor
    Customer() = default;

    // Parameterized Constructor
    Customer(const std::string& id,
            const std::string& name,
            const std::string& email,
            const std::string& phone
        ) : customerId(id),name(name),email(email),phone(phone) {}
    
    // Getters
    std::string getCustomerId() const {return customerId;}
    std::string getName() const {return name;}
    std::string getEmail() const {return email;}
    std::string getPhone() const {return phone;}
    
    std::string toString() const {
        return "[Customer " + customerId + "]" + name + " | " + email + " | " + phone;
    }
};