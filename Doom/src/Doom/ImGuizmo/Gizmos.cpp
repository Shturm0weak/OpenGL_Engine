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
		faces.push_back(new GameObject("Face",x,y,z));
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
