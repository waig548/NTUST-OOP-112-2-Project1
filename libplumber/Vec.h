#ifndef PLUMBER_VEC_H
#define PLUMBER_VEC_H

namespace Plumber
{
    class Vec
    {
    public:
        Vec();
        Vec(int x, int y);
        Vec(const Vec &other);
        ~Vec();

        Vec &operator=(const Vec &other);

        int getX() const;
        int getY() const;

        Vec operator+(const Vec &other) const;
        Vec operator-(const Vec &other) const;
        Vec operator-() const;
        bool operator==(const Vec &other) const;
        bool operator!=(const Vec &other) const;

    private:
        int x, y;

        static Vec add(const Vec &a, const Vec &b);
        static Vec sub(const Vec &a, const Vec &b);
        static bool equals(const Vec &a, const Vec &b);
    };

} // namespace Plumber

#endif // VEC_H
