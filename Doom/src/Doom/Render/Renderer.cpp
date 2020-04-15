#include "../pch.h"
#include "Renderer.h"
#include <iostream>
#include "Batch.h"
#include "../Core/Timer.h"
#include "Line.h"

using namespace Doom;

void Doom::Renderer::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	delete go;
}

void Doom::Renderer::Save(const std::string filename) {
	std::ofstream out_file;
	out_file.open(filename, std::ofstream::trunc);
	if (out_file.is_open()) {
		for (unsigned int i = 0; i < Renderer2DLayer::objects2d.size(); i++)
		{
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
			if (Renderer2DLayer::objects2d[i].get().GetShaderType() == 0){
				out_file << *Renderer2DLayer::objects2d[i].get().GetPathToTexture() << "\n";
				out_file << color[0] << " " << color[1] << " " << color[2] << " " << 255 << "\n";
			}
			else if (Renderer2DLayer::objects2d[i].get().GetShaderType() == 1) {
				out_file << "None" << "\n";
				out_file << color[0] << " " << color[1] << " " << color[2] << " " << color[3] << "\n";
			}
			out_file << scale[0] << " " << scale[1] << " " << scale[2] << "\n";
			out_file << Renderer2DLayer::objects2d[i].get().GetRenderType();
			if (i + 1 != Renderer2DLayer::objects2d.size())
				out_file << "\n";
			delete color;
		}
	}
	else {
		std::cout << "Error: filename doesn't exist";
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
	int rendertype = 0;
	int shadertype = 1;
	int axes[3] = { 0,0,1 };
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
				if (type == "GameObject") {		
					LoadObj<GameObject>(name, pathtotext, angle, color, scale, pos, shadertype,hascollision,offset,rendertype,axes,istrigger,tag);
				}
		}
	}
	in_file.close();
	std::cout << "Save has been loaded" << std::endl;
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

void Renderer::Render() {
	OrthographicCamera* camera = &Window::GetCamera();
	SubmitGameObjects(*camera);
	Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->BasicShader);
	
	//Simple render. one line in one draw call.
	/*for (unsigned int i = 0; i < Line::lines.size(); i++)
	{
		if (Line::lines[i]->Enable) {
			glLineWidth(Line::lines[i]->width);
			Line::lines[i]->OnRunning();
		}
	}
	glLineWidth(1.0f);*/

	Batch::GetInstance()->Lindexcount = 0;
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
	RenderCollision(*camera);
	RenderText();
}

void Doom::Renderer::SubmitGameObjects(OrthographicCamera& camera)
{
	std::unique_lock<std::mutex> lock(Renderer::mtx);
	{
		condVar.wait(lock, [] { return(isReadyToRenderFirstThread && isReadyToRenderSecondThread && isReadyToRenderThirdThread); });
		isReadyToRenderFirstThread = false;
		isReadyToRenderSecondThread = false;
		isReadyToRenderThirdThread = false;
		unsigned int size = Renderer2DLayer::objects2d.size();
		if (size > 0)
		{
			Batch* batch = Batch::GetInstance();
			batch->Gindexcount = 0;
			batch->BeginGameObjects();
			isReadyToRenderFirstThread = false;
			for (unsigned int i = 0; i < size; ++i) {
				GameObject* go = (GameObject*)&Renderer2DLayer::objects2d[i].get();
				if (go->Enable == true)
				{
					if (batch->Gindexcount >= RENDERER_INDICES_SIZE) {
						Batch::GetInstance()->EndGameObjects();
						Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->BasicShader);
						Batch::GetInstance()->BeginGameObjects();
					}
					//auto f = std::bind(&Batch::SubmitG,batch,*go);
					batch->SubmitG(*go);
					//ThreadPool::Instance()->enqueue(f);
				}

			}
			batch->EndGameObjects();
		}
		
	}
}

void Doom::Renderer::CalculateMVPforAllObjects()
{
	
	ThreadPool::Instance()->enqueue([] {std::unique_lock<std::mutex> lock(Renderer::mtx);
	unsigned int size = GetAmountOfObjects() * 0.33333;
		for (unsigned int i = 0; i < size; i++)
		{
			GameObject* go = (GameObject*)(&Renderer2DLayer::objects2d[i].get());
			float * scaleVal = go->GetScale();
			go->submitedVectors[0] = (glm::vec2(go->mesh2D[0] * scaleVal[0], go->mesh2D[1] * scaleVal[1]));
			go->submitedVectors[1] = (glm::vec2(go->mesh2D[4] * scaleVal[0], go->mesh2D[5] * scaleVal[1]));
			go->submitedVectors[2] = (glm::vec2(go->mesh2D[8] * scaleVal[0], go->mesh2D[9] * scaleVal[1]));
			go->submitedVectors[3] = (glm::vec2(go->mesh2D[12] * scaleVal[0], go->mesh2D[13] * scaleVal[1]));
			go->MVP = go->pos * go->view;
		}
		isReadyToRenderFirstThread = true;
		Renderer::condVar.notify_one();
	});
	ThreadPool::Instance()->enqueue([] {std::unique_lock<std::mutex> lock(Renderer::mtx);
		unsigned int size = GetAmountOfObjects() - GetAmountOfObjects() * 0.33333;
		unsigned int size1 = GetAmountOfObjects() * 0.33333 ;
		for (unsigned int i = size1; i < size; i++)
		{
			GameObject* go = (GameObject*)(&Renderer2DLayer::objects2d[i].get());
			float * scaleVal = go->GetScale();
			go->MVP = go->pos * go->view;
			go->submitedVectors[0] = (glm::vec2(go->mesh2D[0] * scaleVal[0], go->mesh2D[1] * scaleVal[1]));
			go->submitedVectors[1] = (glm::vec2(go->mesh2D[4] * scaleVal[0], go->mesh2D[5] * scaleVal[1]));
			go->submitedVectors[2] = (glm::vec2(go->mesh2D[8] * scaleVal[0], go->mesh2D[9] * scaleVal[1]));
			go->submitedVectors[3] = (glm::vec2(go->mesh2D[12] * scaleVal[0], go->mesh2D[13] * scaleVal[1]));
			
		}
		isReadyToRenderSecondThread = true;
		Renderer::condVar.notify_one();
	});
	ThreadPool::Instance()->enqueue([] {std::unique_lock<std::mutex> lock(Renderer::mtx);
		unsigned int size = GetAmountOfObjects();
		unsigned int size1 = size - GetAmountOfObjects() * 0.33333;
		for (unsigned int i = size1; i < size; i++)
		{
			GameObject* go = (GameObject*)(&Renderer2DLayer::objects2d[i].get());
			float * scaleVal = go->GetScale();
			go->MVP = go->pos * go->view;
			go->submitedVectors[0] = (glm::vec2(go->mesh2D[0] * scaleVal[0], go->mesh2D[1] * scaleVal[1]));
			go->submitedVectors[1] = (glm::vec2(go->mesh2D[4] * scaleVal[0], go->mesh2D[5] * scaleVal[1]));
			go->submitedVectors[2] = (glm::vec2(go->mesh2D[8] * scaleVal[0], go->mesh2D[9] * scaleVal[1]));
			go->submitedVectors[3] = (glm::vec2(go->mesh2D[12] * scaleVal[0], go->mesh2D[13] * scaleVal[1]));
			
		}
		Renderer::isReadyToRenderThirdThread = true; 
		Renderer::condVar.notify_one();
	});
}

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
					//col->OnRunning(camera);
					//DrawCalls++;
					Collision* go = static_cast<Collision*>(col);
					Batch::GetInstance()->Submit(*go);
				}
			}
		}
		Batch::GetInstance()->EndGameObjects();
		Batch::GetInstance()->flushCollision(Batch::GetInstance()->CollisionShader);
	}
}