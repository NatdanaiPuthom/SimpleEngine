#pragma once
#include "Graphics/Texture/Texture.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace Graphics
{
	enum class eTextureType
	{
		Default,
		Default_Albedo,
		Default_Normal,
		Default_Material,
		Simple,
		DirectionalLight
	};

	enum class eIconType
	{
		FBX,
		Folder,
		CubeMap,
		PNG,
		JPG,
		OBJ,
		MP3,
		Scene,
		Cursor,
		FlyScript,
		Unknown
	};

	enum class eSkyBox
	{
		DayCloud,
		NightStar,
		DayGrassland,
		AutumnForest,
		TGA_Skansen
	};
}

namespace Graphics
{
	class TextureManager final
	{
	public:
		TextureManager();
		~TextureManager();

		void Init();

		const bool AddTexture(const char* aFileName, const unsigned int aSlot);

		std::shared_ptr<const Texture> GetTexture(const char* aFilePath);
		std::shared_ptr<const Texture> GetTexture(const eTextureType aTextureType);
		std::shared_ptr<const Texture> GetSkyBox(const eSkyBox aSkyBox);
		std::shared_ptr<const Texture> GetIcon(const eIconType aIcon);
	private:
		void PreloadTextures();
	private:
		std::unordered_map<std::string, const std::shared_ptr<const Texture>> myLoadedTextures;
	};
}
