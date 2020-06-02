#include "Material.h"
#include <iterator>
#include <fstream>
#include <sstream>

namespace sgl {

	std::map<std::string, std::shared_ptr<Material>> LoadMaterialsFromMtl(std::string file)
	{
		std::map<std::string, std::shared_ptr<Material>> materials;
		std::ifstream ifs;
		ifs.open(file, std::ifstream::in);
		if (!ifs.is_open())
		{
			throw std::runtime_error("Couldn't open file: " + file);
		}
		std::shared_ptr<Material> material;
		std::string name;
		while (!ifs.eof())
		{
			std::string line = "";
			if (!std::getline(ifs, line)) break;
			if (line.empty()) continue;
			std::istringstream iss(line);
			std::string dump;
			if (!(iss >> dump))
			{
				throw std::runtime_error(
					"Error parsing file: " + file + " no token found.");
			}

			if (dump == "newmtl")
			{
				if (material != nullptr)
				{
					materials[name] = material;
				}
				if (!(iss >> name))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no name found in newmtl.");
				}
				material = std::make_shared<Material>(name);
			}
			else if (dump == "map_Ka") {
				std::string path;
				if (!(iss >> path))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no path found in map_Ka.");
				}
				material->SetAmbientTexture(std::make_shared<Texture>(path));
			}
			else if (dump == "map_Kd") {
				std::string path;
				if (!(iss >> path))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no path found in map_Kd.");
				}
				material->SetDiffuseTexture(std::make_shared<Texture>(path));
			}
			else if (dump == "map_norm") {
				std::string path;
				if (!(iss >> path))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no path found in map_norm.");
				}
				material->SetNormalTexture(std::make_shared<Texture>(path));
			}
			else if (dump == "map_Pm") {
				std::string path;
				if (!(iss >> path))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no path found in map_Pm.");
				}
				material->SetMetallicTexture(std::make_shared<Texture>(path));
			}
			else if (dump == "map_Pr") {
				std::string path;
				if (!(iss >> path))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no path found in map_Pr.");
				}
				material->SetRoughnessTexture(std::make_shared<Texture>(path));
			}
			else if (dump == "d") {
				float alpha;
				if (!(iss >> alpha))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no value found in d.");
				}
				material->SetAlpha(alpha);
			}
			else if (dump == "illum") {
				float illum;
				if (!(iss >> illum))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no value found in illum.");
				}
				material->SetIllum(illum);
			}
			else if (dump == "Ka") {
				float v[3];
				if (!(iss >> v[0]))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no r found in Ka.");
				}
				if (!(iss >> v[1]))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no g found in Ka.");
				}
				if (!(iss >> v[2]))
				{
					throw std::runtime_error(
						"Error parsing file : " +
						file +
						" no b found in Ka.");
				}

				material->CreateAmbientTexture(v);
			}
			else if (dump == "Kd") {
			float v[3];
			if (!(iss >> v[0]))
			{
				throw std::runtime_error(
					"Error parsing file : " +
					file +
					" no r found in Kd.");
			}
			if (!(iss >> v[1]))
			{
				throw std::runtime_error(
					"Error parsing file : " +
					file +
					" no g found in Kd.");
			}
			if (!(iss >> v[2]))
			{
				throw std::runtime_error(
					"Error parsing file : " +
					file +
					" no b found in Kd.");
			}

			material->CreateDiffuseTexture(v);
			}
			else if (dump == "norm") {
			float v[3];
			if (!(iss >> v[0]))
			{
				throw std::runtime_error(
					"Error parsing file : " +
					file +
					" no r found in norm.");
			}
			if (!(iss >> v[1]))
			{
				throw std::runtime_error(
					"Error parsing file : " +
					file +
					" no g found in norm.");
			}
			if (!(iss >> v[2]))
			{
				throw std::runtime_error(
					"Error parsing file : " +
					file +
					" no b found in norm.");
			}

			material->CreateNormalTexture(v);
			}
			else if (dump == "Pm") {
			float v;
			if (!(iss >> v))
			{
				throw std::runtime_error(
					"Error parsing file : " +
					file +
					" no value found in Pm.");
			}

			material->CreateMetallicTexture(&v);
			}
			else if (dump == "Pr") {
			float v;
			if (!(iss >> v))
			{
				throw std::runtime_error(
					"Error parsing file : " +
					file +
					" no value found in Pr.");
			}

			material->CreateRoughnessTexture(&v);
			}
			
		}

		materials[name] = material;
		return materials;
	}


	Material::Material(std::string name) : name_(name)
	{
	}

	void Material::SetAmbientTexture(std::shared_ptr<Texture> texture)
	{
		ambient_texture_ = texture;
	}

	void Material::SetDiffuseTexture(std::shared_ptr<Texture> texture)
	{
		diffuse_texture_ = texture;
	}

	void Material::SetNormalTexture(std::shared_ptr<Texture> texture)
	{
		normal_texture_ = texture;
	}

	void Material::SetMetallicTexture(std::shared_ptr<Texture> texture)
	{
		metallic_texture_ = texture;
	}

	void Material::SetRoughnessTexture(std::shared_ptr<Texture> texture)
	{
		roughness_texture_ = texture;
	}

	void Material::SetAlpha(float alpha)
	{
		alpha_ = alpha;
	}

	void Material::SetIllum(float illum)
	{
		illum_ = illum;
	}

	void Material::CreateAmbientTexture(void* data)
	{
		ambient_texture_ = std::make_shared<Texture>(std::pair(1,1), data, PixelElementSize::FLOAT, PixelStructure::RGB);
	}

	void Material::CreateDiffuseTexture(void* data)
	{
		diffuse_texture_ = std::make_shared<Texture>(std::pair(1, 1), data, PixelElementSize::FLOAT, PixelStructure::RGB);
	}

	void Material::CreateNormalTexture(void* data)
	{
		normal_texture_ = std::make_shared<Texture>(std::pair(1, 1), data, PixelElementSize::FLOAT, PixelStructure::RGB);
	}

	void Material::CreateMetallicTexture(void* data)
	{
		metallic_texture_ = std::make_shared<Texture>(std::pair(1, 1), data, PixelElementSize::FLOAT, PixelStructure::GREY);
	}

	void Material::CreateRoughnessTexture(void* data)
	{
		roughness_texture_ = std::make_shared<Texture>(std::pair(1, 1), data, PixelElementSize::FLOAT, PixelStructure::GREY);
	}

	void Material::AddTextures(TextureManager& texture_manager)
	{
		texture_manager.AddTexture(name_ + "Ambient", ambient_texture_);
		texture_manager.AddTexture(name_ + "Diffuse", diffuse_texture_);
		texture_manager.AddTexture(name_ + "Normal", normal_texture_);
		texture_manager.AddTexture(name_ + "Metallic", metallic_texture_);
		texture_manager.AddTexture(name_ + "Roughness", roughness_texture_);
	}

	std::vector<std::string> Material::GetTexturesName()
	{
		std::vector<std::string> names;
		names.push_back(name_ + "Ambient");
		names.push_back(name_ + "Diffuse");
		names.push_back(name_ + "Normal");
		names.push_back(name_ + "Metallic");
		names.push_back(name_ + "Roughness");
		return names;
	}

}
