#pragma once

#include <memory>
#include <string>
#include <map>
#include "Texture.h"

namespace sgl {

	class Material
	{
	public:
		Material(std::string name);

		void SetAmbientTexture(std::shared_ptr<Texture> texture);
		void SetDiffuseTexture(std::shared_ptr<Texture> texture);
		void SetNormalTexture(std::shared_ptr<Texture> texture);
		void SetMetallicTexture(std::shared_ptr<Texture> texture);
		void SetRoughnessTexture(std::shared_ptr<Texture> texture);
		void SetAlpha(float alpha);
		void SetIllum(float illum);

		void CreateAmbientTexture(void* data);
		void CreateDiffuseTexture(void* data);
		void CreateNormalTexture(void* data);
		void CreateMetallicTexture(void* data);
		void CreateRoughnessTexture(void* data);

		void AddTextures(TextureManager& texture_manager);

		std::vector<std::string> GetTexturesName();
	protected:

		std::string name_;
		std::shared_ptr<Texture> ambient_texture_;
		std::shared_ptr<Texture> diffuse_texture_;
		std::shared_ptr<Texture> normal_texture_;
		std::shared_ptr<Texture> metallic_texture_;
		std::shared_ptr<Texture> roughness_texture_;
		float alpha_ = 1.0;
		float illum_ = 0;
	};

	// Create a Quad Mesh that is on the edge of the screen.
	std::map<std::string, std::shared_ptr<Material>> LoadMaterialsFromMtl(std::string file);
}