#include "Device.h"
#include <stdexcept>
#include <iterator>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include "Frame.h"
#include "Render.h"
#include "Material.h"

namespace sgl {

	Device::Device(
		void* gl_context, 
		const std::pair<std::uint32_t, std::uint32_t> size) : 
		gl_context_(gl_context),
		size_(size)
	{
		// Initialize GLEW.
		if (GLEW_OK != glewInit())
		{
			throw std::runtime_error("couldn't initialize GLEW");
		}

		// Enable blending to 1 - source alpha.
		glEnable(GL_BLEND);
		error_.Display(__FILE__, __LINE__ - 1);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		error_.Display(__FILE__, __LINE__ - 1);
		// Enable Z buffer.
		glEnable(GL_DEPTH_TEST);
		error_.Display(__FILE__, __LINE__ - 1);
		glDepthFunc(GL_LEQUAL);
		error_.Display(__FILE__, __LINE__ - 1);
		// Enable seamless cube map.
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		error_.Display(__FILE__, __LINE__ - 1);
	}

	void Device::Startup(const float fov /*= 65.0f*/)
	{
		fov_ = fov;
		SetupCamera();

		// Create the physically based rendering program.
		pbr_program_ = sgl::CreateProgram("PhysicallyBasedRendering");
		pbr_program_->UniformMatrix("projection", GetProjection());
		pbr_program_->UniformMatrix("view", GetView());
		pbr_program_->UniformMatrix("model", GetModel());

		LoadFromFile("../Asset/Model/scene.obj");
	}

	void Device::Draw(const double dt)
	{
		Display(DrawTexture(dt));
	}

	void Device::Display(const std::shared_ptr<Texture>& texture)
	{
		auto program = CreateProgram("Display");
		auto quad = CreateQuadMesh(program);
		TextureManager texture_manager{};
		texture_manager.AddTexture("Display", texture);
		quad->SetTextures({ "Display" });
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		quad->Draw(texture_manager);
	}

	std::shared_ptr<Texture> Device::DrawTexture(const double dt)
	{
		auto texture = std::make_shared<Texture>(
			size_, 
			PixelElementSize::FLOAT, 
			PixelStructure::RGB);
		DrawMultiTextures({ texture }, dt);
		return texture;
	}

	void Device::DrawMultiTextures(
		const std::vector<std::shared_ptr<Texture>>& out_textures, 
		const double dt)
	{
		if (out_textures.empty())
		{
			throw std::runtime_error("Cannot draw on empty textures.");
		}

		// Setup the camera.
		SetupCamera();

		Frame frame{};
		Render render{};
		frame.BindAttach(render);
		render.BindStorage(size_);

		// Set the view port for rendering.
		glViewport(0, 0, size_.first, size_.second);
		error_.Display(__FILE__, __LINE__ - 1);

		// Clear the screen.
		glClearColor(.2f, 0.f, .2f, 1.0f);
		error_.Display(__FILE__, __LINE__ - 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		error_.Display(__FILE__, __LINE__ - 1);

		int i = 0;
		for (const auto& texture : out_textures)
		{
			frame.BindTexture(*texture, Frame::GetFrameColorAttachment(i));
			++i;
		}
		frame.DrawBuffers(static_cast<std::uint32_t>(out_textures.size()));

		for (const std::shared_ptr<Scene>& scene : scene_tree_)
		{
			const std::shared_ptr<Mesh>& mesh = scene->GetLocalMesh();
			if (!mesh)
			{
				continue;
			}

			// Draw the mesh.
			mesh->Draw(
				texture_manager_,
				perspective_,
				view_,
				scene->GetLocalModel(dt));
		}
	}

	void Device::SetupCamera()
	{
		// Set the perspective matrix.
		const float aspect =
			static_cast<float>(size_.first) / static_cast<float>(size_.second);
		perspective_ = glm::perspective(
			glm::radians(fov_),
			aspect,
			0.1f,
			100.0f);

		// Set the camera.
		view_ = camera_.GetLookAt();
	}

	void Device::LoadFromFile(std::string file)
	{

		sgl::SceneTree scene_tree{};
		{
			auto scene_root = std::make_shared<sgl::SceneMatrix>(glm::mat4(1.0));

			scene_tree.AddNode(scene_root);

			std::ifstream ifs;
			ifs.open(file, std::ifstream::in);
			if (!ifs.is_open())
			{
				throw std::runtime_error("Couldn't open file: " + file);
			}

			std::map<std::string, std::shared_ptr<Material>> materials;
			std::string mesh_str;
			std::string texture_name;
			int delta_pos = INT_MAX;
			int delta_texture = INT_MAX;
			int delta_normal = INT_MAX;
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

				if (dump == "mtllib")
				{
					std::string mtl_name;
					if (!(iss >> mtl_name))
					{
						throw std::runtime_error(
							"Error parsing file : " +
							file +
							" missing mtl_name file name.");
					}
					materials = LoadMaterialsFromMtl("../Asset/Model/" + mtl_name);

					for (const auto& material : materials)
					{
						material.second->AddTextures(texture_manager_);
					}

				}
				else if (dump == "usemtl")
				{
					if (!(iss >> texture_name))
					{
						throw std::runtime_error(
							"Error parsing file : " +
							file +
							" missing material name.");
					}
				}
				else if (dump == "o")
				{
					if (delta_pos != INT_MAX)
					{
						auto mesh = std::make_shared<Mesh>(mesh_str, delta_pos, delta_texture, delta_normal, pbr_program_);


						// Set the texture to be used in the shader.
						mesh->SetTextures(materials[texture_name]->GetTexturesName());
						scene_tree.AddNode(std::make_shared<sgl::SceneMesh>(mesh), scene_root);
					}
					mesh_str = "";
					delta_pos = INT_MAX;
					delta_texture = INT_MAX;
					delta_normal = INT_MAX;

				}
				else if (dump == "f")
				{
					for (int i = 0; i < 3; ++i)
					{
						std::string inner;
						if (!(iss >> inner))
						{
							throw std::runtime_error(
								"Error parsing file : " +
								file +
								" missing inner part of a description.");
						}
						std::array<int, 3> vi;
						std::istringstream viss(inner);
						for (int& i : vi)
						{
							std::string inner_val;
							std::getline(viss, inner_val, '/');
							if (!inner_val.empty())
							{
								i = atoi(inner_val.c_str()) - 1;
							}
							else
							{
								i = -1;
							}
						}

						if (vi[0] < delta_pos)
						{
							delta_pos = vi[0];
						}
						if (vi[1] < delta_texture)
						{
							delta_texture = vi[1];
						}
						if (vi[2] < delta_normal)
						{
							delta_normal = vi[2];
						}
					}
				}

				mesh_str += line + "\n";
			}
		}

		SetSceneTree(scene_tree);
	}

} // End namespace sgl.
