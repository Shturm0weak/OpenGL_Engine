#include "../pch.h"
#include "Gizmos.h"
#include "../Render/Batch.h"

using namespace Doom;

Doom::Gizmos::Gizmos(float x, float y,float z) : GameObject("Gizmos", x, y) {
	Enable = false;
	GameObject::Enable = false;
	EventSystem::GetInstance()->RegisterClient("OnUpdate", (GameObject*)this);

	Renderer::PopBack();
	Renderer::obj_id--;

	for (size_t i = 0; i < 3; i++)
	{
		faces.push_back(new GameObject("Face" + std::to_string(i),x,y,z));
		faces.back()->SetOwner((void*)this);
		AddChild((void*)faces.back());
		axis.push_back(new Line(glm::vec3(0),glm::vec3(0)));
		Renderer::PopBack();
		Renderer::obj_id--;
		Line::lines.pop_back();
	}
	tr = GetComponent<Transform>();
}

void Doom::Gizmos::OnUpdate()
{
	if (obj == nullptr || obj->Enable == false) {
		Enable = false;
		return;
	}
	else
		Enable = true;

	tr->Translate(obj->position.x, obj->position.y, obj->position.z);
	glm::vec3 pos = Window::GetCamera().GetPosition();
	glm::vec3 v = (pos - tr->position);
	float d = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	d *= 0.1;
	tr->Scale(d, d, d);

	axis[0]->SetStartPoint(tr->position);
	axis[0]->SetEndPoint(tr->position.x + d * 1.5, tr->position.y, tr->position.z);
	axis[0]->color = COLORS::Red;
	axis[1]->SetStartPoint(tr->position);
	axis[1]->SetEndPoint(tr->position.x, tr->position.y + d * 1.5, tr->position.z);
	axis[1]->color = COLORS::Green;
	axis[2]->SetStartPoint(tr->position);
	axis[2]->SetEndPoint(tr->position.x, tr->position.y, tr->position.z + d * 1.5);
	axis[2]->color = COLORS::Blue;

	Transform* trO = nullptr;
	faces[0]->GetComponent<SpriteRenderer>()->color = COLORS::Red;
	trO = faces[0]->GetComponent<Transform>();
	trO->Translate(tr->position.x,tr->position.y + d,tr->position.z + d);
	if (pos.x > trO->position.x)
		trO->RotateOnce(0, -90, 0, false);
	else
		trO->RotateOnce(0,90,0,false);

	faces[1]->GetComponent<SpriteRenderer>()->color = COLORS::Green;
	trO = faces[1]->GetComponent<Transform>();
	trO->Translate(tr->position.x + d, tr->position.y, tr->position.z + d);
	if (pos.y < trO->position.y)
		trO->RotateOnce(-90, 0, 0, false);
	else
		trO->RotateOnce(90, 0, 0, false);

	faces[2]->GetComponent<SpriteRenderer>()->color = COLORS::Blue;
	trO = faces[2]->GetComponent<Transform>();
	trO->Translate(tr->position.x + d, tr->position.y + d, tr->position.z);
	if (pos.z < trO->position.z)
		trO->RotateOnce(180, 0, 0, false);
	else
		trO->RotateOnce(0, 0, 0, false);

	glm::vec3 translation = Window::GetCamera().GetMouseDirVec();
	std::pair<float, GameObject*> pair = Intersect(translation);
	GameObject* go = pair.second;
	float dist = pair.first;
	if (go != nullptr && Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		go->GetComponent<SpriteRenderer>()->color = COLORS::Orange;
		glm::vec3 diff = go->GetPositions() - obj->GetPositions();
		if (go->name == "Face0") {
			go->GetComponent<Transform>()->Translate(go->GetPositions().x, Window::GetCamera().GetPosition().y +  translation.y * dist, Window::GetCamera().GetPosition().z + translation.z * dist);
			obj->GetComponent<Transform>()->Translate(obj->GetPositions().x, Window::GetCamera().GetPosition().y + translation.y * dist - diff.y, Window::GetCamera().GetPosition().z + translation.z * dist - diff.z);
			faces[1]->Enable = false;
			faces[2]->Enable = false;
		}
		else if (go->name == "Face1") {
			go->GetComponent<Transform>()->Translate(Window::GetCamera().GetPosition().x + translation.x * dist,go->GetPositions().y, Window::GetCamera().GetPosition().z + translation.z * dist);
			obj->GetComponent<Transform>()->Translate(Window::GetCamera().GetPosition().x + translation.x * dist - diff.x, obj->GetPositions().y, Window::GetCamera().GetPosition().z + translation.z * dist - diff.z);
			faces[0]->Enable = false;
			faces[2]->Enable = false;
		}
		else if (go->name == "Face2") {
			go->GetComponent<Transform>()->Translate(Window::GetCamera().GetPosition().x + translation.x * dist, Window::GetCamera().GetPosition().y + translation.y * dist,go->GetPositions().z);
			obj->GetComponent<Transform>()->Translate(Window::GetCamera().GetPosition().x + translation.x * dist - diff.x, Window::GetCamera().GetPosition().y + translation.y * dist - diff.y, obj->GetPositions().z);
			faces[0]->Enable = false;
			faces[1]->Enable = false;
		}
	}
	else {
		for (size_t i = 0; i < 3; i++)
		{
			faces[i]->Enable = true;
		}
	}
}

std::pair<float,GameObject*> Doom::Gizmos::Intersect(glm::vec3 dir) {
	std::map<float, GameObject*> d;
	glm::vec3 start = Window::GetCamera().GetPosition();
	for (size_t i = 0; i < 3; i++)
	{
		GameObject* c = faces[i];
		if (!c->Enable)
			continue;
		Irenderer* r = c->GetComponent<Irenderer>();
		glm::vec3 pos = c->GetPositions();
		glm::mat4 viewXscale = r->view * r->scale;
		glm::vec4 transformedBMin = viewXscale * glm::vec4(-0.5f, -0.5f, 0, 0);
		glm::vec4 transformedBMax = viewXscale * glm::vec4(0.5f, 0.5f, 0, 0);
		glm::vec3 bMin = pos + (glm::vec3)transformedBMin;
		glm::vec3 bMax = pos + (glm::vec3)transformedBMax;

		float txMin = (bMin.x - start.x) / dir.x;
		float txMax = (bMax.x - start.x) / dir.x;
		if (txMax < txMin) {
			float temp = txMax;
			txMax = txMin;
			txMin = temp;
		}

		float tyMin = (bMin.y - start.y) / dir.y;
		float tyMax = (bMax.y - start.y) / dir.y;
		if (tyMax < tyMin) {
			float temp = tyMax;
			tyMax = tyMin;
			tyMin = temp;
		}

		float tzMin = (bMin.z - start.z) / dir.z;
		float tzMax = (bMax.z - start.z) / dir.z;
		if (tzMax < tzMin) {
			float temp = tzMax;
			tzMax = tzMin;
			tzMin = temp;
		}

		float tMin = (txMin > tyMin) ? txMin : tyMin;
		float tMax = (txMax < tyMax) ? txMax : tyMax;

		if (txMin > tyMax || tyMin > txMax) continue;
		if (tMin > tzMax || tzMin > tMax) continue;
		if (tzMin > tMin) tMin = tzMin;
		if (tzMax < tMax) tMax = tzMax;
		d.insert(std::make_pair(tMin, c));
	}

	float min = 1e300;
	for (auto i = d.begin(); i != d.end(); i++)
	{
		if (i->first < min)
			min = i->first;
	}

	auto iter = d.find(min);
	if (iter != d.end()) {
		return std::make_pair(iter->first,iter->second);
		
	}
	else {
		return std::make_pair(0.0f,nullptr);
	}
}

void Doom::Gizmos::Render()
{
	glm::vec3 pos = Window::GetCamera().GetPosition();
	std::map<float,GameObject*> d;
	for (size_t i = 0; i < 3; i++)
	{
		d.insert(std::make_pair(glm::distance(pos, faces[i]->GetPositions()),faces[i]));
	}

	Batch::GetInstance()->Gindexcount = 0;
	Batch::GetInstance()->BeginGameObjects();
	for (auto i = d.rbegin(); i != d.rend(); i++)
	{
		if(i->second->Enable)
			Batch::GetInstance()->Submit(*i->second->GetComponent<SpriteRenderer>());
	}
	Batch::GetInstance()->EndGameObjects();

	Batch::GetInstance()->BeginLines();
	for (size_t i = 0; i < 3; i++)
	{
		Batch::GetInstance()->Submit(*axis[i]);
	}
	Batch::GetInstance()->EndLines();
	Batch::GetInstance()->Lindexcount = 6;

	Texture::WhiteTexture->Bind();
	glDisable(GL_DEPTH_TEST);
	Batch::GetInstance()->flushLines(Batch::GetInstance()->LineShader);
	Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->BasicShader);
	glEnable(GL_DEPTH_TEST);
}
