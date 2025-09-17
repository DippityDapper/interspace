#ifndef SDL3_FIRST_PROJECT_VEC2_H
#define SDL3_FIRST_PROJECT_VEC2_H

#include <cmath>

struct Vec2
{
    float x = 0;
    float y = 0;

    Vec2(float _x, float _y)
    {
        x = _x;
        y = _y;
    }

    Vec2(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    Vec2 operator*(float rhs) const
    {
        return { x * rhs, y * rhs };
    }

    Vec2& operator+=(const Vec2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2 operator+(const Vec2& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    Vec2 operator-(const Vec2& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    bool operator==(const Vec2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator<(const Vec2& rhs) const
    {
        return (x < rhs.x) || (x == rhs.x && y < rhs.y);
    }

    Vec2 Normalized() const
    {
        float magnitude = std::sqrt(x * x + y * y);

        if (magnitude == 0.0f)
            return {0.0f, 0.0f};

        return { x / magnitude, y / magnitude };
    }

    float DistanceTo(Vec2 to) const
    {
        return std::sqrt((x - to.x) * (x - to.x) + (y - to.y) * (y - to.y));
    }
};


#endif
