#include "../pch.h"
#include "Gizmos.h"
#include "../Render/Batch.h"

using namespace Doom;

Doom::Gizmos::Gizmos(float x, float y,float z) : GameObject("Gizmos", x, y) {
	m_Enable = false;
	GameObject::m_Enable = false;
	EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, (GameObject*)this);
	World::PopBack();
	World::s_ObjId--;

	for (size_t i = 0; i < 3; i++)
	{
		m_Faces.push_back(new GameObject("Face" + std::to_string(i),x,y,z));
		m_Faces.back()->GetComponentManager()->AddComponent<SpriteRenderer>();
		m_Faces.back()->SetOwner((void*)this);
		AddChild((void*)m_Faces.back());
		m_Axis.push_back(new Line(glm::vec3(0),glm::vec3(0)));
		m_Faces.back()->m_Enable = false;
		World::PopBack();
		World::s_ObjId--;
		Line::s_Lines.pop_back();
	}
	m_Tr = GetComponent<Transform>();
}

void Doom::Gizmos::OnUpdate()
{
	m_IsHovered = false;
	if (m_BlockFrame == true && Input::IsMouseReleased(Keycode::MOUSE_BUTTON_1)){
		m_BlockFrame = false;
		return;
	}
	else if (m_BlockFrame == true && !Input::IsMouseReleased(Keycode::MOUSE_BUTTON_1)) {
		return;
	}

	if (m_Obj == nullptr) {
		m_Enable = false;
		return;
	}
	else
		m_Enable = true;

	m_Tr->Translate(m_Obj->GetPosition().x, m_Obj->GetPosition().y, m_Obj->GetPosition().z);
	glm::vec3 pos = Window::GetCamera().GetPosition();
	glm::vec3 v = (pos - m_Tr->GetPosition());
	float d = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	d *= 0.1;
	m_Tr->Scale(d, d, d);

	m_Axis[0]->SetStartPoint(m_Tr->GetPosition());
	m_Axis[0]->SetEndPoint(m_Tr->GetPosition().x + d * 1.5, m_Tr->GetPosition().y, m_Tr->GetPosition().z);
	m_Axis[0]->m_Color = COLORS::Red;
	m_Axis[1]->SetStartPoint(m_Tr->GetPosition());
	m_Axis[1]->SetEndPoint(m_Tr->GetPosition().x, m_Tr->GetPosition().y + d * 1.5, m_Tr->GetPosition().z);
	m_Axis[1]->m_Color = COLORS::Green;
	m_Axis[2]->SetStartPoint(m_Tr->GetPosition());
	m_Axis[2]->SetEndPoint(m_Tr->GetPosition().x, m_Tr->GetPosition().y, m_Tr->GetPosition().z + d * 1.5);
	m_Axis[2]->m_Color = COLORS::Blue;

	Transform* trO = nullptr;
	m_Faces[0]->GetComponent<SpriteRenderer>()->m_Color = COLORS::Red;
	trO = m_Faces[0]->GetComponent<Transform>();
	trO->Translate(m_Tr->GetPosition().x, m_Tr->GetPosition().y + d, m_Tr->GetPosition().z + d);
	if (pos.x > trO->GetPosition().x)
		trO->RotateOnce(0, -90, 0, false);
	else
		trO->RotateOnce(0, 90, 0, false);

	m_Faces[1]->GetComponent<SpriteRenderer>()->m_Color = COLORS::Green;
	trO = m_Faces[1]->GetComponent<Transform>();
	trO->Translate(m_Tr->GetPosition().x + d, m_Tr->GetPosition().y, m_Tr->GetPosition().z + d);
	if (pos.y < trO->GetPosition().y)
		trO->RotateOnce(-90, 0, 0, false);
	else
		trO->RotateOnce(90, 0, 0, false);

	m_Faces[2]->GetComponent<SpriteRenderer>()->m_Color = COLORS::Blue;
	trO = m_Faces[2]->GetComponent<Transform>();
	trO->Translate(m_Tr->GetPosition().x + d, m_Tr->GetPosition().y + d, m_Tr->GetPosition().z);
	if (pos.z < trO->GetPosition().z)
		trO->RotateOnce(180, 0, 0, false);
	else
		trO->RotateOnce(0, 0, 0, false);

	glm::vec3 dir = Window::GetCamera().GetMouseDirVec();
	if (Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		IntersectPlane(dir);
		if (m_F == nullptr)
			IntersectLine(dir, d * 0.1f);
	}
	else if (!Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		IntersectPlane(dir);
		IntersectLine(dir, d * 0.1f);
		m_L = nullptr;
		m_F = nullptr;
	}
}

bool Doom::Gizmos::Intersect(float& tMinOut,glm::vec3 bMin,glm::vec3 bMax,glm::vec3 start, glm::vec3 dir) {
	
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

		if (txMin > tyMax || tyMin > txMax) return false;
		if (tMin > tzMax || tzMin > tMax) return false;
		if (tzMin > tMin) tMin = tzMin;
		if (tzMax < tMax) tMax = tzMax;
		
		tMinOut = tMin;
		return true;
}

void Doom::Gizmos::IntersectLine(glm::vec3 dir,float scale)
{
	std::map<float, Line*> d;
	glm::vec3 start = Window::GetCamera().GetPosition();
	std::pair<float, Line*> pair;
	float dist = 0;
	if (m_L != nullptr)
		scale *= 100;
	for (size_t i = 0; i < 3; i++)
	{
		Line* line = m_Axis[i];
		if (!line->m_Enable)
			continue;
		float tMin = 1e300;
		glm::vec3 bMin = glm::vec3(line->m_Verteces[0], line->m_Verteces[1], line->m_Verteces[2]);
		glm::vec3 bMax = glm::vec3(line->m_Verteces[3], line->m_Verteces[4], line->m_Verteces[5]);
		if (i == 1) {
			bMin[2] -= scale;
			bMax[2] += scale;
		}
		else {
			bMin[2 - i] -= scale;
			bMax[2 - i] += scale;
		}
		if (Intersect(tMin, bMin, bMax, start, dir) == false)
			continue;
			d.insert(std::make_pair(tMin, line));
		}
	
		float min = 1e300;
		for (auto i = d.begin(); i != d.end(); i++)
		{
			if (i->first < min)
				min = i->first;
		}

	
		auto iter = d.find(min);
		if (iter != d.end()) {
			pair = std::make_pair(iter->first, iter->second);

		}
		else {
			pair = std::make_pair(0.0f, nullptr);
		}
		m_L = pair.second;
		dist = pair.first;

	if (m_L != nullptr && Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		m_L->m_Color = COLORS::Orange;
		m_IsHovered = true;
		glm::vec3 diff = glm::vec3(m_L->m_Verteces[3], m_L->m_Verteces[4], m_L->m_Verteces[5]) - glm::vec3(m_L->m_Verteces[0], m_L->m_Verteces[1], m_L->m_Verteces[2]);
		float transform;
		if (m_L == m_Axis[0]) {
			if (m_RoundTransform)
				transform = glm::round(Window::GetCamera().GetPosition().x + dir.x * dist - diff.x * 0.5f);
			else
				transform = Window::GetCamera().GetPosition().x + dir.x * dist - diff.x * 0.5f;
			m_Obj->GetComponent<Transform>()->Translate(transform, m_Obj->GetPosition().y, m_Obj->GetPosition().z);
			m_Faces[0]->m_Enable = false;
			m_Faces[1]->m_Enable = false;
			m_Faces[2]->m_Enable = false;
			m_Axis[1]->m_Enable = false;
			m_Axis[2]->m_Enable = false;
		}
		else if (m_L == m_Axis[1]) {
			if (m_RoundTransform)
				transform = glm::round(Window::GetCamera().GetPosition().y + dir.y * dist - diff.y * 0.5f);
			else
				transform = Window::GetCamera().GetPosition().y + dir.y * dist - diff.y * 0.5f;
			m_Obj->GetComponent<Transform>()->Translate(m_Obj->GetPosition().x, transform, m_Obj->GetPosition().z);
			m_Faces[0]->m_Enable = false;
			m_Faces[1]->m_Enable = false;
			m_Faces[2]->m_Enable = false;
			m_Axis[0]->m_Enable = false;
			m_Axis[2]->m_Enable = false;
		}
		else if (m_L == m_Axis[2]) {
			if (m_RoundTransform)
				transform = glm::round(Window::GetCamera().GetPosition().z + dir.z * dist - diff.z * 0.5f);
			else
				transform = Window::GetCamera().GetPosition().z + dir.z * dist - diff.z * 0.5f;
			m_Obj->GetComponent<Transform>()->Translate(m_Obj->GetPosition().x, m_Obj->GetPosition().y, transform);
			m_Faces[0]->m_Enable = false;
			m_Faces[1]->m_Enable = false;
			m_Faces[2]->m_Enable = false;
			m_Axis[1]->m_Enable = false;
			m_Axis[0]->m_Enable = false;
		}
	}
	else {
		for (size_t i = 0; i < 3; i++)
		{
			m_Axis[i]->m_Enable = true;
		}
	}
}

void Doom::Gizmos::IntersectPlane(glm::vec3 translation)
{
	std::map<float, GameObject*> d;
	glm::vec3 start = Window::GetCamera().GetPosition();
	glm::vec4 scale = glm::vec4(1);
	if (m_F != nullptr)
		scale = glm::vec4(100);
	for (size_t i = 0; i < 3; i++)
	{
		GameObject* c = m_Faces[i];
		if (!c->m_Enable)
			continue;
		Transform* tr = c->GetComponent<Transform>();
		float tMin = 1e300;
		glm::vec3 pos = c->GetPosition();
		glm::mat4 viewXscale = tr->m_ViewMat4 * tr->m_ScaleMat4;
		glm::vec4 transformedBMin = viewXscale * (glm::vec4(-0.5f, -0.5f, 0, 0) * scale);
		glm::vec4 transformedBMax = viewXscale * (glm::vec4(0.5f, 0.5f, 0, 0)* scale);
		glm::vec3 bMin = pos + (glm::vec3)transformedBMin;
		glm::vec3 bMax = pos + (glm::vec3)transformedBMax;
		if (Intersect(tMin, bMin, bMax, start, translation) == false)
			continue;
		d.insert(std::make_pair(tMin, c));
	}
	float min = 1e300;
	for (auto i = d.begin(); i != d.end(); i++)
	{
		if (i->first < min)
			min = i->first;
	}

	std::pair<float, GameObject*> pair;
	auto iter = d.find(min);
	if (iter != d.end()) {
		pair = std::make_pair(iter->first, iter->second);

	}
	else {
		pair = std::make_pair(0.0f, nullptr);
	}
	GameObject* go = pair.second;
	m_F = go;
	float dist = pair.first;
	if (go != nullptr && Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		go->GetComponent<SpriteRenderer>()->m_Color = COLORS::Orange;
		m_IsHovered = true;
		glm::vec3 diff = go->GetPosition() - m_Obj->GetPosition();
		for (size_t i = 0; i < 3; i++)
		{
			m_Axis[i]->m_Enable = false;
		}
		glm::vec3 transform = glm::vec3(Window::GetCamera().GetPosition().x + translation.x * dist
		, Window::GetCamera().GetPosition().y + translation.y * dist
		, Window::GetCamera().GetPosition().z + translation.z * dist);
		if (m_RoundTransform) {
			transform.x = glm::round(transform.x);
			transform.y = glm::round(transform.y);
			transform.z = glm::round(transform.z);
			diff.x = glm::round(diff.x);
			diff.y = glm::round(diff.y);
			diff.z = glm::round(diff.z);
		}
		if (go->m_Name == "Face0") {
			go->GetComponent<Transform>()->Translate(go->GetPosition().x, transform.y, transform.z);
			m_Obj->GetComponent<Transform>()->Translate(m_Obj->GetPosition().x, transform.y - diff.y, transform.z - diff.z);
			m_Faces[1]->m_Enable = false;
			m_Faces[2]->m_Enable = false;
		}
		else if (go->m_Name == "Face1") {
			go->GetComponent<Transform>()->Translate(transform.x, go->GetPosition().y, transform.z);
			m_Obj->GetComponent<Transform>()->Translate(transform.x - diff.x, m_Obj->GetPosition().y, transform.z - diff.z);
			m_Faces[0]->m_Enable = false;
			m_Faces[2]->m_Enable = false;
		}
		else if (go->m_Name == "Face2") {
			go->GetComponent<Transform>()->Translate(transform.x, transform.y, go->GetPosition().z);
			m_Obj->GetComponent<Transform>()->Translate(transform.x - diff.x, transform.y - diff.y, m_Obj->GetPosition().z);
			m_Faces[0]->m_Enable = false;
			m_Faces[1]->m_Enable = false;
		}
	}
	else {
		for (size_t i = 0; i < 3; i++)
		{
			m_Faces[i]->m_Enable = true;
		}
	}
}

void Doom::Gizmos::Render()
{
	if (m_Obj == nullptr) {
		m_Faces[0]->m_Enable = false;
		m_Faces[1]->m_Enable = false;
		m_Faces[2]->m_Enable = false;
		return;
	}

	glm::vec3 pos = Window::GetCamera().GetPosition();
	std::map<float,GameObject*> d;
	for (size_t i = 0; i < 3; i++)
	{
		d.insert(std::make_pair(glm::distance(pos, m_Faces[i]->GetPosition()),m_Faces[i]));
	}
	
	Batch::GetInstance()->m_GIndexCount = 0;
	{
		Batch::GetInstance()->BeginGameObjects();
	}
	for (auto i = d.rbegin(); i != d.rend(); i++)
	{
		if(i->second->m_Enable)
			Batch::GetInstance()->Submit(*i->second->GetComponent<SpriteRenderer>());
	}
	Batch::GetInstance()->EndGameObjects();

	Batch::GetInstance()->m_LIndexCount = 0;
	Batch::GetInstance()->BeginLines();
	for (size_t i = 0; i < 3; i++)
	{
		if (m_Axis[i]->m_Enable)
			Batch::GetInstance()->Submit(*m_Axis[i]);
	}
	Batch::GetInstance()->EndLines();

	Texture::s_WhiteTexture->Bind();
	glDisable(GL_DEPTH_TEST);
	Batch::GetInstance()->flushLines(Batch::GetInstance()->m_LineShader);
	Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->m_BasicShader);
	glEnable(GL_DEPTH_TEST);
}
