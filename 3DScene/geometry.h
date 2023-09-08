#include <vector>

struct Vec3 {
    float x, y, z;
    Vec3 operator+(Vec3 other);
    Vec3 operator+(float scalar);
    Vec3 operator-(Vec3 other);
    Vec3 operator-(float scalar);
    Vec3 operator*(float scalar);
    Vec3 operator/(float scalar);
    float magnitude();
    Vec3 normalized();
};

struct Drawable {
    virtual void draw() = 0;
    virtual ~Drawable() {};
};

class Prism;

struct Polygon : public Drawable {
    std::vector<Vec3> verticies;
    Vec3 color;
    
    Polygon(std::vector<Vec3> vertices, Vec3 color);
    Polygon(std::vector<Vec3> vertices);
    Polygon operator+(Vec3 other);
    Polygon operator+(float scalar);
    Polygon operator-(Vec3 other);
    Polygon operator-(float scalar);
    Polygon reversed();
    Polygon operator*(float scalar);
    Polygon operator/(float scalar);
    Prism extrude(Vec3 extrusion);
    void draw();
};

class Prism : public Drawable {
    public:
        Prism(Polygon base, Vec3 extrusion, Vec3 color);
        Prism(Polygon base, Vec3 extrusion);
        int size();
        void paint(std::vector<Vec3> colors);
        void varigatePaint(float strength);
        void draw();
        std::vector<Polygon> getPolygons();
    private:
        std::vector<Polygon> polygons;
};