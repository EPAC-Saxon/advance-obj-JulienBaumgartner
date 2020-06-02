#pragma once

#include <memory>
#include <string>
#include <map>
#include "Texture.h"

namespace sgl {

	class Material
	{
	public:
		Material();

		void SetAmbientTexture(std::shared_ptr<Texture> texture);
		void SetDiffuseTexture(std::shared_ptr<Texture> texture);
		void SetNormalTexture(std::shared_ptr<Texture> texture);
		void SetMetallicTexture(std::shared_ptr<Texture> texture);
		void SetRoughnessTexture(std::shared_ptr<Texture> texture);
		void SetAlpha(float alpha);
		void SetIllum(float illum);
	protected:

		std::shared_ptr<Texture> ambient_texture_;
		std::shared_ptr<Texture> diffuse_texture_;
		std::shared_ptr<Texture> normal_texture_;
		std::shared_ptr<Texture> metallic_texture_;
		std::shared_ptr<Texture> roughness_texture_;
		float alpha_;
		float illum_;
	};

	// Create a Quad Mesh that is on the edge of the screen.
	std::map<std::string, std::shared_ptr<Material>> LoadMaterialsFromMtl(std::string file);
}