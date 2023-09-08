#include <vector>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "geometry.h"

// ---- Vec3 ---- //

Vec3 Vec3::operator+(Vec3 other) {
    return {x + other.x, y + other.y, z + other.z};
};

Vec3 Vec3::operator+(float scalar) {
    return {x + scalar, y + scalar, z + scalar};
};

Vec3 Vec3::operator-(Vec3 other) {
    return {x - other.x, y - other.y, z - other.z};
};

Vec3 Vec3::operator-(float scalar) {
    return {x - scalar, y - scalar, z - scalar};
};

Vec3 Vec3::operator*(float scalar) {
    return {x * scalar, y * scalar, z * scalar};
};

Vec3 Vec3::operator/(float scalar) {
    return {x / scalar, y / scalar, z / scalar};
};

float Vec3::magnitude() {
    return sqrt(x * x + y * y + z * z);
};

Vec3 Vec3::normalized() {
    return *this / magnitude();
};

// ---- Polygon ---- //

Polygon::Polygon(std::vector<Vec3> vertices, Vec3 color) {
    if (vertices.size() < 3) {
        throw "Polygon must have at least 3 verticies";
    }

    verticies.push_back(vertices[0]);
    this->verticies = vertices;
    this->color = color;
}

Polygon::Polygon(std::vector<Vec3> vertices) : Polygon(vertices, {1, 1, 1}) {}

Polygon Polygon::operator+(Vec3 other) {
    auto verticiesNext = std::vector<Vec3>();
    for (Vec3 vertex : verticies) {
        verticiesNext.push_back(vertex + other);
    }
    return Polygon(verticiesNext, color);
}

Polygon Polygon::operator+(float scalar) {
    auto verticiesNext = std::vector<Vec3>();
    for (Vec3 vertex : verticies) {
        verticiesNext.push_back(vertex + scalar);
    }
    return Polygon(verticiesNext, color);
}

Polygon Polygon::operator-(Vec3 other) {
    return *this + ((other) * -1);
}

Polygon Polygon::operator-(float scalar) {
    return *this + ((scalar) * -1);
}

Polygon Polygon::reversed() {
    auto verticiesNext = std::vector<Vec3>();
    for (int i = verticies.size() - 1; i >= 0; i--) {
        verticiesNext.push_back(verticies[i]);
    }
    return Polygon(verticiesNext, color);
}

Prism Polygon::extrude(Vec3 extrusion) {
    return Prism(*this, extrusion);
}

void Polygon::draw() {
    glColor3f(color.x, color.y, color.z);
    glBegin(GL_POLYGON);
    for (Vec3 vertex : verticies) {
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    glEnd();
}

// ---- Prism ---- //

Prism::Prism(Polygon base, Vec3 extrusion, Vec3 color) {
    base.color = color;
    polygons.push_back(base);
    
    int n = base.verticies.size();
    for (int i = 0; i < n; i++) {
        auto verticies = std::vector<Vec3>();
        
        verticies.push_back(base.verticies[i]);
        verticies.push_back(base.verticies[i] + extrusion);
        verticies.push_back(base.verticies[(i+1) % n] + extrusion);
        verticies.push_back(base.verticies[(i+1) % n]);
        
        Polygon face = Polygon(verticies, color);
        polygons.push_back(face);
    }
    
    polygons.push_back(base.reversed() + extrusion);
}

Prism::Prism(Polygon base, Vec3 extrusion) : Prism(base, extrusion, base.color) {}

int Prism::size() {
    return polygons.size();
}

void Prism::paint(std::vector<Vec3> colors) {
    for (int i = 0; i < polygons.size(); i++) {
        auto color = colors[i % colors.size()];
        polygons[i].color = color;
    }
}

/// @brief Dim brightness slightly according to polygon index
/// @param strength 
void Prism::varigatePaint(float strength) {
    for (int i = 0; i < polygons.size(); i++) {
        auto color = polygons[i].color;
        auto brightness = (i / (float)polygons.size()) * strength;
        polygons[i].color = (color - strength) + brightness;
    }
}

void Prism::draw() {
    for (int i = 0; i < polygons.size(); i++) {
        polygons[i].draw();
    }
}

std::vector<Polygon> Prism::getPolygons() {
    return polygons;
}



