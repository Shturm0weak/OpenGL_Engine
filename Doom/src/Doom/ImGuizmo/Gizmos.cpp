#include "../pch.h"
#include "Gizmos.h"
#include "../Render/Batch.h"

using namespace Doom;

Doom::Gizmos::Gizmos(float x, float y,float z) : GameObject("Gizmos", x, y) {
	Enable = false;
	GameObject::Enable = false;
	EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, (GameObject*)this);
	Renderer::PopBack();
	Renderer::obj_id--;

	for (size_t i = 0; i < 3; i++)
	{
		faces.push_back(new GameObject("Face" + std::to_string(i),x,y,z));
		faces.back()->GetComponentManager()->AddComponent<SpriteRenderer>();
		faces.back()->SetOwner((void*)this);
		AddChild((void*)faces.back());
		axis.push_back(new Line(glm::vec3(0),glm::vec3(0)));
		faces.back()->Enable = false;
		Renderer::PopBack();
		Renderer::obj_id--;
		Line::lines.pop_back();
	}
	tr = GetComponent<Transform>();
}

void Doom::Gizmos::OnUpdate()
{
	isHovered = false;
	if (blockFrame == true && Input::IsMouseReleased(Keycode::MOUSE_BUTTON_1)){
		blockFrame = false;
		return;
	}
	else if (blockFrame == true && !Input::IsMouseReleased(Keycode::MOUSE_BUTTON_1)) {
		return;
	}

	if (obj == nullptr) {
		Enable = false;
		return;
	}
	else
		Enable = true;

	tr->Translate(obj->GetPosition().x, obj->GetPosition().y, obj->GetPosition().z);
	glm::vec3 pos = Window::GetCamera().GetPosition();
	glm::vec3 v = (pos - tr->GetPosition());
	float d = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	d *= 0.1;
	tr->Scale(d, d, d);

	axis[0]->SetStartPoint(tr->GetPosition());
	axis[0]->SetEndPoint(tr->GetPosition().x + d * 1.5, tr->GetPosition().y, tr->GetPosition().z);
	axis[0]->color = COLORS::Red;
	axis[1]->SetStartPoint(tr->GetPosition());
	axis[1]->SetEndPoint(tr->GetPosition().x, tr->GetPosition().y + d * 1.5, tr->GetPosition().z);
	axis[1]->color = COLORS::Green;
	axis[2]->SetStartPoint(tr->GetPosition());
	axis[2]->SetEndPoint(tr->GetPosition().x, tr->GetPosition().y, tr->GetPosition().z + d * 1.5);
	axis[2]->color = COLORS::Blue;

	Transform* trO = nullptr;
	faces[0]->GetComponent<SpriteRenderer>()->color = COLORS::Red;
	trO = faces[0]->GetComponent<Transform>();
	trO->Translate(tr->GetPosition().x, tr->GetPosition().y + d, tr->GetPosition().z + d);
	if (pos.x > trO->GetPosition().x)
		trO->RotateOnce(0, -90, 0, false);
	else
		trO->RotateOnce(0, 90, 0, false);

	faces[1]->GetComponent<SpriteRenderer>()->color = COLORS::Green;
	trO = faces[1]->GetComponent<Transform>();
	trO->Translate(tr->GetPosition().x + d, tr->GetPosition().y, tr->GetPosition().z + d);
	if (pos.y < trO->GetPosition().y)
		trO->RotateOnce(-90, 0, 0, false);
	else
		trO->RotateOnce(90, 0, 0, false);

	faces[2]->GetComponent<SpriteRenderer>()->color = COLORS::Blue;
	trO = faces[2]->GetComponent<Transform>();
	trO->Translate(tr->GetPosition().x + d, tr->GetPosition().y + d, tr->GetPosition().z);
	if (pos.z < trO->GetPosition().z)
		trO->RotateOnce(180, 0, 0, false);
	else
		trO->RotateOnce(0, 0, 0, false);

	glm::vec3 dir = Window::GetCamera().GetMouseDirVec();
	if (Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		IntersectPlane(dir);
		if (f == nullptr)
			IntersectLine(dir, d * 0.1f);
	}
	else if (!Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		IntersectPlane(dir);
		IntersectLine(dir, d * 0.1f);
		l = nullptr;
		f = nullptr;
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
	if (l != nullptr)
		scale *= 100;
	for (size_t i = 0; i < 3; i++)
	{
		Line* line = axis[i];
		if (!line->Enable)
			continue;
		float tMin = 1e300;
		glm::vec3 bMin = glm::vec3(line->mesh2D[0], line->mesh2D[1], line->mesh2D[2]);
		glm::vec3 bMax = glm::vec3(line->mesh2D[3], line->mesh2D[4], line->mesh2D[5]);
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
		l = pair.second;
		dist = pair.first;

	if (l != nullptr && Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		l->color = COLORS::Orange;
		isHovered = true;
		glm::vec3 diff = glm::vec3(l->mesh2D[3], l->mesh2D[4], l->mesh2D[5]) - glm::vec3(l->mesh2D[0], l->mesh2D[1], l->mesh2D[2]);
		float transform;
		if (l == axis[0]) {
			if (roundTransform)
				transform = glm::round(Window::GetCamera().GetPosition().x + dir.x * dist - diff.x * 0.5f);
			else
				transform = Window::GetCamera().GetPosition().x + dir.x * dist - diff.x * 0.5f;
			obj->GetComponent<Transform>()->Translate(transform, obj->GetPosition().y, obj->GetPosition().z);
			faces[0]->Enable = false;
			faces[1]->Enable = false;
			faces[2]->Enable = false;
			axis[1]->Enable = false;
			axis[2]->Enable = false;
		}
		else if (l == axis[1]) {
			if (roundTransform)
				transform = glm::round(Window::GetCamera().GetPosition().y + dir.y * dist - diff.y * 0.5f);
			else
				transform = Window::GetCamera().GetPosition().y + dir.y * dist - diff.y * 0.5f;
			obj->GetComponent<Transform>()->Translate(obj->GetPosition().x, transform, obj->GetPosition().z);
			faces[0]->Enable = false;
			faces[1]->Enable = false;
			faces[2]->Enable = false;
			axis[0]->Enable = false;
			axis[2]->Enable = false;
		}
		else if (l == axis[2]) {
			if (roundTransform)
				transform = glm::round(Window::GetCamera().GetPosition().z + dir.z * dist - diff.z * 0.5f);
			else
				transform = Window::GetCamera().GetPosition().z + dir.z * dist - diff.z * 0.5f;
			obj->GetComponent<Transform>()->Translate(obj->GetPosition().x, obj->GetPosition().y, transform);
			faces[0]->Enable = false;
			faces[1]->Enable = false;
			faces[2]->Enable = false;
			axis[1]->Enable = false;
			axis[0]->Enable = false;
		}
	}
	else {
		for (size_t i = 0; i < 3; i++)
		{
			axis[i]->Enable = true;
		}
	}
}

void Doom::Gizmos::IntersectPlane(glm::vec3 translation)
{
	std::map<float, GameObject*> d;
	glm::vec3 start = Window::GetCamera().GetPosition();
	glm::vec4 scale = glm::vec4(1);
	if (f != nullptr)
		scale = glm::vec4(100);
	for (size_t i = 0; i < 3; i++)
	{
		GameObject* c = faces[i];
		if (!c->Enable)
			continue;
		Transform* tr = c->GetComponent<Transform>();
		float tMin = 1e300;
		glm::vec3 pos = c->GetPosition();
		glm::mat4 viewXscale = tr->view * tr->scale;
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
	f = go;
	float dist = pair.first;
	if (go != nullptr && Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		go->GetComponent<SpriteRenderer>()->color = COLORS::Orange;
		isHovered = true;
		glm::vec3 diff = go->GetPosition() - obj->GetPosition();
		for (size_t i = 0; i < 3; i++)
		{
			axis[i]->Enable = false;
		}
		glm::vec3 transform = glm::vec3(Window::GetCamera().GetPosition().x + translation.x * dist
		, Window::GetCamera().GetPosition().y + translation.y * dist
		, Window::GetCamera().GetPosition().z + translation.z * dist);
		if (roundTransform) {
			transform.x = glm::round(transform.x);
			transform.y = glm::round(transform.y);
			transform.z = glm::round(transform.z);
			diff.x = glm::round(diff.x);
			diff.y = glm::round(diff.y);
			diff.z = glm::round(diff.z);
		}
		if (go->name == "Face0") {
			go->GetComponent<Transform>()->Translate(go->GetPosition().x, transform.y, transform.z);
			obj->GetComponent<Transform>()->Translate(obj->GetPosition().x, transform.y - diff.y, transform.z - diff.z);
			faces[1]->Enable = false;
			faces[2]->Enable = false;
		}
		else if (go->name == "Face1") {
			go->GetComponent<Transform>()->Translate(transform.x, go->GetPosition().y, transform.z);
			obj->GetComponent<Transform>()->Translate(transform.x - diff.x, obj->GetPosition().y, transform.z - diff.z);
			faces[0]->Enable = false;
			faces[2]->Enable = false;
		}
		else if (go->name == "Face2") {
			go->GetComponent<Transform>()->Translate(transform.x, transform.y, go->GetPosition().z);
			obj->GetComponent<Transform>()->Translate(transform.x - diff.x, transform.y - diff.y, obj->GetPosition().z);
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

void Doom::Gizmos::Render()
{
	if (obj == nullptr) {
		faces[0]->Enable = false;
		faces[1]->Enable = false;
		faces[2]->Enable = false;
		return;
	}

	glm::vec3 pos = Window::GetCamera().GetPosition();
	std::map<float,GameObject*> d;
	for (size_t i = 0; i < 3; i++)
	{
		d.insert(std::make_pair(glm::distance(pos, faces[i]->GetPosition()),faces[i]));
	}
	
	Batch::GetInstance()->Gindexcount = 0;
	{
		Batch::GetInstance()->BeginGameObjects();
	}
	for (auto i = d.rbegin(); i != d.rend(); i++)
	{
		if(i->second->Enable)
			Batch::GetInstance()->Submit(*i->second->GetComponent<SpriteRenderer>());
	}
	Batch::GetInstance()->EndGameObjects();

	Batch::GetInstance()->Lindexcount = 0;
	Batch::GetInstance()->BeginLines();
	for (size_t i = 0; i < 3; i++)
	{
		if (axis[i]->Enable)
			Batch::GetInstance()->Submit(*axis[i]);
	}
	Batch::GetInstance()->EndLines();

	Texture::WhiteTexture->Bind();
	glDisable(GL_DEPTH_TEST);
	Batch::GetInstance()->flushLines(Batch::GetInstance()->LineShader);
	Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->BasicShader);
	glEnable(GL_DEPTH_TEST);
}
