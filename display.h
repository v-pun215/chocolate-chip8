#pragma once

namespace Display {
    void init(int scale);
    void draw();
    bool handle_input();
    void shutdown();
}