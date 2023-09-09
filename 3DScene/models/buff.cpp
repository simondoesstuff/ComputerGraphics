#include "buff.h"

struct Pallette
{
    Vec3 body = {.3, .2, .1};
    Vec3 leg = {.35, .25, .1};
    Vec3 head = {.25, .1, .1};
    Vec3 horn = {1, 1, 1};
    Vec3 eye = {1, 1, 1};
    Vec3 tail = {1, 1, 1};
} pall;

Box *buildLeg()
{
    auto leg = Polygon({
                           {0, 0, 0},
                           {0, 0, 1},
                           {1, 0, 1},
                           {1, 0, 0},
                       },
                       pall.leg)
                   .extrude({0, -2, 0})
                   ->varigatePaint(.25);

    auto foot = (new Box({Polygon({
                                      {0, -2, 0},
                                      {0, -2, 1},
                                      {1, -2, 1},
                                      {1, -2, 0},
                                  },
                                  pall.head)
                              .extrude({0, -.75, 0})
                              ->varigatePaint(.25)}))
                    ->scale({1.15, 1, 1.15});

    return new Box({leg, foot});
}

Box *buildHead() {
    auto head = Polygon({
                            {0, 0, 0},
                            {0, 2, 0},
                            {2.5, 1.8, 0},
                            {2.5, 0, 0},
                        },
                        pall.head)
                    .extrude({0, 0, 1.5})
                    ->varigatePaint(.25);

    auto horn = Polygon({
                            {.5, 1, .5},
                            {.5, 1, .75},
                            {.75, 1, .75},
                            {.75, 1, .5},
                        },
                        pall.horn)
                    .extrude({0, 1, 0})
                    ->varigatePaint(.25);

    auto eye = Polygon({
                           {.25, .5, .25},
                           {.25, .5, .75},
                           {.75, .5, .75},
                           {.75, .5, .25},
                       },
                       pall.eye)
                   .extrude({0, .5, 0})
                   ->varigatePaint(.25);

    return new Box({head, horn, eye});
}

// public
Box buildBuff()
{
    auto body = Polygon({
                            {0, 0, 0},
                            {0, 2, 0},
                            {2.5, 2, 0},
                            {2.5, 0, 0},
                        },
                        pall.body)
                    .extrude({0, 0, 4})
                    ->varigatePaint(.25);

    auto legs = (new Box({
                     buildLeg(),
                     buildLeg()->move({2.5, 0, 0}),
                     buildLeg()->move({2.5, 0, 4}),
                     buildLeg()->move({0, 0, 4}),
                 }))
                    ->scale(.67)
                    ->move({.08, 0, .3});
    
    auto head = buildHead();

    // return Box({body, legs, head});
    return Box({head});
}