#include <catch_wrap.hpp>
#include <physics/Octree.h>
#include <physics/Collider.h>

namespace lfw
{
    TEST_CASE("TestOctree")
    {
        SECTION("Create")
        {
            lmath::Vector3 MinBox = lmath::Vector3::construct(-100.0f, -100.0f, -100.0f);
            lmath::Vector3 MaxBox = lmath::Vector3::construct(100.0f, 100.0f, 100.0f);

            Octree octree;
            lcore::Array<ColliderSphere> spheres;
            lfw::Octree::CollisionPairArray collisionPair;

            octree.setRange(MinBox, MaxBox);

            ColliderSphere sphere;
            sphere.bsphere_.set(0.5f, 0.5f, 0.5f, 0.5f);
            spheres.push_back(sphere);
            octree.add(&spheres[0]);
            sphere.bsphere_.set(-0.5f, -0.5f, -0.5f, 0.5f);
            spheres.push_back(sphere);
            octree.add(&spheres[1]);

            collisionPair.reserve(1024);
            collisionPair.clear();
            octree.collideAll(collisionPair);

        }
    }
}
