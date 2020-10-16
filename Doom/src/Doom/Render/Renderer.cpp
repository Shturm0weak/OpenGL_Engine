#include "../ECS/ComponentManager.h"
#include "Renderer.h"
#include <iostream>
#include "Batch.h"
#include "../Core/Timer.h"
#include "Line.h"
#include "ViewPort.h"
#include "ParticleSystem.h"
#include "../Core/Editor.h"
#include "Instancing.h"
#include "../Core/World.h"

using namespace Doom;

void Doom::Renderer::Clear(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0f);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void Doom::Renderer::DeleteAll() {
	for (unsigned int i = 0; i < World::objects.size(); i++)
	{
		delete(World::objects[i]);
	}
	for (unsigned int i = 0; i < Renderer::collision2d.size(); i++)
	{
		delete(Renderer::collision2d[i]);
	}
	Renderer::collision2d.clear();
	World::objects.clear();
	Renderer::col_id = 0;
	Renderer::obj_id = 0;
}

void Doom::Renderer::DeleteObject(int id) {
	GameObject* go = World::objects[id];
	World::objects.erase(World::objects.begin() + id);
	Renderer::obj_id--;
	unsigned int size = World::objects.size();
	if (id != size) {
		for (unsigned int i = 0; i < size; i++)
		{
			World::objects[i]->SetId(i);
			World::objects[i]->GetLayer() = i;
		}
	}
	unsigned int childsAmount = go->GetChilds().size();
	for (unsigned int i = 0; i < childsAmount; i++)
	{
		GameObject* child = static_cast<GameObject*>(go->GetChilds()[i]);
		child->SetOwner(nullptr);
	}
	if (go->GetOwner() != nullptr) {
		GameObject* owner = static_cast<GameObject*>(go->GetOwner());
		owner->RemoveChild(go);
	}
	if (go->GetComponent<Irenderer>()->renderType == "3D") {
		go->GetComponent<Renderer3D>()->EraseFromInstancing();
	}
	delete go;
}

void Doom::Renderer::Save(const std::string filename) {
	std::ofstream out_file;
	out_file.open(filename, std::ofstream::trunc);
	if (out_file.is_open()) {
		for (unsigned int i = 0; i < World::objects.size(); i++)
		{
			GameObject* go = (GameObject*)World::objects[i];
			SpriteRenderer* sr = static_cast<SpriteRenderer*>(go->GetComponentManager()->GetComponent<Irenderer>());
			if (World::objects[i]->type.c_str() == "GameObject")
				continue;
			float* color = sr->GetColor();
			glm::vec3 scale = go->GetScale();
			out_file << go->name << "\n"
				<< go->Enable << "\n"
				<< go->type << "\n"
				<< go->GetPositions().x << " " << go->GetPositions().y << " " << go->GetPositions().z << "\n"
				<< go->GetAngle() << "\n"
				<< Editor::Instance()->axes[0] << " " << Editor::Instance()->axes[1] << " " << Editor::Instance()->axes[2] << "\n";
			if (static_cast<ComponentManager*>(World::objects[i]->GetComponentManager())->GetComponent<RectangleCollider2D>() != nullptr) {
				out_file << 1 << "\n";
				out_file << static_cast<ComponentManager*>(go->GetComponentManager())->GetComponent<RectangleCollider2D>()->offset.x
					<< " " << static_cast<ComponentManager*>(go->GetComponentManager())->GetComponent<RectangleCollider2D>()->offset.y << "\n"
					<< static_cast<ComponentManager*>(go->GetComponentManager())->GetComponent<RectangleCollider2D>()->IsTrigger << "\n"
					<< static_cast<ComponentManager*>(go->GetComponentManager())->GetComponent<RectangleCollider2D>()->GetTag() << "\n";
			}
			else {
				out_file << 0 << "\n";
				out_file << 0 << " " << 0 << "\n";
				out_file << 0 << "\n";
				out_file << "NONE" << "\n";
			}
			if (World::objects[i]->GetComponentManager()->GetComponent<Irenderer>() != nullptr && World::objects[i]->GetComponentManager()->GetComponent<Irenderer>()->renderType == "2D")
				out_file << sr->GetPathToTexture() << "\n";
			else
				out_file << "None" << "\n";
			out_file << color[0] << " " << color[1] << " " << color[2] << " " << color[3] << "\n";
			out_file << scale[0] << " " << scale[1] << " " << scale[2] << "\n";
		
			out_file << sr->mesh2D[2] << " " << sr->mesh2D[3] << " " << sr->mesh2D[6]<< " " << sr->mesh2D[7] << " "
				<< sr->mesh2D[10] << " " << sr->mesh2D[11] << " " << sr->mesh2D[14] << " " << sr->mesh2D[15] << "\n";
			if (sr->textureAtlas == nullptr) {
				out_file << 0 << "\n";
				out_file << 0 << " " << 0;
			}
			else {
				out_file << 1 << "\n";
				out_file << sr->textureAtlas->spriteWidth << " " << sr->textureAtlas->spriteHeight;
			}
			if (i + 1 != World::objects.size())
				out_file << "\n";
			delete color;

		}
	}
	else {
		std::cout << "Error: filename doesn't exist";
		out_file.close();
		return;
	}

	out_file.close();
	std::cout  << "Saved" << std::endl;
}

void Doom::Renderer::Load(const std::string filename)
{
	DeleteAll();
	bool enable = true;
	std::string name = "";
	std::string type = "";
	std::string pathtotext = "";
	std::string tag = "";
	bool hascollision = 0;
	bool istrigger = false;
	float angle = 0;
	double pos[3];
	float scale[3];
	float color[4];
	float offset[2];
	float UVs[8];
	int axes[3] = { 0,0,1 };
	bool isSprite = false;
	float spriteSize[2];
	std::ifstream in_file;
	in_file.open(filename);
	if (in_file.is_open()) {
		while (in_file.peek() != EOF) {
				in_file >> name;
				//std::cout << name << std::endl;
				in_file >> enable;
				//std::cout << enable << std::endl;
				in_file >> type;
				//std::cout << type << std::endl;
				in_file >> pos[0] >> pos[1] >> pos[2];
				//std::cout << pos[0] << "	" << pos[1] << std::endl;
				in_file >> angle;
				in_file >> axes[0] >> axes[1] >> axes[2];
				//std::cout << angle << std::endl;
				in_file >> hascollision;
				//std::cout << hascollision << std::endl;
				in_file >> offset[0] >> offset[1];
				//std::cout << offset[0] << " " << offset[1] << std::endl;
				in_file >> istrigger;
				//std::cout << istrigger << std::endl;
				in_file >> tag;
				//std::cout << tag << std::endl;
				in_file >> pathtotext;
				//std::cout << pathtotext << std::endl;
				in_file >> color[0] >> color[1] >> color[2] >> color[3];
				//std::cout << color[0] << "	" << color[1] <<  "	" << color[2] << "	" << color[3] << std::endl;
				in_file >> scale[0] >> scale[1] >> scale[2];
				//std::cout << scale[0] << "	" << scale[1] << "	" << scale[2] << std::endl;
				in_file >> UVs[0] >> UVs[1] >> UVs[2] >> UVs[3] >> UVs[4] >> UVs[5] >> UVs[6] >> UVs[7];
				in_file >> isSprite;
				in_file >> spriteSize[0] >> spriteSize[1];
				if (type == "GameObject") {		
					LoadObj<GameObject>(enable,name, pathtotext, angle, color, scale, pos, hascollision, offset, axes, istrigger, tag, UVs,isSprite,spriteSize);
				}
		}
	}
	in_file.close();
	std::cout << BOLDGREEN << "Save has been loaded" << RESET << std::endl;
}

GameObject* Doom::Renderer::SelectObject()
{
	std::vector < glm::vec2> p;
	for (unsigned int i = 0; i < GetAmountOfObjects(); i++)
	{
		GameObject* go = static_cast<GameObject*>(World::objects[i]);
		SpriteRenderer* sr = static_cast<SpriteRenderer*>(go->GetComponentManager()->GetComponent<Irenderer>());
		p.clear();
		p.push_back(glm::vec2(sr->WorldVertexPositions[0] + go->GetPositions().x, sr->WorldVertexPositions[1] + go->GetPositions().y));
		p.push_back(glm::vec2(sr->WorldVertexPositions[2] + go->GetPositions().x, sr->WorldVertexPositions[3] + go->GetPositions().y));
		p.push_back(glm::vec2(sr->WorldVertexPositions[4] + go->GetPositions().x, sr->WorldVertexPositions[5] + go->GetPositions().y));
		p.push_back(glm::vec2(sr->WorldVertexPositions[6] + go->GetPositions().x, sr->WorldVertexPositions[7] + go->GetPositions().y));
		if (ObjectCollided(p,i)) {
			if (Editor::Instance()->go != go) {
				Editor::Instance()->go = go;
				return go;
			}
		}
	}
	Editor::Instance()->go = nullptr;
	return nullptr;

}

std::vector<unsigned int> Doom::Renderer::CalculateObjectsVectors()
{
	ObjectsWithNoOwner.clear();
	ObjectsWithOwner.clear();
	for (unsigned int i = 0; i < World::objects.size(); i++)
	{
		if (World::objects[i]->GetOwner() == nullptr) {
			ObjectsWithNoOwner.push_back(World::objects[i]->GetId());
		}
		else {
			ObjectsWithOwner.push_back(World::objects[i]->GetId());
		}
	}
	return ObjectsWithNoOwner;
}

void Doom::Renderer::ShutDown()
{
	size_t sizeO = World::objects.size();
	for (size_t i = 0; i < sizeO; i++)
	{
		delete World::objects[i];
	}
	World::objects.clear();
}

void Doom::Renderer::PopBack()
{
	World::objects.pop_back();
}

unsigned int Doom::Renderer::GetAmountOfObjects()
{
	return World::objects.size();
}

const char ** Doom::Renderer::GetItems()
{
	delete[] items;
	items = new const char*[GetObjectsWithNoOwnerReference().size()];
	for (unsigned int i = 0; i < GetObjectsWithNoOwnerReference().size(); i++)
	{
		int id = GetObjectsWithNoOwnerReference()[i];
		items[i] = World::objects[id]->name.c_str();

	}
	return items;
}

GameObject * Doom::Renderer::CreateGameObject()
{
	GameObject * go = new GameObject("Unnamed", 0, 0);
	return go;
}

bool Doom::Renderer::ObjectCollided(std::vector<glm::vec2>& p,int i)
{
	glm::vec2 MousePos = glm::vec2(ViewPort::GetInstance()->GetMousePositionToWorldSpace().x, ViewPort::GetInstance()->GetMousePositionToWorldSpace().y);

	for (unsigned int a = 0; a < p.size(); a++)
	{
		{
			int b = (a + 1) % p.size();
			glm::vec2 axisProj = glm::vec2(-(p[b].y - p[a].y), p[b].x - p[a].x);

			float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
			axisProj = { axisProj.x / d, axisProj.y / d };

			float min_r1 = INFINITY, max_r1 = -INFINITY;
			for (int f = 0; f < p.size(); f++)
			{
				float q = (p[f].x * axisProj.x + p[f].y * axisProj.y);
				min_r1 = std::fmin(min_r1, q);
				max_r1 = std::fmax(max_r1, q);
			}

			float min_r2 = INFINITY, max_r2 = -INFINITY;

			float q = (MousePos.x * axisProj.x + MousePos.y * axisProj.y);
			min_r2 = std::fmin(min_r2, q);
			max_r2 = std::fmax(max_r2, q);


			if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
				return false;
		}
	}
	return true;
}

void Renderer::Render() {
	if (Window::GetCamera().type == Camera::CameraTypes::ORTHOGRAPHIC) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		Render2DObjects();
		RenderCollision();
		RenderLines();
		RenderText();
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		return;
	}			
	
	Render2DObjects();
	RenderCollision();
	Render3DObjects();
	Editor::Instance()->gizmo->Render();
	RenderLines();
	size_t size = CubeCollider3D::colliders.size();
	if (size > 0) {
		for (size_t i = 0; i < size; i++)
		{
			if(RectangleCollider2D::IsVisible)
				CubeCollider3D::colliders[i]->Render();
		}
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	RenderText();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Doom::Renderer::Render2DObjects()
{
	{
		size_t size = Renderer::objects2d.size();
		if (size > 0)
		{
			Batch* batch = Batch::GetInstance();
			batch->BeginGameObjects();
			for (size_t i = 0; i < size;i++) {
				SpriteRenderer* r = Renderer::objects2d[i];
				if (r->GetOwnerOfComponent()->Enable == true)// && sqrt(pow((go->position.x - Window::GetCamera().GetPosition().x), 2) + pow((go->position.y - Window::GetCamera().GetPosition().y), 2)) < 50 * Window::GetCamera().GetZoomLevel())
				{
					r->Render();
				}
			}
			size_t particleSize = Particle::particles.size();
			if (particleSize > 0) {

				for (size_t i = 0; i < particleSize; i++)
				{
					Particle* p = Particle::particles[i];
					if (p->Enable)
						Batch::GetInstance()->Submit(p->pos, p->view, p->color, glm::vec2(p->scaleX, p->scaleY), p->texture);
				}
			}
			batch->EndGameObjects();
		}
		
	}

	Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->BasicShader);
}

void Doom::Renderer::Render3DObjects()
{
	if(PolygonMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	//each 3D object in one drawcall
	
	for each (Renderer3D* r in Renderer::objects3d)
	{
		if (r->GetOwnerOfComponent()->Enable == true)
		{
			r->Render();
		}
	}

	//All 3D objects with one mesh in one drawcall

	Instancing::Instance()->Render();

	if (PolygonMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Doom::Renderer::BakeShadows()
{
	for each (Renderer3D* r in Renderer::objects3d)
	{
		if (r->GetOwnerOfComponent()->Enable == true)
		{
			r->BakeShadows();
		}
	}

	//All 3D objects with one mesh in one drawcall

	//Instancing::Instance()->Render();
}

void Doom::Renderer::UpdateLightSpaceMatrices()
{
	for (DirectionalLight* light : DirectionalLight::dirLights)
	{
		light->UpdateLightMatrix();
	}
}

void Doom::Renderer::RenderLines()
{
	Batch::GetInstance()->BeginLines();
	uint32_t size = Line::lines.size();
	for (uint32_t i = 0; i < size; i++)
	{
		if (Line::lines[i]->Enable) {
			Batch::GetInstance()->Submit(*Line::lines[i]);
		}
	}
	if (Editor::Instance()->drawNormals) {
		size = Editor::Instance()->normals.size();
		for (uint32_t i = 0; i < size; i++)
		{
			Batch::GetInstance()->Submit(*Editor::Instance()->normals[i]);
		}
	}
	Batch::GetInstance()->EndLines();
	Batch::GetInstance()->flushLines(Batch::GetInstance()->LineShader);
}

void Doom::Renderer::RenderText() {
	Batch::GetInstance()->flushText(Batch::GetInstance()->TextShader);
}

void Doom::Renderer::RenderCollision(){
	if (RectangleCollider2D::IsVisible == true) {
		Batch::GetInstance()->BeginGameObjects();
		for (unsigned int i = 0; i < Renderer::collision2d.size(); i++) {
			RectangleCollider2D* col = Renderer::collision2d[i];
			if (col != nullptr && col->Enable == true) {
				Batch::GetInstance()->Submit(*col);
			}
		}
		Batch::GetInstance()->EndGameObjects();
		Batch::GetInstance()->flushCollision(Batch::GetInstance()->CollisionShader);
	}
}