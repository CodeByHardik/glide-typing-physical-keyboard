#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <libinput.h>
#include <libudev.h>

using namespace std;
using namespace chrono;

// Virtual keyboard layout mapping keys to (x, y) positions
unordered_map<uint16_t, pair<int, int>> key_positions = {
    {KEY_Q, {0, 0}}, {KEY_W, {1, 0}}, {KEY_E, {2, 0}}, {KEY_R, {3, 0}}, {KEY_T, {4, 0}},
    {KEY_Y, {5, 0}}, {KEY_U, {6, 0}}, {KEY_I, {7, 0}}, {KEY_O, {8, 0}}, {KEY_P, {9, 0}},
    {KEY_A, {0, 1}}, {KEY_S, {1, 1}}, {KEY_D, {2, 1}}, {KEY_F, {3, 1}}, {KEY_G, {4, 1}},
    {KEY_H, {5, 1}}, {KEY_J, {6, 1}}, {KEY_K, {7, 1}}, {KEY_L, {8, 1}},
    {KEY_Z, {0, 2}}, {KEY_X, {1, 2}}, {KEY_C, {2, 2}}, {KEY_V, {3, 2}}, {KEY_B, {4, 2}},
    {KEY_N, {5, 2}}, {KEY_M, {6, 2}}
};

// Libinput interface struct
static int open_restricted(const char *path, int flags, void *user_data) {
    return open(path, flags);
}
static void close_restricted(int fd, void *user_data) {
    close(fd);
}
static const struct libinput_interface libinput_iface = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted
};

void track_keys() {
    struct udev *udev = udev_new();
    if (!udev) {
        cerr << "Failed to create udev object" << endl;
        return;
    }

    struct libinput *li = libinput_udev_create_context(&libinput_iface, nullptr, udev);
    if (!li) {
        cerr << "Failed to create libinput context" << endl;
        udev_unref(udev);
        return;
    }

    if (libinput_udev_assign_seat(li, "seat0") != 0) {
        cerr << "Failed to assign seat0" << endl;
        libinput_unref(li);
        udev_unref(udev);
        return;
    }

    ofstream log_file("../data/keylog.txt", ios::app);
    auto last_time = steady_clock::now();

    while (true) {
        libinput_dispatch(li);
        struct libinput_event *event;

        while ((event = libinput_get_event(li))) {
            if (libinput_event_get_type(event) == LIBINPUT_EVENT_KEYBOARD_KEY) {
                struct libinput_event_keyboard *kb_event = libinput_event_get_keyboard_event(event);
                if (!kb_event) continue;  // NULL check

                uint16_t key = libinput_event_keyboard_get_key(kb_event);
                int state = libinput_event_keyboard_get_key_state(kb_event);

                if (state == LIBINPUT_KEY_STATE_PRESSED) {
                    auto it = key_positions.find(key);
                    if (it != key_positions.end()) {  // Ensure key exists in map
                        auto [x, y] = it->second;
                        auto now = steady_clock::now();
                        double time_diff = duration_cast<milliseconds>(now - last_time).count();
                        last_time = now;

                        log_file << key << " " << x << " " << y << " " << time_diff << "ms" << endl;
                        cout << "Key: " << key << " Pos: (" << x << "," << y << ") Time: " << time_diff << "ms" << endl;
                    }
                }
            }
            libinput_event_destroy(event);
        }
    }

    libinput_unref(li);
    udev_unref(udev);
}

int main() {
    cout << "Glide Typing Tracker Running..." << endl;
    track_keys();
    return 0;
}

