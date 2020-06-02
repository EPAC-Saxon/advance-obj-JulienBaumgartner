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
				material = std::make_shared<Material>();
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
			
		}

		materials[name] = material;
		return materials;
	}


	Material::Material()
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

}
