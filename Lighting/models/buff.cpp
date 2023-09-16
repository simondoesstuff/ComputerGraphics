#include "buff.h"

struct Pallette
{
    Vec3 body = {.3, .2, .1};
    Vec3 leg = {.35, .25, .1};
    Vec3 head = {.25, .1, .1};
    Vec3 horn = {1, .9, .7};
    Vec3 eye = {1, 1, 1};
    Vec3 tail = {.4, .3, .18};
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
                   .extrude({0, -2, 0});

    auto foot = Polygon({
                            {0, -2, 0},
                            {0, -2, 1},
                            {1, -2, 1},
                            {1, -2, 0},
                        },
                        pall.head)
                    .extrude({0, -.75, 0})
                    ->boxed()
                    ->scale({1.15, 1, 1.15})
                    ->move({-.08, 0, 0});

    return new Box({leg, foot});
}

Box *buildHorn()
{
    auto b1 = Polygon({
                          {0, 0, 0},
                          {0, .8, 0},
                          {0, .8, 1.2},
                          {0, 0, 1.2},
                      },
                      pall.horn)
                  .extrude({-1.5, 0, 0});

    auto b2 = Polygon({
                          {0, 0, 0},
                          {0, 0, 1.2},
                          {-.8, 0, 1.2},
                          {-.8, 0, 0},
                      },
                      pall.horn)
                  .extrude({0, 2, 0})
                  ->boxed()
                  ->move({-1.5, 0, 0});

    return (new Box({b1, b2}))
        ->scale({.4, .6, .4});
}

Box *buildEye()
{
    return Polygon({{0, 0, 0},
                    {-1, 0, 0},
                    {-1, 1, 0},
                    {0, 1, 0}},
                   pall.eye)
        .extrude({0, 0, .1})
        ->boxed()
        ->scale({.7, .5, .2})
        ->move({.952, .8, 1.88});
}

Box *buildHead()
{
    auto head = Polygon({
                            {0, 0, 0},
                            {0, 1.8, 0},
                            {2.5, 1.8, 0},
                            {2.5, 0, 0},
                        },
                        pall.head)
                    .extrude({0, 0, 1.85})
                    ->boxed()
                    ->scale({1, 1.2, 1})
                    ->move({0, -.2, 0});

    auto horn1 = buildHorn();
    auto horn2 = buildHorn()
                     ->rotate({0, 180, 0})
                     ->move({-2.5, 0, -.8});
    auto horns = (new Box({horn1, horn2}))
                     ->move({0, .8, .65});

    auto eye1 = buildEye();
    auto eye2 = buildEye()
                    ->move({1.25, 0, 0});
    auto eyes = new Box({eye1, eye2});

    auto mouth = Polygon({
                             {0, 0, 0},
                             {-1, 0, 0},
                             {-1, 1, 0},
                             {0, 1, 0},
                         },
                         pall.horn)
                     .extrude({0, 0, 1})
                     ->boxed()
                     ->scale({2.25, .7, .7})
                     ->move({2.4, -.15, 1.7});
    
    auto eyebrow = Polygon({
                             {0, 0, 0},
                             {-1, 0, 0},
                             {-1, 1, 0},
                             {0, 1, 0},
                         },
                         pall.head)
                     .extrude({0, 0, 1})
                     ->boxed()
                     ->scale({2.4, .5, .3})
                     ->move({2.45, 1.5, 1.8});

    return new Box({head, horns, eyes, mouth, eyebrow});
}

Box *buildTail(){
    auto base = Polygon({{0, 0, 0},
                         {1, 0, 0},
                         {1, 1, 0},
                         {0, 1, 0}},
                        pall.tail)
                    .extrude({0, 0, -2.5})
                    ;
    
    auto tip = Polygon({{0, 0, 0},
                        {1, 0, 0},
                        {1, 1, 0},
                        {0, 1, 0}},
                       pall.head)
                   .extrude({0, 0, -1})
                   ->boxed()
                   ->scale({1, 2, 1.3})
                   ->move({0, -.68, -2.5});
    
    return new Box({base, tip});
}

// public
Box *buildBuff()
{
    auto body = Polygon({
                            {0, 0, 0},
                            {0, 2, 0},
                            {2.5, 2, 0},
                            {2.5, 0, 0},
                        },
                        pall.body)
                    .extrude({0, 0, 4});

    auto legs = (new Box({
                     buildLeg(),
                     buildLeg()->move({2.5, 0, 0}),
                     buildLeg()->move({2.5, 0, 4}),
                     buildLeg()->move({0, 0, 4}),
                 }))
                    ->scale(.67)
                    ->move({.08, 0, .3});

    auto head = buildHead()
                    ->scale({1.1, 1.1, 1.1})
                    ->move({-.1, 1.5, 3});
    
    auto tail = buildTail()
                    ->scale({.9, .5, .5})
                    ->move({.7, .5, 0});

    return new Box({body, legs, head, tail});
}