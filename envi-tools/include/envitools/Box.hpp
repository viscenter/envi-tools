#pragma once

namespace envitools
{
class Box
{
public:
    Box() : xmin(0), xmax(0), ymin(0), ymax(0) {}
    Box(int x_min, int y_min, int x_max, int y_max)
        : xmin(x_min), xmax(x_max), ymin(y_min), ymax(y_max)
    {
    }

    int xmin, xmax, ymin, ymax;
};
}
