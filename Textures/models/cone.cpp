#include "cone.h"

#define Cos(th) (float)cos(3.14159265/180*(th))
#define Sin(th) (float)sin(3.14159265/180*(th))

Box *buildCone(float height, float radius, int slices, Vec3 color) {
    std::vector<Drawable*> polygons;
    std::vector<Vec3> baseVerts;

    float dTheta = 360.0 / slices;
    
    for (int i = 0; i < slices; i++) {
        float theta = i * dTheta;
        float nextTheta = (i + 1) * dTheta;
        
        Vec3 p1 = {radius * Cos(theta), 0, radius * Sin(theta)};
        Vec3 p2 = {radius * Cos(nextTheta), 0, radius * Sin(nextTheta)};
        Vec3 p3 = {0, height, 0};
        
        Polygon *side = new Polygon({p3, p2, p1}, color);
        polygons.push_back(side);
        baseVerts.push_back(p1);
    }
    
    Polygon *base = new Polygon(baseVerts, color);
    polygons.push_back(base);
    
    return new Box(polygons);
}