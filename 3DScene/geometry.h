#include <vector>
#include <initializer_list>

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
    Vec3 multComps(Vec3 other);
    Vec3 reciprocal();
};

struct BoundingBox {
    Vec3 min, max;
    Vec3 center();
    Vec3 size();
};

struct Drawable {
    virtual void draw() = 0;
    virtual ~Drawable() {};
    virtual BoundingBox bounds() = 0;
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
    Prism extrude(Vec3 extrusion);
    void draw();
    BoundingBox bounds();
};

class Frustum : public Drawable {
    public:
        Frustum(Polygon base1, Polygon base2, Vec3 color);
        Frustum(Polygon base, Polygon base2);
        int size();
        Frustum painted(std::vector<Vec3> colors);
        Frustum varigatePaint(float strength);
        void draw();
        std::vector<Polygon> getPolygons();
        BoundingBox bounds();
    private:
        std::vector<Polygon> polygons;
};

class Prism : public Frustum {
    public:
        Prism(Polygon base, Vec3 extrusion, Vec3 color);
        Prism(Polygon base, Vec3 extrusion);
};

class Box : public Drawable {
    private:
        float matrix[16];
        Vec3 scaleFactor = {1, 1, 1};

    public:
        std::vector<Drawable*> children;
        
        Box(std::vector<Drawable*> children);
        Box(std::initializer_list<Drawable*> children);
        void identity();
        BoundingBox bounds();
        void draw();
        Box scale(Vec3 scale);
        Box move(Vec3 pos);
        Box rotate(Vec3 rot);
};