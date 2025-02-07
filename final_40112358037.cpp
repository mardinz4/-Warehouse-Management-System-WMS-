#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdlib>
using namespace std;

// ***********************
// Definitions of constants and structures
// ***********************

const int MAX_USERS = 5;     // Maximum number of users
const int MAX_PRODUCTS = 50;  // Maximum number of products

// User structure
struct User {
    string username;
    string password;
    int wallet;     // Wallet balance
    bool isAdmin;   // Whether the user is an admin or not
};

// Product structure
struct Product {
    string name;
    int price;
    int quantity;
};

// Global arrays to store users and products
User users[MAX_USERS];
int userCount = 0;

Product products[MAX_PRODUCTS];
int productCount = 0;

// ***********************
// Helper functions
// ***********************

// Log function: every action performed by the user is logged in log.txt
void logAction(const string &action) {
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        time_t now = time(0);
        char* dt = ctime(&now);
        string timestamp(dt);
        // Remove trailing newline
        if (!timestamp.empty() && timestamp[timestamp.size()-1] == '\n')
            timestamp.erase(timestamp.size()-1);
        logFile << "[" << timestamp << "] " << action << endl;
        logFile.close();
    }
}

// Find user by username
int findUser(const string &username) {
    for (int i = 0; i < userCount; i++) {
        if (users[i].username == username)
            return i;
    }
    return -1;
}

// Find product by name (case-sensitive)
int findProduct(const string &name) {
    for (int i = 0; i < productCount; i++) {
        if (products[i].name == name)
            return i;
    }
    return -1;
}

// Display help for a given command
void showHelp(const string &cmd) {
    if (cmd == "show") {
        cout << "show <ITEM> : Show product information" << endl;
    } else if (cmd == "buy") {
        cout << "buy <ITEM> : Buy one unit of a product (if wallet has enough funds)" << endl;
    } else if (cmd == "balance") {
        cout << "balance : Show your wallet balance" << endl;
    } else if (cmd == "add") {
        cout << "add <ITEM> <PRICE> : (Admin) Add a new product with a specific price (initial quantity 10)" << endl;
    } else if (cmd == "remove") {
        cout << "remove <ITEM> : (Admin) Remove product from inventory" << endl;
    } else if (cmd == "rename") {
        cout << "rename <OLD_ITEM> <NEW_ITEM> : (Admin) Rename a product" << endl;
    } else if (cmd == "price") {
        cout << "price <ITEM> <NEW_PRICE> : (Admin) Change the price of a product" << endl;
    } else if (cmd == "credit") {
        cout << "credit <USER> <AMOUNT> : (Admin) Increase the wallet balance of a user" << endl;
    } else if (cmd == "bulk") {
        cout << "bulk <ITEM1> <QTY1> <ITEM2> <QTY2> ... : (Admin) Add products in bulk to inventory" << endl;
    } else if (cmd == "help") {
        cout << "help <COMMAND> : Display help for a specific command" << endl;
    } else {
        cout << "Command not found." << endl;
    }
}

// Save product information to a file (for future use)
void saveProducts() {
    ofstream prodFile("products.txt");
    if (prodFile.is_open()) {
        for (int i = 0; i < productCount; i++) {
            prodFile << products[i].name << " " << products[i].price << " " << products[i].quantity << "\n";
        }
        prodFile.close();
    }
}

// Function to validate string to integer conversion
// Returns false if unsuccessful.
bool stringToInt(const string &str, int &number) {
    istringstream iss(str);
    iss >> number;
    return !iss.fail() && iss.eof();
}

// ***********************
// User command processing
// ***********************

// This function processes commands entered by the user (whether admin or regular)
void processCommand(User &currentUser) {
    string input;
    while (true) {
        cout << "\n[" << currentUser.username << "]> ";
        getline(cin, input);
        if(input.empty())
            continue;

        // Log the command
        logAction(currentUser.username + " entered command: " + input);

        // Parse the command
        istringstream iss(input);
        string command;
        iss >> command;

        // Logout command to exit the user account
        if (command == "logout") {
            cout << "Logging out..." << endl;
            logAction(currentUser.username + " logged out.");
            break;
        }
        // Help command to display help
        else if (command == "help") {
            string cmd;
            iss >> cmd;
            showHelp(cmd);
        }
        // Show wallet balance
        else if (command == "balance") {
            cout << "Your wallet balance: " << currentUser.wallet << endl;
        }
        // Show product information
        else if (command == "show") {
            string item;
            iss >> item;
            int index = findProduct(item);
            if (index != -1) {
                cout << "Product: " << products[index].name
                     << " | Price: " << products[index].price
                     << " | Quantity: " << products[index].quantity << endl;
            } else {
                cout << "Product not found." << endl;
            }
        }
        // Buy product (decrease quantity and wallet balance)
        else if (command == "buy") {
            string item;
            iss >> item;
            int index = findProduct(item);
            if (index == -1) {
                cout << "Product not found." << endl;
            } else {
                if (products[index].quantity <= 0) {
                    cout << "This product is out of stock." << endl;
                } else if (currentUser.wallet < products[index].price) {
                    cout << "Insufficient wallet balance." << endl;
                } else {
                    currentUser.wallet -= products[index].price;
                    products[index].quantity -= 1;
                    cout << "Purchase successful! " << products[index].name << " has been purchased." << endl;
                }
            }
        }
        // Admin-specific commands
        else if (currentUser.isAdmin && command == "add") {
            // Add new product: add <ITEM> <PRICE>
            string item, priceStr;
            iss >> item >> priceStr;
            int price;
            if (!stringToInt(priceStr, price)) {
                cout << "Invalid price entered." << endl;
                continue;
            }
            if (findProduct(item) != -1) {
                cout << "This product has already been added." << endl;
            } else {
                if (productCount < MAX_PRODUCTS) {
                    products[productCount].name = item;
                    products[productCount].price = price;
                    products[productCount].quantity = 10; // Initial quantity of 10
                    productCount++;
                    cout << "Product " << item << " added with price " << price << " (initial quantity: 10)." << endl;
                } else {
                    cout << "Product capacity reached." << endl;
                }
            }
        }
        else if (currentUser.isAdmin && command == "remove") {
            // Remove product: remove <ITEM>
            string item;
            iss >> item;
            int index = findProduct(item);
            if (index == -1) {
                cout << "Product not found." << endl;
            } else {
                // Remove by shifting elements in the array
                for (int i = index; i < productCount - 1; i++) {
                    products[i] = products[i+1];
                }
                productCount--;
                cout << "Product " << item << " has been removed." << endl;
            }
        }
        else if (currentUser.isAdmin && command == "rename") {
            // Rename product: rename <OLD_ITEM> <NEW_ITEM>
            string oldName, newName;
            iss >> oldName >> newName;
            int index = findProduct(oldName);
            if (index == -1) {
                cout << "Product not found." << endl;
            } else {
                products[index].name = newName;
                cout << "Product successfully renamed to " << newName << "." << endl;
            }
        }
        else if (currentUser.isAdmin && command == "price") {
            // Change product price: price <ITEM> <NEW_PRICE>
            string item, newPriceStr;
            iss >> item >> newPriceStr;
            int newPrice;
            if (!stringToInt(newPriceStr, newPrice)) {
                cout << "Invalid price entered." << endl;
                continue;
            }
            int index = findProduct(item);
            if (index == -1) {
                cout << "Product not found." << endl;
            } else {
                products[index].price = newPrice;
                cout << "Price of product " << item << " has been changed to " << newPrice << "." << endl;
            }
        }
        else if (currentUser.isAdmin && command == "credit") {
            // Increase user wallet balance: credit <USER> <AMOUNT>
            string uname, amountStr;
            iss >> uname >> amountStr;
            int amount;
            if (!stringToInt(amountStr, amount)) {
                cout << "Invalid amount entered." << endl;
                continue;
            }
            int uIndex = findUser(uname);
            if (uIndex == -1) {
                cout << "User not found." << endl;
            } else {
                users[uIndex].wallet += amount;
                cout << "Wallet balance of " << uname << " increased by " << amount << "." << endl;
            }
        }
        else if (currentUser.isAdmin && command == "bulk") {
            // Bulk add products: bulk <ITEM1> <QTY1> <ITEM2> <QTY2> ...
            string token;
            while (iss >> token) {
                string item = token;
                string qtyStr;
                int qty;
                if (!(iss >> qtyStr) || !stringToInt(qtyStr, qty)) {
                    cout << "Invalid input for product " << item << endl;
                    break;
                }
                int index = findProduct(item);
                if (index == -1) {
                    // If product is not found, add it with default price of 100
                    if (productCount < MAX_PRODUCTS) {
                        products[productCount].name = item;
                        products[productCount].price = 100; // Default price
                        products[productCount].quantity = qty;
                        productCount++;
                        cout << "Product " << item << " added as a new product (default price: 100, quantity: " << qty << ")." << endl;
                    } else {
                        cout << "Product capacity reached." << endl;
                    }
                } else {
                    products[index].quantity += qty;
                    cout << "Quantity of product " << item << " increased by " << qty << "." << endl;
                }
            }
        }
        else {
            cout << "Invalid command. Use 'help' for command assistance." << endl;
        }
    }
}

// ***********************
// Main function
// ***********************

int main() {
    // Default users
    users[0] = {"admin", "admin", 0, true};
    users[1] = {"user1", "user1", 1000, false};
    users[2] = {"user2", "user2", 500, false};
    
    userCount = 3;

    // Default products (optional)
    products[0] = {"apple", 50, 20};
    products[1] = {"banana", 30, 15};
    productCount = 2;

    cout << "==== Warehouse Management System (WMS) ====" << endl;
    cout << "==== welcome MARDIN ===="<< endl;

    while (true) {
        string username, password;
        cout << "\nLogin" << endl;
        cout << "Username: ";
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);

        int uIndex = findUser(username);
        if (uIndex != -1 && users[uIndex].password == password) {
            cout << "Login successful! Welcome " << username << endl;
            logAction(username + " logged in successfully.");
            processCommand(users[uIndex]);

            cout << "\nTo exit the program, type 'exit' or press any key to login again: ";
            string choice;
            getline(cin, choice);
            if (choice == "exit")
                break;
        } else {
            cout << "Incorrect username or password." << endl;
            logAction("Failed login attempt for user: " + username);
        }
    }

    // Save product information to a file (optional)
    saveProducts();
    cout << "\nExiting the program. Goodbye!" << endl;
    return 0;
}
