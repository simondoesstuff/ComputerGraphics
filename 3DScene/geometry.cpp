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

// ------ BoundingBox ------ //

Vec3 BoundingBox::center() {
    return (min + max) / 2;
}

Vec3 BoundingBox::size() {
    return max - min;
}

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

Vec3 Vec3::multComps(Vec3 other) {
    return {x * other.x, y * other.y, z * other.z};
};

float Vec3::magnitude() {
    return sqrt(x * x + y * y + z * z);
};

Vec3 Vec3::normalized() {
    return *this / magnitude();
};

Vec3 Vec3::reciprocal() {
    return {1 / x, 1 / y, 1 / z};
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

BoundingBox Polygon::bounds() {
    auto min = Vec3{INFINITY, INFINITY, INFINITY};
    auto max = Vec3{-INFINITY, -INFINITY, -INFINITY};
    
    for (Vec3 vertex : verticies) {
        min.x = std::min(min.x, vertex.x);
        min.y = std::min(min.y, vertex.y);
        min.z = std::min(min.z, vertex.z);
        
        max.x = std::max(max.x, vertex.x);
        max.y = std::max(max.y, vertex.y);
        max.z = std::max(max.z, vertex.z);
    }
    
    return BoundingBox {min, max};
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

Prism Prism::painted(std::vector<Vec3> colors) {
    for (int i = 0; i < polygons.size(); i++) {
        auto color = colors[i % colors.size()];
        polygons[i].color = color;
    }
    
    return *this;
}

/// @brief Dim brightness slightly according to polygon index
/// @param strength 
Prism Prism::varigatePaint(float strength) {
    for (int i = 0; i < polygons.size(); i++) {
        auto color = polygons[i].color;
        auto brightness = (i / (float)polygons.size()) * strength;
        polygons[i].color = (color - strength) + brightness;
    }
    
    return *this;
}

void Prism::draw() {
    for (int i = 0; i < polygons.size(); i++) {
        polygons[i].draw();
    }
}

std::vector<Polygon> Prism::getPolygons() {
    return polygons;
}

BoundingBox Prism::bounds() {
    auto min = Vec3{INFINITY, INFINITY, INFINITY};
    auto max = Vec3{-INFINITY, -INFINITY, -INFINITY};
    
    for (Polygon polygon : polygons) {
        auto polyBounds= polygon.bounds();
        auto polyMin = polyBounds.min;
        auto polyMax = polyBounds.max;
        
        min.x = std::min(min.x, polyMin.x);
        min.y = std::min(min.y, polyMin.y);
        min.z = std::min(min.z, polyMin.z);
        
        max.x = std::max(max.x, polyMax.x);
        max.y = std::max(max.y, polyMax.y);
        max.z = std::max(max.z, polyMax.z);
    }
    
    return BoundingBox {min, max};
}

// -------- Box -------- //

// container
Box::Box(std::vector<Drawable*> children) {
    this->children = children;
    this->identity();
}

Box::Box(std::initializer_list<Drawable*> children) {
    this->children = std::vector<Drawable*>();
    for (Drawable* child : children) {
        this->children.push_back(child);
    }
    this->identity();
}

void Box::identity() {
    float matrix[16] = {
        1, 0, 0, 0, // x
        0, 1, 0, 0, // y
        0, 0, 1, 0, // z
        0, 0, 0, 1, // w
    };
    
    for (int i = 0; i < 16; i++) {
        this->matrix[i] = matrix[i];
    }
}

BoundingBox Box::bounds() {
    auto min = Vec3{INFINITY, INFINITY, INFINITY};
    auto max = Vec3{-INFINITY, -INFINITY, -INFINITY};
    
    for (Drawable* child : children) {
        auto bounds = child->bounds();
        auto childMin = bounds.min;
        auto childMax = bounds.max;
        
        min.x = std::min(min.x, childMin.x);
        min.y = std::min(min.y, childMin.y);
        min.z = std::min(min.z, childMin.z);
        
        max.x = std::max(max.x, childMax.x);
        max.y = std::max(max.y, childMax.y);
        max.z = std::max(max.z, childMax.z);
    }
    
    return BoundingBox {min, max};
}

void Box::draw() {
    glPushMatrix();

    glMultMatrixf(this->matrix);
    auto size = this->bounds().size();
    glTranslatef(-size.x / 2, -size.y / 2, -size.z / 2);
    
    for (Drawable* child : children) {
        child->draw();
    }
    
    glPopMatrix();
}

Box Box::scale(Vec3 scale) {
    glPushMatrix();
    glLoadMatrixf(this->matrix);
    glScalef(scale.x, scale.y, scale.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, this->matrix);
    glPopMatrix();
    return *this;
}

Box Box::move(Vec3 pos) {
    glPushMatrix();
    glLoadMatrixf(this->matrix);
    glTranslatef(pos.x, pos.y, pos.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, this->matrix);
    glPopMatrix();
    return *this;
}

Box Box::rotate(Vec3 rot) {
    glPushMatrix();
    glLoadMatrixf(this->matrix);
    glRotatef(rot.x, 1, 0, 0);
    glRotatef(rot.y, 0, 1, 0);
    glRotatef(rot.z, 0, 0, 1);
    glGetFloatv(GL_MODELVIEW_MATRIX, this->matrix);
    glPopMatrix();
    return *this;
}