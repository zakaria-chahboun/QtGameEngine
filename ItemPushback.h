#ifndef ITEMPUSHBACK_H
#define ITEMPUSHBACK_H

#include "EntityTargetItem.h"

class Entity;

class ItemPushback : public EntityTargetItem
{
public:
    ItemPushback();

    virtual void use_(Entity *onEntity);
};

#endif // ITEMPUSHBACK_H
