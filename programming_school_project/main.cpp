
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <ctime>

using namespace std;

// Account information
struct Account {
    int accountNumber;
    string name;
    string pin;
    double balance;
};

// Transaction information
struct Transaction {
    int accountNumber;
    string type;
    double amount;
    double balanceAfter;
};

// File names
const string ACCOUNT_FILE = "accounts.csv";
const string TRANSACTION_FILE = "transaction.csv";

vector<Account> accounts;

// Get a number safely from the user
int getNumber(string message) {
    int number;

    while (true) {
        cout << message;

        if (cin >> number) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return number;
        }

        cout << "Invalid input. Enter a whole number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Get a money amount safely
double getAmount(string message) {
    double amount;

    while (true) {
        cout << message;

        if (cin >> amount && amount > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return amount;
        }

        cout << "Enter a valid amount greater than zero.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Load accounts from accounts.csv
void loadAccounts() {
    ifstream file(ACCOUNT_FILE);
    string line;

    if (!file.is_open()) {
        return;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string numberText, name, pin, balanceText;

        if (!getline(ss, numberText, ',')) continue;
        if (!getline(ss, name, ',')) continue;
        if (!getline(ss, pin, ',')) continue;
        if (!getline(ss, balanceText, ',')) continue;

        try {
            Account account;
            account.accountNumber = stoi(numberText);
            account.name = name;
            account.pin = pin;
            account.balance = stod(balanceText);

            if (account.accountNumber > 0 && account.balance >= 0) {
                accounts.push_back(account);
            }
        } catch (...) {
            // Skip the CSV heading or an invalid row.
        }
    }

    file.close();
}

// Save all accounts to accounts.csv
void saveAccounts() {
    ofstream file(ACCOUNT_FILE);

    if (!file.is_open()) {
        cout << "Error: Could not save accounts.\n";
        return;
    }

    file << "account_number,name,pin,balance\n";
    file << fixed << setprecision(2);

    for (const Account& account : accounts) {
        file << account.accountNumber << ","
             << account.name << ","
             << account.pin << ","
             << account.balance << "\n";
    }

    file.close();
}

// Add a transaction to transaction.csv
void saveTransaction(int accountNumber, string type,
                     double amount, double balanceAfter) {
    ifstream checkFile(TRANSACTION_FILE);
    bool fileExists = checkFile.good();
    checkFile.close();

    ofstream file(TRANSACTION_FILE, ios::app);

    if (!file.is_open()) {
        cout << "Warning: Could not save transaction history.\n";
        return;
    }

    // Add a heading when creating a new file
    if (!fileExists) {
        file << "account_number,type,amount,balance_after\n";
    }

    file << fixed << setprecision(2);
    file << accountNumber << ","
         << type << ","
         << amount << ","
         << balanceAfter << "\n";

    file.close();
}

// Find an account using its account number
int findAccount(int accountNumber) {
    for (int i = 0; i < static_cast<int>(accounts.size()); i++) {
        if (accounts[i].accountNumber == accountNumber) {
            return i;
        }
    }

    return -1;
}

// Create a new bank account
void createAccount() {
    Account account;

    cout << "\n--- CREATE ACCOUNT ---\n";

    int highestNumber = 1000;

    for (const Account& existing : accounts) {
        if (existing.accountNumber > highestNumber) {
            highestNumber = existing.accountNumber;
        }
    }

    account.accountNumber = highestNumber + 1;

    cout << "Enter your full name: ";
    getline(cin, account.name);

    if (account.name.empty() || account.name.find(',') != string::npos) {
        cout << "Name cannot be empty or contain a comma.\n";
        return;
    }

    cout << "Create a four-digit PIN: ";
    cin >> account.pin;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (account.pin.length() != 4) {
        cout << "PIN must contain exactly four characters.\n";
        return;
    }

    for (char digit : account.pin) {
        if (digit < '0' || digit > '9') {
            cout << "PIN must contain digits only.\n";
            return;
        }
    }

    account.balance = 0.0;

    accounts.push_back(account);
    saveAccounts();

    cout << "\nAccount created successfully!\n";
    cout << "Your account number is: "
         << account.accountNumber << "\n";
    cout << "Keep your account number and PIN safe.\n";
}

// Display the account balance
void checkBalance(const Account& account) {
    cout << fixed << setprecision(2);
    cout << "\nAccount holder: " << account.name << "\n";
    cout << "Account number: " << account.accountNumber << "\n";
    cout << "Current balance: KSh " << account.balance << "\n";
}

// Deposit money
void depositMoney(int index) {
    double amount = getAmount("Enter amount to deposit (KSh): ");

    accounts[index].balance += amount;
    saveAccounts();

    saveTransaction(accounts[index].accountNumber,
                    "DEPOSIT", amount, accounts[index].balance);

    cout << "Deposit successful.\n";
    cout << "New balance: KSh "
         << fixed << setprecision(2)
         << accounts[index].balance << "\n";
}

// Withdraw money
void withdrawMoney(int index) {
    double amount = getAmount("Enter amount to withdraw (KSh): ");

    if (amount > accounts[index].balance) {
        cout << "Insufficient balance. Withdrawal cancelled.\n";
        return;
    }

    accounts[index].balance -= amount;
    saveAccounts();

    saveTransaction(accounts[index].accountNumber,
                    "WITHDRAWAL", amount, accounts[index].balance);

    cout << "Withdrawal successful.\n";
    cout << "New balance: KSh "
         << fixed << setprecision(2)
         << accounts[index].balance << "\n";
}

// Display transaction history for an account
void viewTransactions(int accountNumber) {
    ifstream file(TRANSACTION_FILE);
    string line;
    bool found = false;

    cout << "\n--- TRANSACTION HISTORY ---\n";

    if (!file.is_open()) {
        cout << "No transaction history is available yet.\n";
        return;
    }

    cout << left
         << setw(15) << "Type"
         << setw(15) << "Amount"
         << "Balance After\n";

    cout << "---------------------------------------------\n";

    while (getline(file, line)) {
        stringstream ss(line);
        string numberText, type, amountText, balanceText;

        if (!getline(ss, numberText, ',')) continue;
        if (!getline(ss, type, ',')) continue;
        if (!getline(ss, amountText, ',')) continue;
        if (!getline(ss, balanceText, ',')) continue;

        try {
            int savedNumber = stoi(numberText);

            if (savedNumber == accountNumber) {
                cout << left
                     << setw(15) << type
                     << setw(15) << amountText
                     << balanceText << "\n";

                found = true;
            }
        } catch (...) {
            // Skip the CSV heading or an invalid row.
        }
    }

    if (!found) {
        cout << "No transactions found for this account.\n";
    }

    file.close();
}

// Menu displayed after a successful login
void accountMenu(int index) {
    int choice;

    do {
        cout << "\n========== M-NEST BANK ==========\n";
        cout << "Welcome, " << accounts[index].name << "\n";
        cout << "1. Check balance\n";
        cout << "2. Deposit money\n";
        cout << "3. Withdraw money\n";
        cout << "4. View transaction history\n";
        cout << "5. Logout\n";
        cout << "=================================\n";

        choice = getNumber("Choose an option: ");

        switch (choice) {
            case 1:
                checkBalance(accounts[index]);
                break;

            case 2:
                depositMoney(index);
                break;

            case 3:
                withdrawMoney(index);
                break;

            case 4:
                viewTransactions(accounts[index].accountNumber);
                break;

            case 5:
                cout << "You have logged out successfully.\n";
                break;

            default:
                cout << "Invalid option. Try again.\n";
        }

    } while (choice != 5);
}

// Log in to an existing account
void login() {
    int accountNumber;
    string pin;

    cout << "\n--- CUSTOMER LOGIN ---\n";

    accountNumber = getNumber("Enter account number: ");

    cout << "Enter your four-digit PIN: ";
    cin >> pin;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int index = findAccount(accountNumber);

    if (index == -1) {
        cout << "Account not found.\n";
        return;
    }

    if (accounts[index].pin != pin) {
        cout << "Incorrect PIN. Login failed.\n";
        return;
    }

    cout << "Login successful.\n";
    accountMenu(index);
}

// Main program
int main() {
    loadAccounts();

    int choice;

    cout << "=================================\n";
    cout << "       WELCOME TO M-NEST BANK\n";
    cout << "=================================\n";

    do {
        cout << "\nMAIN MENU\n";
        cout << "1. Create an account\n";
        cout << "2. Customer login\n";
        cout << "3. Exit\n";

        choice = getNumber("Choose an option: ");

        switch (choice) {
            case 1:
                createAccount();
                break;

            case 2:
                login();
                break;

            case 3:
                cout << "Thank you for using M-Nest Bank.\n";
                break;

            default:
                cout << "Invalid option. Choose 1, 2, or 3.\n";
        }

    } while (choice != 3);

    return 0;
}
