#include <iostream>
#include <string>
#include <iomanip>
#include "logisticsengine.h"

using namespace std;

#ifdef _WIN32
#define CLEAR_CMD "cls"
#else
#define CLEAR_CMD "clear"
#endif

// ANSI Styles for a beautiful UI
#define CYAN    "\033[1;36m"
#define GOLD    "\033[1;33m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define GRAY    "\033[90m"
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

void clearScreen() {
    system(CLEAR_CMD);
}

void displayHeader() {
    cout << CYAN;
    cout << "                                           ____           _  __ _   _______  __\n";
    cout << "                                          / ___|_      __(_)/ _| |_| ____\\ \\/ /\n";
    cout << "                                          \\___ \\ \\ /\\ / /| | |_| __|  _|  \\  / \n";
    cout << "                                           ___) \\ V  V / | |  _| |_| |___ /  \\ \n";
    cout << "                                          |____/ \\_/\\_/  |_|_|  \\__|_____/_/\\_\\\n";
    cout << RESET;
    cout << GRAY << "                               ──────────────────────────────────────────────────────────\n" << RESET;
    cout << BOLD << "                                  S W I F T E X   L O G I S T I C S   T E R M I N A L\n" << RESET;
    cout << GRAY << "                               ──────────────────────────────────────────────────────────\n" << RESET;
}

void pauseFunc() {
    cout << GRAY << "\n  (Press Enter to return to Dashboard)" << RESET;
    cin.ignore(1000, '\n');
    cin.get();
}

int main() {
    LogisticsEngine engine;
    int choice;

    while (true) {
        clearScreen();
        displayHeader();

        // System Status Bar
        cout << GREEN << " [ONLINE]" << RESET << "  " << GRAY << "Location: Central Hub" << RESET << "\n\n";

        // Dashboard Menu - Clean 2-Column Layout
        cout << "  " << GOLD << "1." << RESET << " Add New Parcel          " << GOLD << "5." << RESET << " View Map Details\n";
        cout << "  " << GOLD << "2." << RESET << " Dispatch Warehouse      " << GOLD << "6." << RESET << " Live Transit Monitor\n";
        cout << "  " << GOLD << "3." << RESET << " Track Parcel            " << GOLD << "7." << RESET << " Cancel Parcel\n";
        cout << "  " << GOLD << "4." << RESET << " List All Inventory      " << GOLD << "8." << RESET << " Undo Last Action\n";

        cout << "\n  " << RED << "9. Save & Exit Terminal" << RESET << "\n";
        cout << GRAY << " ──────────────────────────────────────────────────────────\n" << RESET;
        cout << "  Enter Selection " << CYAN << "» " << RESET;

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        if (choice == 9) {
            cout << "\n  " << CYAN << "Syncing database... Shutdown complete." << RESET << endl;
            engine.saveToFile();
            break;
        }

        switch (choice) {
        case 1: {
            string id, dest;
            double w;
            int p;
            clearScreen();
            cout << BOLD << CYAN << "[ PARCEL REGISTRATION ]\n" << RESET;
            cout << "  ID Code: "; cin >> id;
            cin.ignore(1000, '\n');
            cout << "  Destination: "; getline(cin, dest);
            cout << "  Weight (kg): "; cin >> w;
            cout << "  Priority (1-High, 3-Low): "; cin >> p;
            engine.requestPickup(id, dest, w, p);
            cout << GREEN << "\n  ✔ Parcel successfully logged." << RESET << endl;
            pauseFunc();
            break;
        }

        case 2:
            clearScreen();
            cout << BOLD << CYAN << "[ WAREHOUSE DISPATCH ]\n" << RESET;
            engine.processNext();
            pauseFunc();
            break;

        case 3: {
            string id;
            clearScreen();
            cout << "Enter Tracking ID: "; cin >> id;
            engine.viewParcel(id);
            pauseFunc();
            break;
        }

        case 4:
            clearScreen();
            cout << BOLD << CYAN << "[ TOTAL INVENTORY LIST ]\n" << RESET;
            engine.listAll();
            pauseFunc();
            break;

        case 5:
            clearScreen();
            engine.showMap();
            pauseFunc();
            break;

        case 6:
            clearScreen();
            engine.liveMonitor(); // Live monitors usually handle their own exit
            break;

        case 7: {
            string id;
            clearScreen();
            cout << "Enter ID to Cancel: "; cin >> id;
            engine.cancelParcel(id);
            pauseFunc();
            break;
        }

        case 8:
            clearScreen();
            engine.undoLast();
            cout << GREEN << "\n  ✔ Last action reverted." << RESET << endl;
            pauseFunc();
            break;
        }

        engine.updateRealTime();
    }

    return 0;
}