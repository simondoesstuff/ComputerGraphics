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

Vec3 BoundingBox::center()
{
    return (min + max) / 2;
}

Vec3 BoundingBox::size()
{
    return max - min;
}

// ---- Vec3 ---- //

Vec3 Vec3::operator+(Vec3 other)
{
    return {x + other.x, y + other.y, z + other.z};
};

Vec3 Vec3::operator+(float scalar)
{
    return {x + scalar, y + scalar, z + scalar};
};

Vec3 Vec3::operator-(Vec3 other)
{
    return {x - other.x, y - other.y, z - other.z};
};

Vec3 Vec3::operator-(float scalar)
{
    return {x - scalar, y - scalar, z - scalar};
};

Vec3 Vec3::operator*(float scalar)
{
    return {x * scalar, y * scalar, z * scalar};
};

Vec3 Vec3::operator/(float scalar)
{
    return {x / scalar, y / scalar, z / scalar};
};

Vec3 Vec3::multComps(Vec3 other)
{
    return {x * other.x, y * other.y, z * other.z};
};

float Vec3::magnitude()
{
    return sqrt(x * x + y * y + z * z);
};

Vec3 Vec3::normalized()
{
    return *this / magnitude();
};

Vec3 Vec3::reciprocal()
{
    return {1 / x, 1 / y, 1 / z};
};

Vec3 Vec3::cross(Vec3 other)
{
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x,
    };
};

/// @brief Convert the vector to array of length 4.
/// Initialized on the heap, so it must be deleted.
/// @return 
float* Vec3::toArray()
{
    return new float[4] {x, y, z, 1};
};

// ---- Polygon ---- //

Polygon::Polygon(std::vector<Vec3> vertices, Vec3 color)
{
    if (vertices.size() < 3)
    {
        throw "Polygon must have at least 3 verticies";
    }

    verticies.push_back(vertices[0]);
    this->verticies = vertices;
    this->color = color;
    
    // calculate the normal
    
    auto v1 = vertices[1] - vertices[0];
    auto v2 = vertices[2] - vertices[0];
    auto normal = v1.cross(v2);
    this->normal = normal.normalized();
}

Polygon::Polygon(std::vector<Vec3> vertices) : Polygon(vertices, {1, 1, 1}) {}

Polygon Polygon::operator+(Vec3 other)
{
    auto verticiesNext = std::vector<Vec3>();
    for (Vec3 vertex : verticies)
    {
        verticiesNext.push_back(vertex + other);
    }
    return Polygon(verticiesNext, color);
}

Polygon Polygon::operator+(float scalar)
{
    auto verticiesNext = std::vector<Vec3>();
    for (Vec3 vertex : verticies)
    {
        verticiesNext.push_back(vertex + scalar);
    }
    return Polygon(verticiesNext, color);
}

Polygon Polygon::operator-(Vec3 other)
{
    return *this + ((other) * -1);
}

Polygon Polygon::operator-(float scalar)
{
    return *this + ((scalar) * -1);
}

Polygon Polygon::reversed()
{
    auto verticiesNext = std::vector<Vec3>();
    for (int i = verticies.size() - 1; i >= 0; i--)
    {
        verticiesNext.push_back(verticies[i]);
    }
    return Polygon(verticiesNext, color);
}

Prism* Polygon::extrude(Vec3 extrusion)
{
    return new Prism(*this, extrusion);
}

BoundingBox Polygon::bounds()
{
    auto min = Vec3{INFINITY, INFINITY, INFINITY};
    auto max = Vec3{-INFINITY, -INFINITY, -INFINITY};

    for (Vec3 vertex : verticies)
    {
        min.x = std::min(min.x, vertex.x);
        min.y = std::min(min.y, vertex.y);
        min.z = std::min(min.z, vertex.z);

        max.x = std::max(max.x, vertex.x);
        max.y = std::max(max.y, vertex.y);
        max.z = std::max(max.z, vertex.z);
    }

    return BoundingBox{min, max};
}

void Polygon::draw()
{
    glColor3f(color.x, color.y, color.z);
    glNormal3f(normal.x, normal.y, normal.z);
    
    glBegin(GL_POLYGON);
    for (Vec3 vertex : verticies)
    {
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }
    
    glEnd();
}

// ---- Frustum ---- //

Frustum::Frustum(Polygon base1, Polygon base2, Vec3 color)
{
    if (base1.verticies.size() != base2.verticies.size())
    {
        throw "Base polygons must have the same number of verticies";
    }
    
    base1.color = color;
    polygons.push_back(base1);

    int n = base1.verticies.size();
    for (int i = 0; i < n; i++)
    {
        auto verticies = std::vector<Vec3>();

        int i2 = (i + 1) % n;
        int j = n - i - 1;
        int j2 = j == 0 ? n - 1 : j - 1;

        verticies.push_back(base1.verticies[i]);
        verticies.push_back(base2.verticies[j]);
        verticies.push_back(base2.verticies[j2]);
        verticies.push_back(base1.verticies[i2]);

        Polygon face = Polygon(verticies, color);
        polygons.push_back(face);
    }

    polygons.push_back(base2);
}

Frustum::Frustum(Polygon base1, Polygon base2) : Frustum(base1, base2, base1.color) {}

int Frustum::size()
{
    return polygons.size();
}

Frustum* Frustum::painted(std::vector<Vec3> colors)
{
    for (int i = 0; i < polygons.size(); i++)
    {
        auto color = colors[i % colors.size()];
        polygons[i].color = color;
    }

    return this;
}

/// @brief Dim brightness slightly according to polygon index
/// @param strength
Frustum* Frustum::varigatePaint(float strength)
{
    for (int i = 0; i < polygons.size(); i++)
    {
        auto color = polygons[i].color;
        auto brightness = (i / (float)polygons.size()) * strength;
        polygons[i].color = (color - strength) + brightness;
    }

    return this;
}

void Frustum::draw()
{
    for (int i = 0; i < polygons.size(); i++)
    {
        polygons[i].draw();
    }
}

std::vector<Polygon> Frustum::getPolygons()
{
    return polygons;
}

BoundingBox Frustum::bounds()
{
    auto min = Vec3{INFINITY, INFINITY, INFINITY};
    auto max = Vec3{-INFINITY, -INFINITY, -INFINITY};

    for (Polygon polygon : polygons)
    {
        auto polyBounds = polygon.bounds();
        auto polyMin = polyBounds.min;
        auto polyMax = polyBounds.max;

        min.x = std::min(min.x, polyMin.x);
        min.y = std::min(min.y, polyMin.y);
        min.z = std::min(min.z, polyMin.z);

        max.x = std::max(max.x, polyMax.x);
        max.y = std::max(max.y, polyMax.y);
        max.z = std::max(max.z, polyMax.z);
    }

    return BoundingBox{min, max};
}

Box* Frustum::boxed()
{
    return new Box({this});
}

// --------- Prism --------- //

Prism::Prism(Polygon base, Vec3 extrusion) : Frustum(base, base.reversed() + extrusion) {}
Prism::Prism(Polygon base1, Vec3 extrusion, Vec3 color) : Frustum(base1, base1.reversed() + extrusion, color) {}

// -------- Box -------- //

// container
Box::Box(std::vector<Drawable *> children)
{
    this->children = children;
    this->identity();
}

Box::Box(std::initializer_list<Drawable *> children)
{
    this->children = std::vector<Drawable *>();
    for (Drawable *child : children)
    {
        this->children.push_back(child);
    }
    this->identity();
}

Box::~Box()
{
    for (Drawable *child : children)
    {
        delete child;
    }
}

void Box::identity()
{
    float matrix[16] = {
        1, 0, 0, 0, // x
        0, 1, 0, 0, // y
        0, 0, 1, 0, // z
        0, 0, 0, 1, // w
    };

    for (int i = 0; i < 16; i++)
    {
        this->matrix[i] = matrix[i];
    }

    this->scaleFactor = {1, 1, 1};
}

BoundingBox Box::bounds()
{
    auto min = Vec3{INFINITY, INFINITY, INFINITY};
    auto max = Vec3{-INFINITY, -INFINITY, -INFINITY};

    for (Drawable *child : children)
    {
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

    return BoundingBox{min, max};
}

void Box::draw()
{
    glPushMatrix();

    glMultMatrixf(this->matrix);
    
    // center the box
    // auto size = this->bounds().size();
    // glTranslatef(-size.x / 2, -size.y / 2, -size.z / 2);

    for (Drawable *child : children)
    {
        child->draw();
    }

    glPopMatrix();
}

Box* Box::scale(Vec3 scale)
{
    glPushMatrix();
    glLoadMatrixf(this->matrix);
    glScalef(scale.x, scale.y, scale.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, this->matrix);
    glPopMatrix();
    this->scaleFactor = this->scaleFactor.multComps(scale);
    return this;
}

Box* Box::scale(float scale) {
    return this->scale({scale, scale, scale});
}

Box* Box::move(Vec3 pos)
{
    pos = pos.multComps(this->scaleFactor.reciprocal());
    glPushMatrix();
    glLoadMatrixf(this->matrix);
    glTranslatef(pos.x, pos.y, pos.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, this->matrix);
    glPopMatrix();
    return this;
}

Box* Box::rotate(Vec3 rot)
{
    glPushMatrix();
    glLoadMatrixf(this->matrix);
    glRotatef(rot.x, 1, 0, 0);
    glRotatef(rot.y, 0, 1, 0);
    glRotatef(rot.z, 0, 0, 1);
    glGetFloatv(GL_MODELVIEW_MATRIX, this->matrix);
    glPopMatrix();
    return this;
}