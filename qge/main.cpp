#include <QApplication>
#include "MapGrid.h"
#include "PathingMap.h"
#include "Map.h"
#include "Entity.h"
#include "ECKeyboardMover8Directional.h"
#include "ECMouseFacer.h"
#include "ECCameraFollower.h"
#include "TerrainLayer.h"
#include "WeaponSlot.h"
#include "Spear.h"
#include "Inventory.h"
#include "ECBodyThruster.h"
#include "ECEnemyAttacker.h"
#include "Axe.h"
#include "ItemRainOfSpears.h"
#include "ItemPushback.h"
#include "ECItemPickerUpper.h"
#include "InventoryViewer.h"
#include "InventoryUser.h"
#include "Sound.h"
#include "Bow.h"
#include "RangedWeaponSlot.h"
#include "ECPathMover.h"
#include "ECMapMover.h"
#include "ECCurrentMapGrabber.h"
#include "PositionalSound.h"
#include "FogWeather.h"
#include "RainWeather.h"
#include "SnowWeather.h"
#include "Utilities.h"
#include "ECFieldOfViewEmitter.h"
#include "ECChaser.h"
#include "SpriteSheet.h"
#include "ECKeyboardMover4Directional.h"
#include "ECKeyboardMoverPerspective.h"
#include "Animation.h"
#include "TopDownSprite.h"
#include "AngledSprite.h"
#include "Sprite.h"
#include "FireballLauncher.h"
#include "AnimationAttack.h"
#include "MCSpawner.h"
#include "CItemDropper.h"
#include "CHealthShower.h"
#include "DialogGui.h"
#include "ECGuiShower.h"
#include "ShopGui.h"
#include "ECMerchant.h"
#include "ItemHealthPotion.h"
#include "ItemTeleport.h"
#include "RandomImageEntity.h"
#include "QuestAcceptor.h"
#include "Quests.h"
#include "Quest.h"

using namespace qge;

Entity* player;
CItemDropper* itemDropper;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // play music in the background
    Sound* bgMusic = new Sound("qrc:/resources/sounds/music3.mp3");
    bgMusic->setVolume(50);
    bgMusic->play(-1);

    // create a MapGrid to put some Maps inside
    MapGrid* mapGrid = new MapGrid(3,3);

    // create the Maps
    Map* map1 = new Map(new PathingMap(100,100,16));
    Map* map2 = new Map(new PathingMap(50,50,64));

    // put Maps in MapGrid
    mapGrid->setMapAtPos(map1,0,1);
    mapGrid->setMapAtPos(map2,0,0);

    // create a Game to visualize the Maps
    Game* game = new Game(mapGrid,0,1);
    game->launch();

    // create an entity that is controlled via keyboard/mouse
    player = new Entity();
    player->setGroup(1);
    player->setMaxHealth(100);
    player->setHealth(100);
    player->pathingMap().fill();

    // create an EntitySprite for the entity
    SpriteSheet minitaurSpriteSheet(":/resources/graphics/characterSpritesheets/minotaur_alpha.png",24,8,128,128);
    AngledSprite* sprplayer = new AngledSprite();
    for (int i = 0, n = 8; i < n; ++i){ // for each angle
        // stand
        sprplayer->addFrames((180+45*i) % 360,"stand",minitaurSpriteSheet,Node(0,0+i),Node(3,0+i));
        for (int j = 2; j > 0; --j){
            sprplayer->addFrame(minitaurSpriteSheet.tileAt(Node(j,0+i)),"stand",(180+45*i) % 360);
        }
        // walk
        sprplayer->addFrames((180+45*i) % 360,"walk",minitaurSpriteSheet,Node(4,0+i),Node(11,0+i));

        // attack
        sprplayer->addFrames((180+45*i) % 360,"attack",minitaurSpriteSheet,Node(12,0+i),Node(18,0+i));
    }
    player->setOrigin(QPointF(64,64));
    player->setSprite(sprplayer);
    player->setPathingMapPos(QPointF(64,64));

    // add entity to a map
    map1->addEntity(player);
    player->setPos(QPointF(200,650));

    // add controllers to control the behavior of the entity
    ECMouseFacer* rotContr = new ECMouseFacer(player);
    ECKeyboardMoverPerspective* moveContr = new ECKeyboardMoverPerspective(player);
    ECCameraFollower* grabCamContr = new ECCameraFollower(player);
    ECMapMover* moveToNMC = new ECMapMover(player);
    ECCurrentMapGrabber* grabCM = new ECCurrentMapGrabber(player);
    ECItemPickerUpper* pickUpItemContr = new ECItemPickerUpper(player);

    // create some slots for the entity
    WeaponSlot* rightHandMelee = new WeaponSlot();
    rightHandMelee->setName("melee");
    rightHandMelee->setPosition(QPointF(25,50));
    player->addSlot(rightHandMelee);

    RangedWeaponSlot* rangedSlot = new RangedWeaponSlot();
    rangedSlot->setName("ranged");
    rangedSlot->setPosition(QPointF(64,45));
    player->addSlot(rangedSlot);

    // equip something in slot
    AnimationAttack* animAttack = new AnimationAttack("attack","qrc:/resources/sounds/axe.wav",10,85,90);
    player->inventory()->addItem(animAttack);
    rightHandMelee->equip(animAttack);

    // drop some items on the ground
    Axe* axeItem = new Axe();
    axeItem->setPos(QPointF(100,300));
    map1->addEntity(axeItem);

    ItemRainOfSpears* ros = new ItemRainOfSpears();
    ros->setNumOfCharges(2);
    ros->setPos(QPointF(200,200));
    map1->addEntity(ros);

    ItemPushback* pushBackItem = new ItemPushback();
    pushBackItem->setPos(QPointF(300,300));
    map1->addEntity(pushBackItem);

    // create a gui that allows visualizing/using of inventory of player
    InventoryUser* invUser = new InventoryUser(game,player->inventory());
    game->addGui(invUser);

    // add a weather effect (can add multiple at the same time, if they play nice with each other)
    RainWeather* rain = new RainWeather();
//    map1->addWeatherEffect(*rain);
//    FogWeather* fog = new FogWeather();
//    map1->addWeatherEffect(*fog);

    // create an item dropper
    itemDropper = new CItemDropper();

    // create health shower
    CHealthShower* hs = new CHealthShower();
    hs->addEntity(player);

    // create a dialog gui (to test gui shower)
    DialogGui* dg = new DialogGui();
    Response* startingText = new Response("Hello stranger, what brings thee here?");
    Choice* startTextChoice1 = new Choice("Screw you!");
    Choice* startTextChoice2 = new Choice("I'm looking for adventure!");

    dg->addResponse(startingText);
    dg->addChoice(startingText,startTextChoice1);
    dg->addChoice(startingText,startTextChoice2);
    dg->setResponse(startingText); // set initial text

    Response* screwYouResponse = new Response("You're manners are not very good...you won't last long here, kid.");
    Response* adventureResponse = new Response("I salute thee! You should talk to our village elder for some quests.");
    dg->addResponse(screwYouResponse);
    dg->addResponse(adventureResponse);
    dg->setResponseForChoice(screwYouResponse,startTextChoice1);
    dg->setResponseForChoice(adventureResponse,startTextChoice2);

    Choice* backChoice = new Choice("<back>");
    dg->addChoice(screwYouResponse, backChoice);
    dg->setResponseForChoice(startingText, backChoice);

    double guix = game->width() - dg->getGuiBoundingBox().width();
    dg->setGuiPos(QPointF(guix,0));

    // add a building
    Entity* bEntity = new Entity();
    QPixmap bPixmap(":/resources/graphics/buildings/bldg2.png");
    bPixmap = bPixmap.scaled(0.85*bPixmap.width(),0.85*bPixmap.height());
    TopDownSprite* bSprite = new TopDownSprite(bPixmap);
    bEntity->setSprite(bSprite);

    // add its pathing
    QPixmap bPMPixmap(":/resources/graphics/buildings/bldg2pathing.png");
    bPMPixmap = bPMPixmap.scaled(0.85*bPMPixmap.width(),0.85*bPMPixmap.height());
    PathingMap* bPM = new PathingMap(bPMPixmap,32);
    bEntity->setPathingMap(*bPM);

    bEntity->setPos(QPointF(600,900));
    map1->addEntity(bEntity);
    QPixmap grassPatch (":/resources/graphics/terrain/grasspatch.png");
    map1->addTerrainDecoration(grassPatch,bEntity->pos() - QPointF(grassPatch.width()/2,0));

    // add well
    Entity* well = new Entity();

    QPixmap wellPixmap(":/resources/graphics/buildings/well.png");
    wellPixmap = wellPixmap.scaledToWidth(wellPixmap.width()*0.5);
    QPixmap wellPMPixmap(":/resources/graphics/buildings/wellpathing.png");
    wellPMPixmap = wellPMPixmap.scaledToWidth(wellPMPixmap.width()*0.5);

    well->setSprite(new TopDownSprite(wellPixmap));
    well->setPathingMap(*(new PathingMap(wellPMPixmap,32)));

    well->setPos(QPointF(1300,900));
    map1->addEntity(well);

    TerrainLayer* flowers = new TerrainLayer(2,2,QPixmap(":/resources/graphics/terrain/flowersopacity.png"));
    flowers->fill();
    map1->addTerrainLayer(flowers);
    flowers->setPos(QPointF(750,500));

    bEntity->setInvulnerable(true);
    well->setInvulnerable(true);

    addRandomTrees(map2,15,"Two",8);
    addRandomTrees(map2,5,"Three",8);
    addRandomTrees(map2,5,"Four",8);

    // add weather effects in map 2
    map2->addWeatherEffect(*(new FogWeather()));
    map2->addWeatherEffect(*(new RainWeather()));

    // create a spanwer in map 2
    MCSpawner* spawner = new MCSpawner(map2,QRectF(0,0,map2->width(),map2->height()),MCSpawner::SpawnType::Spider,10,0.2);

    // create villager that sells you stuff
    // villager sprite
    SpriteSheet villagerSS(":/resources/graphics/characterSpritesheets/villager.png",17,8,91,163);
    AngledSprite* sprVillager = new AngledSprite();
    for (int i = 0, n = 8; i < n; ++i){ // for each angle
        // stand
        sprVillager->addFrames((90+45*i) % 360,"stand",villagerSS,Node(16,0+i),Node(16,0+i));

        // walk
        sprVillager->addFrames((90+45*i) % 360,"walk",villagerSS,Node(0,0+i),Node(14,0+i));
    }
    // villager
    sprVillager->play("stand",-1,1,0);
    Entity* villager = new Entity();
    villager->setInvulnerable(true);
    villager->setOrigin(QPointF(91.0/2,163.0/2));
    villager->setSprite(sprVillager);
    villager->setPathingMapPos(QPointF(91.0/2,163.0/2));
    villager->setPos(bEntity->botRight() - QPointF(150,150));
    map1->addEntity(villager);
    sprVillager->scale(0.65);
    villager->setFacingAngle(135);
    // sell
    villager->inventory()->addItem(new ItemHealthPotion(10));
    villager->inventory()->addItem(new ItemTeleport());
    villager->inventory()->addItem(new ItemRainOfSpears());
    villager->inventory()->addItem(new FireballLauncher());

    ECMerchant* mc = new ECMerchant(villager);
    mc->addEntityOfInterest(player);

    PathingMap treePM(QPixmap(":/resources/graphics/tree/treeOnepathing.png"),32);

    // create animated tree
    Entity* animatedTree = new Entity();
    TopDownSprite* animatedTreeSpr = new TopDownSprite();
    animatedTreeSpr->addFrames(":/resources/graphics/tree/animTree",30,"animTree");
    for (int i = 28, n = 1; i >= n; --i){
        std::string fullPath = ":/resources/graphics/tree/animTree/animTree" + std::to_string(i) + ".png";
        animatedTreeSpr->addFrame(QPixmap(fullPath.c_str()),"animTree");
    }
    animatedTree->setSprite(animatedTreeSpr);
    animatedTree->setPathingMap(*(new PathingMap(QPixmap(":/resources/graphics/tree/animTree/animTreepathing.png"),32)));
    animatedTree->setInvulnerable(true);
    map1->addEntity(animatedTree);
    animatedTree->sprite()->play("animTree",-1,8,0);
    animatedTree->setPos(bEntity->pos() + QPointF(225,-400));

    // create some more trees in map1
    RandomImageEntity* tree2 = new RandomImageEntity(":/resources/graphics/tree", "treeOne" , 5, *(new PathingMap(treePM)));
    RandomImageEntity* tree3 = new RandomImageEntity(":/resources/graphics/tree", "treeOne" , 5, *(new PathingMap(treePM)));
    RandomImageEntity* tree4 = new RandomImageEntity(":/resources/graphics/tree", "treeOne" , 5, *(new PathingMap(treePM)));
    RandomImageEntity* tree5 = new RandomImageEntity(":/resources/graphics/tree", "treeOne" , 5, *(new PathingMap(treePM)));
    RandomImageEntity* tree6 = new RandomImageEntity(":/resources/graphics/tree", "treeOne" , 5, *(new PathingMap(treePM)));
    addTag("scenery",{tree2,tree3,tree4,tree5});
    map1->addEntity(tree2);
    map1->addEntity(tree3);
    map1->addEntity(tree4);
    map1->addEntity(tree5);
    map1->addEntity(tree6);
    tree2->setPos(QPointF(300,300));
    tree3->setPos(QPointF(800,375));
    tree4->setPos(QPointF(1200,1400));
    tree5->setPos(QPointF(200,1400));
    tree6->setPos(QPointF(1400,200));

    // create quest giver
    Quests* quests = new Quests();
    quests->addQuest(new Quest("Kill Spiders","We have a spider problem. Please kill 10 spiders from the northern forest."));
    quests->addQuest(new Quest("Gather Gold","We are poor. Please gather 10 gold and bring it back to me."));
    quests->addQuest(new Quest("Craft nets","We are hungry. Please make some nets from the vines found in the northern forest."));
    quests->addQuest(new Quest("Craft Weapons","We have been getting raided by bandits. Please make some weapons that our villagers can use to defend themselves."));
    quests->addQuest(new Quest("Chop trees","We need to build a wall. Please chop some trees and bring back the logs."));

    QuestAcceptor* questAccepter = new QuestAcceptor(game);
    questAccepter->setQuests(quests);

    game->addGui(questAccepter);
    questAccepter->setGuiPos(QPointF(game->width() - questAccepter->getGuiBoundingBox().width(),0));

    ECGuiShower* questGuiShower = new ECGuiShower(villager,questAccepter);
    questGuiShower->addEntityOfInterest(player);

    player->inventory()->addItem(new FireballLauncher());
    player->inventory()->addItem(new ItemTeleport());
    player->inventory()->addItem(new ItemTeleport());
    player->inventory()->addItem(new ItemTeleport());
    player->inventory()->addItem(new ItemTeleport());
    player->inventory()->addItem(new ItemTeleport());
    player->inventory()->addItem(new ItemTeleport());

    return a.exec();
}