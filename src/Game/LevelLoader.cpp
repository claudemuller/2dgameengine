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

        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
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

            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<RigidBodyComponent>(
                    glm::vec2(
                        entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                    )
                );
            }

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

            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<AnimationComponent>(
                    entity["components"]["animation"]["num_frames"].get_or(1),
                    entity["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

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

            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<HealthComponent>(
                    static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                );
            }

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

            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<CameraFollowComponent>();
            }

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
}
