#ifndef ASSETSTORE_H
#define ASSETSTORE_H

#include <map>
#include <string>
#include <SDL2/SDL.h>

class AssetStore {
private:
	std::map<std::string, SDL_Texture*> textures;
	// std::map<std::string, SDL_Texture*> fonts;
	// std::map<std::string, SDL_Texture*> audio;

public:
	AssetStore();
	~AssetStore();

	void ClearAssets();
	void AddTexture(SDL_Renderer* renderer, const std::string &assetId, const std::string &filePath);
	SDL_Texture *GetTexture(const std::string &assetId);
};

#endif // ASSETSTORE_H
