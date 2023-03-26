#include <fstream>
#include "LevelLoader.h"
#include "Game.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"

LevelLoader::LevelLoader() {

}

LevelLoader::~LevelLoader() {

}

void
LevelLoader::LoadLevel(
    sol::state &lua,
    const std::unique_ptr<EntityManager> &entityManager,
    const std::unique_ptr<AssetStore> &assetStore,
    SDL_Renderer *renderer,
    const int levelNum
) {
    std::string levelScriptName = "./assets/scripts/Level" + std::to_string(levelNum) + ".lua";
    sol::load_result script = lua.load_file(levelScriptName);
    if (!script.valid()) {
	sol::error err = script;
	Logger::Err("Error reading Lua script: " + std::string(err.what()));
	exit(1);
    }

    lua.script_file(levelScriptName);

    sol::table level = lua["Level"];
    sol::table assets = level["assets"];

    int i = 0;
    while (true) {
	sol::optional<sol::table> hasAsset = assets[i];
	if (hasAsset == sol::nullopt) {
	    break;
	}

	sol::table asset = assets[i];
	std::string assetType = asset["type"];
	std::string assetId = asset["id"];

	if (assetType == "texture") {
	    assetStore->AddTexture(renderer, assetId, asset["file"]);
	    Logger::Info("new texture asset added");
	}

	if (assetType == "font") {
	    assetStore->AddFont(assetId, asset["file"], asset["font_size"]);
	    Logger::Info("new font asset added");
	}

	i++;
    }

    sol::table map = level["tilemap"];
    std::string mapFilePath = map["map_file"];
    std::string mapTextureAssetId = map["texture_asset_id"];
    int mapNumRows = map["num_rows"];
    int mapNumCols = map["num_cols"];
    int tileSize = map["tile_size"];
    double mapScale = map["scale"];
    std::fstream mapFile;
    mapFile.open(mapFilePath);
    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = entityManager->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(x * (mapScale * tileSize), y * (mapScale * tileSize)), glm::vec2(mapScale, mapScale), 0.0);
            tile.AddComponent<SpriteComponent>(mapTextureAssetId, tileSize, tileSize, 0, false, srcRectX, srcRectY);
        }
    }
    mapFile.close();
    Game::MapWidth = mapNumCols * tileSize * mapScale;
    Game::MapHeight = mapNumRows * tileSize * mapScale;

    sol::table entities = level["entities"];
    i = 0;
    while (true) {
        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt) {
            break;
        }

        sol::table entity = entities[i];

        Entity newEntity = entityManager->CreateEntity();

        // Tag
        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        // Group
        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        // Components
        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            // Transform
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt) {
                newEntity.AddComponent<TransformComponent>(
                    glm::vec2(
                        entity["components"]["transform"]["position"]["x"],
                        entity["components"]["transform"]["position"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["transform"]["scale"]["x"].get_or(1.0),
                        entity["components"]["transform"]["scale"]["y"].get_or(1.0)
                    ),
                    entity["components"]["transform"]["rotation"].get_or(0.0)
                );
            }

            // RigidBody
            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<RigidBodyComponent>(
                    glm::vec2(
                        entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                    )
                );
            }

            // Sprite
            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt) {
                newEntity.AddComponent<SpriteComponent>(
                    entity["components"]["sprite"]["texture_asset_id"],
                    entity["components"]["sprite"]["width"],
                    entity["components"]["sprite"]["height"],
                    entity["components"]["sprite"]["z_index"].get_or(1),
                    entity["components"]["sprite"]["fixed"].get_or(false),
                    entity["components"]["sprite"]["src_rect_x"].get_or(0),
                    entity["components"]["sprite"]["src_rect_y"].get_or(0)
                );
            }

            // Animation
            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<AnimationComponent>(
                    entity["components"]["animation"]["num_frames"].get_or(1),
                    entity["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

            // BoxCollider
            sol::optional<sol::table> collider = entity["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                newEntity.AddComponent<BoxColliderComponent>(
                    entity["components"]["boxcollider"]["width"],
                    entity["components"]["boxcollider"]["height"],
                    glm::vec2(
                        entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
                        entity["components"]["boxcollider"]["offset"]["y"].get_or(0)
                    )
                );
            }

            // Health
            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<HealthComponent>(
                    static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                );
            }

            // ProjectileEmitter
            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                newEntity.AddComponent<ProjectileEmitterComponent>(
                    glm::vec2(
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                    ),
                    static_cast<int>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(10)),
                    entity["components"]["projectile_emitter"]["friendly"].get_or(false)
                );
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<CameraFollowComponent>();
            }

            // KeyboardControlled
            sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                newEntity.AddComponent<KeyboardControlComponent>(
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["up_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["up_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["right_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["right_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["down_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["down_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["left_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["left_velocity"]["y"]
                    )
                );
            }
        }
        i++;
    }

 //    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
 //    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
 //    assetStore->AddTexture(renderer, "tree-image", "./assets/images/tree.png");
 //    assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
 //    assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
 //    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
 //    assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
 //    assetStore->AddFont("charriot-font-20", "./assets/fonts/charriot.ttf", 20);
 //    assetStore->AddFont("pico8-font-5", "./assets/fonts/pico8.ttf", 5);
 //    assetStore->AddFont("pico8-font-10", "./assets/fonts/pico8.ttf", 10);
	//
 //    int tileSize = 32;
 //    double tileScale = 2.0;
 //    int mapNumCols = 25;
 //    int mapNumRows = 20;
 //    std::fstream mapFile;
 //    mapFile.open("./assets/tilemaps/jungle.map");
	//
 //    for (int y = 0; y < mapNumRows; y++) {
	// for (int x = 0; x < mapNumCols; x++) {
	//     char ch;
	//     mapFile.get(ch);
	//     int srcRectY = std::atoi(&ch) * tileSize;
	//     mapFile.get(ch);
	//     int srcRectX = std::atoi(&ch) * tileSize;
	//     mapFile.ignore();
	//
	//     Entity tile = entityManager->CreatEntity();
	//     tile.Group(Game::Groups[Game::TILES]);
	//     tile.AddComponent<TransformComponent>(
	// 	    glm::vec2(x * (tileScale*tileSize), y * (tileScale*tileSize)),
	// 	    glm::vec2(tileScale, tileScale),
	// 	    0.0
	//     );
	//     tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY);
	// }
 //    }
 //    mapFile.close();
 //    Game::MapWidth = tileSize * mapNumCols * tileScale;
 //    Game::MapHeight = tileSize * mapNumRows * tileScale;
	//
 //    Entity radar = entityManager->CreatEntity();
 //    radar.Group(Game::Groups[Game::UI]);
 //    radar.AddComponent<TransformComponent>(glm::vec2(Game::WindowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
 //    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
 //    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
 //    radar.AddComponent<AnimationComponent>(8, 5, true);
	//
 //    Entity chopper = entityManager->CreatEntity();
 //    chopper.Tag("player");
 //    chopper.Group(Game::Groups[Game::PLAYER]);
 //    chopper.AddComponent<TransformComponent>(glm::vec2(250.0, 120.0), glm::vec2(1.0, 1.0), 0.0);
 //    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
 //    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
 //    chopper.AddComponent<AnimationComponent>(2, 15, true);
 //    chopper.AddComponent<BoxColliderComponent>(32, 32);
 //    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
 //    chopper.AddComponent<KeyboardControlComponent>(glm::vec2(0, -80), glm::vec2(80, 0), glm::vec2(0, 80), glm::vec2(-80, 0));
 //    chopper.AddComponent<CameraFollowComponent>();
 //    chopper.AddComponent<HealthComponent>(100);
	//
 //    Entity tank = entityManager->CreatEntity();
 //    tank.Group(Game::Groups[Game::ENEMIES]);
 //    tank.AddComponent<TransformComponent>(glm::vec2(450.0, 490.0), glm::vec2(1.0, 1.0), 0.0);
 //    tank.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
 //    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
 //    tank.AddComponent<BoxColliderComponent>(32, 32);
 //    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000, 3000, 10, false);
 //    tank.AddComponent<HealthComponent>(100);
	//
 //    Entity truck = entityManager->CreatEntity();
 //    truck.Group(Game::Groups[Game::ENEMIES]);
 //    truck.AddComponent<TransformComponent>(glm::vec2(770.0, 140.0), glm::vec2(1.0, 1.0), 0.0);
 //    truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
 //    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
 //    truck.AddComponent<BoxColliderComponent>(32, 32);
 //    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 2000, 5000, 10, false);
 //    truck.AddComponent<HealthComponent>(100);
	//
 //    Entity treeA = entityManager->CreatEntity();
 //    treeA.Group(Game::Groups[Game::WORLD]);
 //    treeA.AddComponent<TransformComponent>(glm::vec2(600.0, 490.0), glm::vec2(1.0, 1.0), 0.0);
 //    treeA.AddComponent<RigidBodyComponent>(glm::vec2(0));
 //    treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
 //    treeA.AddComponent<BoxColliderComponent>(16, 32);
	//
 //    Entity treeB = entityManager->CreatEntity();
 //    treeB.Group(Game::Groups[Game::WORLD]);
 //    treeB.AddComponent<TransformComponent>(glm::vec2(400.0, 490.0), glm::vec2(1.0, 1.0), 0.0);
 //    treeB.AddComponent<RigidBodyComponent>(glm::vec2(0));
 //    treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
 //    treeB.AddComponent<BoxColliderComponent>(16, 32);
	//
 //    Entity label = entityManager->CreatEntity();
 //    label.Group(Game::Groups[Game::UI]);
 //    SDL_Color green = {0, 255, 0};
 //    label.AddComponent<TextLabelComponent>(glm::vec2(Game::WindowWidth / 2 - 40, 10), "Chopper v1.0", "charriot-font-20", green);
}
