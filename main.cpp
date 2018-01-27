/**
 ** This file is part of the cardcheck project.
 ** Copyright 2018 Sinipelto.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/


// This program checks if your credit/debit card is valid
// using the Luhn algorithm and some casual checks.

#include <iostream>
#include <thread>
#include <time.h>
#include <conio.h>

using namespace std;

const string digits = "0123456789";

static bool isValid = false;
static bool isExpired = true;

void backspace() {
    cout << "\b \b";
}

void luhnCheck(string numberText) {
    const size_t size = numberText.size();
    unsigned parity = size % 2;
    int sum = 0;

    // Collect digits from string
    for (unsigned i=0;i<size;i++) {
        int digit = numberText[i] - '0';
        if (i % 2 == parity) {
            // Double every second digit
            digit *= 2;
        }
        if (digit > 9) {
            digit -= 9;
        }
       sum += digit;
    }
    isValid = (sum % 10 == 0);
    return;
}

void dateCheck(int month, int year) {
    year += 2000;

    time_t timeNow = time(0);
    struct tm * now = localtime(&timeNow);
    int yearNow = now->tm_year + 1900;
    int monthNow = now->tm_mon + 1;

    if (yearNow < 0 || monthNow < 0) {
        cout << "Please check your time and date settings." << endl;
        return;
    }
    if (year < yearNow) {
        isExpired = true;
        return;
    }
    else if (year == yearNow) {
        if (month < monthNow) {
            isExpired = true;
            return;
        }
    }
    isExpired = false;
    return;
}

int main() {

    while (1) {
    string cardNumber = "";
    string expiryDate = "";

    for (int n=0;n<40;n++) {
        cout << "\n" << endl;
    }

    cout << "*** Credit Card Validator ***" << endl;
    cout << "Hit Ctrl + C to exit.\n" << endl;

    cardNumber = "";
    cout << "Please enter card number: ";
    for (int i=0;i<20;++i) {
        if ((i == 4 || i == 9 || i == 14)) {
            cout << "-";
            continue;
        }
        bool wasNumber = false;
        int x = _getch();
        if (x == 3) {
            return 0;
        }
        if (i == 19 && x != 8) {
            // Last + and not backspace
            break; // Stop collection
        }
        char num = char(x); // get char
        for (char number : digits) {
            if (num == number) {
                cout << num;
                cardNumber += num;
                wasNumber = true;
                break;
            }
        }
        if (x == 8) {
            // Backspace to remove last char
            if (i <= 0) {--i;}
            else {
                backspace();
                if (!(i-1 == 4 || i-1 == 9 || i-1 == 14)) {
                    i-=2;
                }
                else {
                    backspace();
                    i-=3;
                }
                cardNumber.pop_back();
            }
        }
        else if (!wasNumber) {
            if (i >= 0) {--i;}
            else {i=0;}
        }
    }
    if (cardNumber.size() != 16) {
        cout << "INVALID INPUT SIZE" << endl;
    }
    cout << endl;
    //    cout << cardNumber << endl;
    cout << "CARD SUCCESS\n" << endl;
    cout << endl;

    thread calc(luhnCheck, cardNumber);

    int monthInt = 0;
    int yearInt = 0;

    while (1) {
        cout << "Type in your expiration date in format MM-YY: ";
        getline(cin, expiryDate);

        if (expiryDate.size() != 5) {
            cout << "INVALID DATE SIZE" << endl;
            continue;
        }

        // Collect month and year substrings
        string month = expiryDate.substr(0, 2);
        string year = expiryDate.substr(3, 2);
        bool monthBit = false;
        bool yearBit = false;

        //        cout << month << "/" << year << endl;

        for (int i=0;i<10;i++) {
            for (int j=0;j<10;j++) {
                //                cout << "*" << i << ":" << j << "*" << endl;
                if ((month.at(0) - '0') == i && (month.at(1) - '0') == j && monthBit == false) {
                    monthBit = true;
                    //                    cout << "MONTH TRUE" << endl;
                }
                if ((year.at(0) - '0') == i && (year.at(1) - '0') == j && yearBit == false) {
                    yearBit = true;
                    //                    cout << "YEAR TRUE"<< endl;
                }
            }
            if (monthBit && yearBit) {
                break;
            }
        }
        if (monthBit && yearBit) {
            monthInt = stoi(month);
            yearInt = stoi(year);
            if (monthInt >= 1 && monthInt <= 12 && yearInt >= 0 && yearInt <= 99) {
                cout << "DATE SUCCESS\n" << endl;
                break;
            }
        }
        cout << "INVALID DATE" << endl;
    }
    thread dateCalc(dateCheck, monthInt, yearInt);

    cout << endl;

    string state = "[STATE]";
    string expiry = "[STATE]";

    calc.join(); // Wait for result

    if (isValid) {
        state = "VALID";
    }
    else {
        state = "INVALID";
    }

    dateCalc.join();
    if (isExpired) {
        expiry = "EXPIRED";
    }
    else {
        expiry = "VALID";
    }

    cout << "***RESULTS***" << endl;

    cout << "\n\nYour card number is......";
    this_thread::sleep_for(chrono::milliseconds(1200));
    cout << state << "!" << endl << endl;

    cout << "\nYour card date is......";
    this_thread::sleep_for(chrono::milliseconds(1200));
    cout << expiry << "!" << endl << endl;

    if (isValid && !isExpired) {
        cout << "Congratulations!!! Your card is fully valid!" << endl;
    }
    else {
        cout << "There might be a problem with your card.\n"
                "Try contacting your card issuer." << endl;
    }

    cout << endl << endl;
    cout << "Any key to continue...";
    _getch();
    }
}
