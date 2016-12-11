#ifndef COLLISIONBEHAVIOR_H
#define COLLISIONBEHAVIOR_H

class Entity;

/// An interface for behavior objects that decides what happens when two Entities
/// collide.
/// @author Abdullah Aghazadah
/// 12/8/16
class CollisionBehavior
{
public:
    /// This function will be called on the CollisionBehavior whenever two entities
    /// of interest have collided. Some behviors will damage one or both entity,
    /// some will heal, some will push one of the entities back, etc...
    virtual void onCollided(Entity* entityOne, Entity* entityTwo) = 0;
};

#endif // COLLISIONBEHAVIOR_H