#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Managers/TextureManager.hpp"
#include <cassert>

namespace Graphics
{
	TextureManager::TextureManager()
	{
	}

	TextureManager::~TextureManager()
	{
	}

	void TextureManager::Init()
	{
		PreloadTextures();
	}

	const bool TextureManager::AddTexture(const char* aFileName, const unsigned int aSlot)
	{
		auto it = myLoadedTextures.find(aFileName);

		if (it != myLoadedTextures.end())
		{
			return false;
		}

		std::shared_ptr<Texture> texture = std::make_shared<Texture>();

		if (!texture->LoadDDS(aFileName))
			return false;

		if (SimpleUtilities::FindSuffix(aFileName, "_C"))
		{
			texture->SetSlot(Global_Slot_Albedo);
		}
		else if (SimpleUtilities::FindSuffix(aFileName, "_N"))
		{
			texture->SetSlot(Global_Slot_Normal);
		}
		else if (SimpleUtilities::FindSuffix(aFileName, "_M"))
		{
			texture->SetSlot(Global_Slot_Material);
		}
		else
		{
			texture->SetSlot(aSlot);
		}

		myLoadedTextures.emplace(aFileName, texture);

		return true;
	}

	void TextureManager::PreloadTextures()
	{
		if (!AddTexture("Assets\\Textures\\T_SimpleTexture_C.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\T_Cat_C.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\T_Hamster_C.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\T_CatScared_C.dds", Global_Slot_Albedo))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_CloudAnime_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_NightStars_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_DayCloud_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_AutumnForest_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");

		if (!AddTexture("Assets\\Textures\\Cubemaps\\T_Skansen_E.dds", Global_Slot_CubeMap))
			assert(false && "Failed to add Texture");
	}

	std::shared_ptr<const Texture> TextureManager::GetTexture(const char* aFilePath)
	{
		auto it = myLoadedTextures.find(aFilePath);

		if (it != myLoadedTextures.end())
		{
			return it->second;
		}
		else
		{
			unsigned int slot = Graphics::Global_Slot_Albedo;

			if (SimpleUtilities::FindSuffix(aFilePath, "_C"))
			{
				slot = Graphics::Global_Slot_Albedo;
			}
			else if (SimpleUtilities::FindSuffix(aFilePath, "_M"))
			{
				slot = Graphics::Global_Slot_Material;
			}
			else if (SimpleUtilities::FindSuffix(aFilePath, "_N"))
			{
				slot = Graphics::Global_Slot_Normal;
			}
			else if (SimpleUtilities::FindSuffix(aFilePath, "_E"))
			{
				slot = Global_Slot_CubeMap;
			}

			if (AddTexture(aFilePath, slot) == true)
			{
				it = myLoadedTextures.find(aFilePath);

				if (it != myLoadedTextures.end())
				{
					return it->second;
				}
			}
		}

		return nullptr;
	}

	std::shared_ptr<const Texture> TextureManager::GetTexture(const eTextureType aTextureType)
	{
		std::shared_ptr<const Texture> texture = nullptr;

		switch (aTextureType)
		{
		case eTextureType::Default:
			texture = GetTexture("Assets\\Textures\\T_SimpleTexture_C.dds");
			break;
		case eTextureType::Default_Albedo:
			texture = GetTexture("Assets\\Textures\\Materials\\T_DefaultMaterial_C.dds");
			break;
		case eTextureType::Default_Normal:
			texture = GetTexture("Assets\\Textures\\Materials\\T_DefaultMaterial_N.dds");
			break;
		case eTextureType::Default_Material:
			texture = GetTexture("Assets\\Textures\\Materials\\T_DefaultMaterial_M.dds");
			break;
		case eTextureType::Simple:
			texture = GetTexture("Assets\\Textures\\T_Hamster_C.dds");
			break;
		case eTextureType::DirectionalLight:
			texture = GetTexture("Assets\\Textures\\T_Sunlight_C.dds");
			break;
		}

		return texture;
	}

	std::shared_ptr<const Texture> TextureManager::GetSkyBox(const eSkyBox aSkyBox)
	{
		std::shared_ptr<const Texture> texture = nullptr;

		switch (aSkyBox)
		{
		case eSkyBox::DayCloud:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_DayCloud_E.dds");
			break;
		case eSkyBox::NightStar:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_NightStars_E.dds");
			break;
		case eSkyBox::DayGrassland:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_CloudAnime_E.dds");
			break;
		case eSkyBox::AutumnForest:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_AutumnForest_E.dds");
			break;
		case eSkyBox::TGA_Skansen:
			texture = GetTexture("Assets\\Textures\\Cubemaps\\T_Skansen_E.dds");
			break;
		}

		return texture;
	}

	std::shared_ptr<const Texture> TextureManager::GetIcon(const eIconType aIcon)
	{
		std::shared_ptr<const Texture> texture = nullptr;

		switch (aIcon)
		{
		case eIconType::FBX:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_FBX.dds");
			break;
		case eIconType::Folder:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_Folder.dds");
			break;
		case eIconType::CubeMap:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_CubeMap.dds");
			break;
		case eIconType::PNG:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_PNG.dds");
			break;
		case eIconType::JPG:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_JPG.dds");
			break;
		case eIconType::OBJ:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_OBJ.dds");
			break;
		case eIconType::MP3:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_MP3.dds");
			break;
		case eIconType::Scene:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_Scene.dds");
			break;
		case eIconType::Cursor:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_Cursor.dds");
			break;
		case eIconType::FlyScript:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_FlyScript.dds");
			break;
		case eIconType::Unknown:
			texture = GetTexture("Assets\\Textures\\Icons_Editor\\Icon_Question.dds");
			break;
		}

		return texture;
	}

}
