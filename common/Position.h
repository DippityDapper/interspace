#ifndef SDL3_FIRST_PROJECT_POSITION_H
#define SDL3_FIRST_PROJECT_POSITION_H

#include <cmath>

struct Position
{
    float x = 0;
    float y = 0;

    Position operator*(float rhs) const
    {
        return { x * rhs, y * rhs };
    }

    Position& operator+=(const Position& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Position operator+(const Position& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    Position operator-(const Position& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    bool operator==(const Position& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator<(const Position& rhs) const
    {
        return (x < rhs.x) || (x == rhs.x && y < rhs.y);
    }

    Position Normalized() const
    {
        float magnitude = std::sqrt(x * x + y * y);

        if (magnitude == 0.0f)
            return {0.0f, 0.0f};

        return { x / magnitude, y / magnitude };
    }

    float DistanceTo(Position to) const
    {
        return std::sqrt((x - to.x) * (x - to.x) + (y - to.y) * (y - to.y));
    }

    Position ConvertToCoordinateSystem(float sizeX, float sizeY)
    {
        float dx = std::floor(x / sizeX);
        float dy = std::floor(y / sizeY);
        return {dx, dy};
    }
};


#endif
