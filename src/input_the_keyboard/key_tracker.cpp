#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <chrono>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

using namespace std;

vector<pair<int, int>> keyCoordinates;
chrono::steady_clock::time_point lastKeyPressTime;

void configureTerminal() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ICANON;  // Disable line buffering
    tty.c_lflag &= ~ECHO;    // Disable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    
    // Set non-blocking mode
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void resetTerminal() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ICANON;  // Enable line buffering back
    tty.c_lflag |= ECHO;    // Enable echo back
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void saveAndTriggerPython() {
    ofstream file("/home/hardik/GitHub/glide-typing-physical-keyboard/data/glide_input.txt");
    if (file.is_open()) {
        for (auto &[x, y] : keyCoordinates) {
            file << x << " " << y << endl;
        }
        file.close();
    }
    
    cout << "\nNo keypress for 3s. Calling Python script..." << endl;
    system("python3 /home/hardik/GitHub/glide-typing-physical-keyboard/src/AI/process_glide.py");
}

int main() {
    configureTerminal();
    
    cout << "Listening for keypresses...\n" << endl;
    lastKeyPressTime = chrono::steady_clock::now();

    while (true) {
        char ch;
        int bytesRead = read(STDIN_FILENO, &ch, 1);

        if (bytesRead > 0) {
            int x = ch, y = ch + 10;  // Dummy coordinates for now
            keyCoordinates.push_back({x, y});
            lastKeyPressTime = chrono::steady_clock::now();
            cout << ch << flush;
        }

        auto now = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::seconds>(now - lastKeyPressTime).count() >= 3) {
            if (!keyCoordinates.empty()) {
                saveAndTriggerPython();
                keyCoordinates.clear();
            }
            lastKeyPressTime = now;  // Reset timer
        }

        this_thread::sleep_for(10ms);  // Reduce CPU usage
    }

    resetTerminal();
    return 0;
}

