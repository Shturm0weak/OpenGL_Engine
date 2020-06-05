#include "../pch.h"
#include "Renderer.h"
#include <iostream>
#include "Batch.h"
#include "../Core/Timer.h"
#include "Line.h"
#include "ViewPort.h"
#include "ParticleSystem.h"

using namespace Doom;

void Doom::Renderer::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Doom::Renderer::DeleteAll() {
	for (unsigned int i = 0; i < Renderer2DLayer::objects2d.size(); i++)
	{
		delete(&Renderer2DLayer::objects2d[i].get());
	}
	for (unsigned int i = 0; i < Renderer2DLayer::collision2d.size(); i++)
	{
		delete(&Renderer2DLayer::collision2d[i].get());
	}
	Renderer2DLayer::collision2d.clear();
	Renderer2DLayer::objects2d.clear();
	Renderer2DLayer::col_id = 0;
	Renderer2DLayer::obj_id = 0;
}

void Doom::Renderer::DeleteObject(int id) {
	if (Renderer2DLayer::objects2d[id].get().GetCollisionReference() != nullptr) {
		int _id = Renderer2DLayer::objects2d[id].get().GetCollisionReference()->GetId();
		Renderer2DLayer* col = Renderer2DLayer::objects2d[id].get().GetCollisionReference();
		Renderer2DLayer::collision2d.erase(_id + Renderer2DLayer::collision2d.begin());	
		Renderer2DLayer::col_id--;
		unsigned int size = Renderer2DLayer::collision2d.size();
		if (_id != size) {
			for (unsigned int i = _id; i < size; i++)
			{
				Renderer2DLayer::collision2d[i].get().SetId(i);
			}
		}
		delete col;
	}
	Renderer2DLayer* go = &Renderer2DLayer::objects2d[id].get();
	Renderer2DLayer::objects2d.erase(Renderer2DLayer::objects2d.begin() + id);	
	Renderer2DLayer::obj_id--;
	unsigned int size = Renderer2DLayer::objects2d.size();
	if (id != size) {
		for (unsigned int i = 0; i < size; i++)
		{
			Renderer2DLayer::objects2d[i].get().SetId(i);
			Renderer2DLayer::objects2d[i].get().GetLayer() = i;
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
	delete go;
}

void Doom::Renderer::Save(const std::string filename) {
	std::ofstream out_file;
	out_file.open(filename, std::ofstream::trunc);
	if (out_file.is_open()) {
		for (unsigned int i = 0; i < Renderer2DLayer::objects2d.size(); i++)
		{
			GameObject* go = (GameObject*)&Renderer2DLayer::objects2d[i].get();
			if (Renderer2DLayer::objects2d[i].get().type->c_str() == "GameObject")
				continue;
			float* color = Renderer2DLayer::objects2d[i].get().GetColor();
			float* scale = Renderer2DLayer::objects2d[i].get().GetScale();
			out_file << *Renderer2DLayer::objects2d[i].get().name << "\n"
				<< *Renderer2DLayer::objects2d[i].get().type << "\n"
				<< Renderer2DLayer::objects2d[i].get().GetShaderType() << "\n"
				<< Renderer2DLayer::objects2d[i].get().GetPositions().x << " " << Renderer2DLayer::objects2d[i].get().GetPositions().y << "\n"
				<< Renderer2DLayer::objects2d[i].get().GetAngle() << "\n"
				<< Editor::Instance()->axes[0] << " " << Editor::Instance()->axes[1] << " " << Editor::Instance()->axes[2] << "\n";
			if (static_cast<ComponentManager*>(Renderer2DLayer::objects2d[i].get().GetComponentManager())->GetComponent<Collision>() != nullptr) {
				out_file << 1 << "\n";
				out_file << static_cast<ComponentManager*>(Renderer2DLayer::objects2d[i].get().GetComponentManager())->GetComponent<Collision>()->offsetX
					<< " " << static_cast<ComponentManager*>(Renderer2DLayer::objects2d[i].get().GetComponentManager())->GetComponent<Collision>()->offsetY << "\n"
					<< static_cast<ComponentManager*>(Renderer2DLayer::objects2d[i].get().GetComponentManager())->GetComponent<Collision>()->IsTrigger << "\n"
					<< static_cast<ComponentManager*>(Renderer2DLayer::objects2d[i].get().GetComponentManager())->GetComponent<Collision>()->GetTag() << "\n";
			}
			else {
				out_file << 0 << "\n";
				out_file << 0 << " " << 0 << "\n";
				out_file << 0 << "\n";
				out_file << "NONE" << "\n";
			}
			out_file << *Renderer2DLayer::objects2d[i].get().GetPathToTexture() << "\n";
			out_file << color[0] << " " << color[1] << " " << color[2] << " " << color[3] << "\n";
			out_file << scale[0] << " " << scale[1] << " " << scale[2] << "\n";
			out_file << Renderer2DLayer::objects2d[i].get().GetRenderType() << "\n";
			out_file << go->mesh2D[2] << " " << go->mesh2D[3] << " " << go->mesh2D[6]<< " " << go->mesh2D[7] << " "
				<< go->mesh2D[10] << " " << go->mesh2D[11] << " " << go->mesh2D[14] << " " << go->mesh2D[15] << "\n";
			if (go->textureAtlas == nullptr) {
				out_file << 0 << "\n";
				out_file << 0 << " " << 0;
			}
			else {
				out_file << 1 << "\n";
				out_file << go->textureAtlas->spriteWidth << " " << go->textureAtlas->spriteHeight;
			}
			if (i + 1 != Renderer2DLayer::objects2d.size())
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
	std::string name = "";
	std::string type = "";
	std::string pathtotext = "";
	std::string tag = "";
	bool hascollision = 0;
	bool istrigger = false;
	float angle = 0;
	double pos[2];
	float scale[3];
	float color[4];
	float offset[2];
	float UVs[8];
	int rendertype = 0;
	int shadertype = 1;
	int axes[3] = { 0,0,1 };
	bool isSprite = false;
	float spriteSize[2];
	std::ifstream in_file;
	in_file.open(filename);
	if (in_file.is_open()) {
		while (in_file.peek() != EOF) {
				in_file >> name;
				//std::cout << name << std::endl;
				in_file >> type;
				//std::cout << type << std::endl;
				in_file >> shadertype;
				//std::cout << shadertype << std::endl;
				in_file >> pos[0] >> pos[1];
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
				in_file >> rendertype;
				in_file >> UVs[0] >> UVs[1] >> UVs[2] >> UVs[3] >> UVs[4] >> UVs[5] >> UVs[6] >> UVs[7];
				in_file >> isSprite;
				in_file >> spriteSize[0] >> spriteSize[1];
				if (type == "GameObject") {		
					LoadObj<GameObject>(name, pathtotext, angle, color, scale, pos, shadertype, hascollision, offset, rendertype, axes, istrigger, tag, UVs,isSprite,spriteSize);
				}
		}
	}
	in_file.close();
	std::cout << GREEN << "Save has been loaded" << RESET << std::endl;
}

GameObject* Doom::Renderer::SelectObject()
{
	std::vector < glm::vec2> p;
	for (unsigned int i = 0; i < GetAmountOfObjects(); i++)
	{
		GameObject* go = static_cast<GameObject*>(&Renderer2DLayer::objects2d[i].get());
		p.clear();
		p.push_back(glm::vec2(go->WorldVertexPositions[0] + go->GetPositions().x, go->WorldVertexPositions[1] + go->GetPositions().y));
		p.push_back(glm::vec2(go->WorldVertexPositions[2] + go->GetPositions().x, go->WorldVertexPositions[3] + go->GetPositions().y));
		p.push_back(glm::vec2(go->WorldVertexPositions[4] + go->GetPositions().x, go->WorldVertexPositions[5] + go->GetPositions().y));
		p.push_back(glm::vec2(go->WorldVertexPositions[6] + go->GetPositions().x, go->WorldVertexPositions[7] + go->GetPositions().y));
		if (ObjectCollided(p,i)) {
			if (Editor::Instance()->selectedGO != go) {
				Editor::Instance()->selectedGO = go;
				return go;
			}
		}
	}
	Editor::Instance()->selectedGO = nullptr;
	return nullptr;

}

std::vector<unsigned int> Doom::Renderer::CalculateObjectsVectors()
{
	ObjectsWithNoOwner.clear();
	ObjectsWithOwner.clear();
	for (unsigned int i = 0; i < Renderer2DLayer::objects2d.size(); i++)
	{
		if (Renderer2DLayer::objects2d[i].get().GetOwner() == nullptr) {
			ObjectsWithNoOwner.push_back(Renderer2DLayer::objects2d[i].get().GetId());
		}
		else {
			ObjectsWithOwner.push_back(Renderer2DLayer::objects2d[i].get().GetId());
		}
	}
	return ObjectsWithNoOwner;
}

bool Doom::Renderer::ObjectCollided(std::vector<glm::vec2>& p,int i)
{
	glm::vec2 MousePos = glm::vec2(ViewPort::Instance()->GetMousePositionToWorldSpace().x, ViewPort::Instance()->GetMousePositionToWorldSpace().y);

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
	OrthographicCamera* camera = &Window::GetCamera();
	
	SubmitGameObjects(*camera); 
	
	size_t particleSize = Particle::particles.size();
	for (size_t i = 0; i < particleSize; i++)
	{
		Particle* p = Particle::particles[i];
		if (p->Enable)
			Batch::GetInstance()->Submit(p->pos, p->view, p->color, glm::vec2(p->scaleX,p->scaleY),p->texture);
	}
	
	//Simple render. one line in one draw call.
	/*for (unsigned int i = 0; i < Line::lines.size(); i++)
	{
		if (Line::lines[i]->Enable) {
			glLineWidth(Line::lines[i]->width);
			Line::lines[i]->OnRunning();
		}
	}
	glLineWidth(1.0f);*/
	Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->BasicShader);
	Batch::GetInstance()->Lindexcount = 0;
	RenderCollision(*camera);
	Batch::GetInstance()->BeginLines();
	unsigned int size = Line::lines.size();
	for (unsigned int i = 0; i < size; i++)
	{
		if (Line::lines[i]->Enable) {
			Batch::GetInstance()->Submit(*Line::lines[i]);
		}
	}
	Batch::GetInstance()->EndLines();
	Batch::GetInstance()->flushLines(Batch::GetInstance()->LineShader);
	RenderText();
}

void Doom::Renderer::SubmitGameObjects(OrthographicCamera& camera)
{
	//std::unique_lock<std::mutex> lock(Renderer::mtx);
	{
		//condVar.wait(lock, [] { return(isReadyToRenderFirstThread && isReadyToRenderSecondThread && isReadyToRenderThirdThread); });
		//isReadyToRenderFirstThread = false;
		//isReadyToRenderSecondThread = false;
		//isReadyToRenderThirdThread = false;
		size_t size = Renderer2DLayer::objects2d.size();
		if (size > 0)
		{
			Batch* batch = Batch::GetInstance();
			batch->Gindexcount = 0;
			batch->BeginGameObjects();
			/*for (size_t i = 0; i < size; i++) {
				GameObject* go = (GameObject*)&Renderer2DLayer::objects2d[i].get();
				if (go->Enable == true)
				{
					batch->Submit(*go);
				}
			}*/
			for (auto object : Renderer2DLayer::objects2d) {
				GameObject* go = (GameObject*)&object.get();
				if (go->Enable == true && (go->AlwaysDraw || sqrt(pow((go->position.x - Window::GetCamera().GetPosition().x), 2) + pow((go->position.y - Window::GetCamera().GetPosition().y), 2)) < 50 * Window::GetCamera().GetZoomLevel()))
				{
					batch->Submit(*go);
				}
			}
			batch->EndGameObjects();
		}
		
	}
}

//void Doom::Renderer::CalculateMVPforAllObjects()
//{
//	
//	ThreadPool::Instance()->enqueue([] {std::unique_lock<std::mutex> lock(Renderer::mtx);
//	unsigned int size = GetAmountOfObjects() * 0.33333;
//		for (unsigned int i = 0; i < size; i++)
//		{
//			GameObject* go = (GameObject*)(&Renderer2DLayer::objects2d[i].get());
//			float * scaleVal = go->GetScale();
//			go->submitedVectors[0] = (glm::vec2(go->mesh2D[0] * scaleVal[0], go->mesh2D[1] * scaleVal[1]));
//			go->submitedVectors[1] = (glm::vec2(go->mesh2D[4] * scaleVal[0], go->mesh2D[5] * scaleVal[1]));
//			go->submitedVectors[2] = (glm::vec2(go->mesh2D[8] * scaleVal[0], go->mesh2D[9] * scaleVal[1]));
//			go->submitedVectors[3] = (glm::vec2(go->mesh2D[12] * scaleVal[0], go->mesh2D[13] * scaleVal[1]));
//			//go->MVP = go->pos * go->view;
//		}
//		isReadyToRenderFirstThread = true;
//		Renderer::condVar.notify_one();
//	});
//	ThreadPool::Instance()->enqueue([] {std::unique_lock<std::mutex> lock(Renderer::mtx);
//		unsigned int size = GetAmountOfObjects() - GetAmountOfObjects() * 0.33333;
//		unsigned int size1 = GetAmountOfObjects() * 0.33333 ;
//		for (unsigned int i = size1; i < size; i++)
//		{
//			GameObject* go = (GameObject*)(&Renderer2DLayer::objects2d[i].get());
//			float * scaleVal = go->GetScale();
//			//go->MVP = go->pos * go->view;
//			go->submitedVectors[0] = (glm::vec2(go->mesh2D[0] * scaleVal[0], go->mesh2D[1] * scaleVal[1]));
//			go->submitedVectors[1] = (glm::vec2(go->mesh2D[4] * scaleVal[0], go->mesh2D[5] * scaleVal[1]));
//			go->submitedVectors[2] = (glm::vec2(go->mesh2D[8] * scaleVal[0], go->mesh2D[9] * scaleVal[1]));
//			go->submitedVectors[3] = (glm::vec2(go->mesh2D[12] * scaleVal[0], go->mesh2D[13] * scaleVal[1]));
//			
//		}
//		isReadyToRenderSecondThread = true;
//		Renderer::condVar.notify_one();
//	});
//	ThreadPool::Instance()->enqueue([] {std::unique_lock<std::mutex> lock(Renderer::mtx);
//		unsigned int size = GetAmountOfObjects();
//		unsigned int size1 = size - GetAmountOfObjects() * 0.33333;
//		for (unsigned int i = size1; i < size; i++)
//		{
//			GameObject* go = (GameObject*)(&Renderer2DLayer::objects2d[i].get());
//			float * scaleVal = go->GetScale();
//			//go->MVP = go->pos * go->view;
//			go->submitedVectors[0] = (glm::vec2(go->mesh2D[0] * scaleVal[0], go->mesh2D[1] * scaleVal[1]));
//			go->submitedVectors[1] = (glm::vec2(go->mesh2D[4] * scaleVal[0], go->mesh2D[5] * scaleVal[1]));
//			go->submitedVectors[2] = (glm::vec2(go->mesh2D[8] * scaleVal[0], go->mesh2D[9] * scaleVal[1]));
//			go->submitedVectors[3] = (glm::vec2(go->mesh2D[12] * scaleVal[0], go->mesh2D[13] * scaleVal[1]));
//			
//		}
//		Renderer::isReadyToRenderThirdThread = true; 
//		Renderer::condVar.notify_one();
//	});
//}

void Doom::Renderer::RenderText() {
	Batch::GetInstance()->flushText(Batch::GetInstance()->TextShader);
}

void Doom::Renderer::RenderCollision(OrthographicCamera& camera){
	if (Collision::IsVisible == true && Renderer2DLayer::collision2d.size() > 0) {
		Batch::GetInstance()->Gindexcount = 0;
		Batch::GetInstance()->BeginGameObjects();
		for (unsigned int i = 0; i < Renderer2DLayer::collision2d.size(); i++) {
			Renderer2DLayer* col = &Renderer2DLayer::collision2d[i].get();
			if (col != nullptr && col->Enable == true) {
				if (col->IsCollisionEnabled()) {
					Collision* go = static_cast<Collision*>(col);
					Batch::GetInstance()->Submit(*go);
				}
			}
		}
		Batch::GetInstance()->EndGameObjects();
		Batch::GetInstance()->flushCollision(Batch::GetInstance()->CollisionShader);
	}
}