#pragma once
#include <unordered_map>
#define E 0.01

void PerlinNoise2D(int nWidth, int nHeight, float *fSeed, int nOctaves, float fBias, float *fOutput)
{
	// Used 1D Perlin Noise
	for (int x = 0; x < nWidth; x++)
		for (int y = 0; y < nHeight; y++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nWidth >> o;
				int nSampleX1 = (x / nPitch) * nPitch;
				int nSampleY1 = (y / nPitch) * nPitch;

				int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
				int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

				float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
				float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

				float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
				float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

				fScaleAcc += fScale;
				fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			fOutput[y * nWidth + x] = fNoise / fScaleAcc;
		}

}

#include <fstream>

class Minecraft : public Application {
public:
	Minecraft(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}
	std::vector<GameObject*> cube;
	std::vector<GameObject*> tree;
	GameObject* terrain = nullptr;
	GameObject* gladiator = nullptr;
	std::vector<GameObject*> lights;
	int width = 16;
	int height = 16;
	int dirtCount = 0;
	double time = 0.0;
	Ray3D::Hit hit;
	Texture* dirtImg = nullptr;
	std::vector <glm::vec3> dots1;
	std::vector <glm::vec3> dots2;
	std::unordered_multimap<uint32_t,uint32_t> lines;
	std::map<uint32_t, glm::vec3> lD;
	virtual void OnStart() override {

		MeshManager::LoadMesh("kompas", "src/Mesh/model.stl");

		ImGui::SetCurrentContext(Window::imGuiContext);
		dirtImg = Texture::Create("src/Minecraft/Textures/dirtImg.png");
		//float* seed = new float[width * height];
		//float* noise = new float[width * height];
		terrain = new GameObject("terrain");
		
		//for (size_t i = 0; i < width * height; i++)
		//{
		//	seed[i] = (float)rand() / (float)RAND_MAX;
		//}
		//PerlinNoise2D(width, height,seed,5,0.7,noise);
		//delete[] seed;
		std::vector<std::string> faces = {
				"src/SkyBox/back.png",
				"src/SkyBox/front.png",
				"src/SkyBox/top.png",
				"src/SkyBox/bottom.png",
				"src/SkyBox/left.png",
				"src/SkyBox/right.png",
		};
		MeshManager::LoadMesh("skyboxCube", "src/Mesh/cube.fbx");
		MeshManager::AsyncLoadMesh("cube", "src/Minecraft/Models/cube.fbx");
		MeshManager::LoadMesh("gladiator", "src/Mesh/Try.stl");
		SkyBox* skybox = new SkyBox(faces, MeshManager::GetMesh("skyboxCube"));
		/*for (size_t i = 0; i < width; i++)
		{
			for (size_t j = 0; j < height; j++)
			{
				CreateCube(i, j);
				cube.back()->GetComponent<Transform>()->Translate(i,0,j);
			}
		}*/
		for (uint32_t i = 0; i < 2; i++)
		{
			lights.push_back(new GameObject(std::string("light " + std::to_string(i)), i * 10, 0, i * 10));
			lights.back()->GetComponentManager()->AddComponent<PointLight>();
			lights.back()->GetComponentManager()->GetComponent<PointLight>()->color = glm::vec3(i * 0.1, 0, i * 0.1);
		}
		lights.back()->GetComponentManager()->RemoveComponent<PointLight>();
		lights.back()->GetComponentManager()->AddComponent<DirectionalLight>();
		lights.back()->GetComponent<Transform>()->RotateOnce(-110,0,0);
		gladiator = new GameObject("gladiator", 0, 5, 0);
		gladiator->GetComponentManager()->RemoveComponent<Irenderer>();
		gladiator->GetComponentManager()->AddComponent<Renderer3D>()->LoadMesh(MeshManager::GetMesh("cube"));
		//gladiator->GetComponent<Transform>()->Scale(0.1, 0.1, 0.1);
		gladiator->GetComponent<Transform>()->Translate(0, 10, 0);
		glm::vec2 a(0, 0);
		glm::vec2 b(0, 100);
		CrossSection(dots1,a,b);
		glm::vec2 c(0, 0);
		glm::vec2 d(0.1, 0);
		CrossSection(dots2,c, d);

		uint32_t index = 0;
		uint32_t counter = 0;

		for (uint32_t i = 0; i < dots1.size(); i++)
		{
			//std::cout << dots1[i].x << "	" << dots1[i].y << "	" << dots1[i].z << std::endl;
			for (auto iter = lD.begin(); iter != lD.end(); iter++)
			{
				if (IsVec3Equal(dots1[i], iter->second)) {
					counter++;
				}
				else {
					//std::cout << dots1[i].x << "	" << dots1[i].y << "	" << dots1[i].z << " / " << iter->second.x << "	" << iter->second.y << "	" << iter->second.z << std::endl;
				}
			}
			if (counter == 0) {
				//std::cout << "insert " << dots1[i].x << "	" << dots1[i].y << "	" << dots1[i].z << std::endl;
				lD.insert(std::make_pair(index, dots1[i]));
				index++;
			}
			counter = 0;
		}
		int size = lD.size();

		for (uint32_t i = 0; i < dots1.size(); i+=2)
		{
			int32_t a1 = -1;
			int32_t b1 = -1;
			for (auto iter = lD.begin(); iter != lD.end(); iter++)
			{
				if (IsVec3Equal(dots1[i], iter->second)) {
					a1 = iter->first;
				}
			}
			for (auto iter = lD.begin(); iter != lD.end(); iter++)
			{
				if (IsVec3Equal(dots1[i + 1], iter->second)) {
					b1 = iter->first;
				}
			}
			if (a1 < 0 || b1 < 0)
				std::cout << "No such pair of verteces\n";
			else {
				lines.insert(std::make_pair(a1, b1));
				
			}
		}

		//!!!!!
		// Loop is closed if each edge has in and out it looks like this
		// ([x,..] and [...,x]) or ([..,x] and [...,x]) and is not if [x,x] or ([x,..] and [..,x])
		// TO DO!!!!!
		//!!!!!

		for (auto i = lines.begin(); i != lines.end();)
		{
			if (i->first == i->second) {
				lines.erase(i++);
			}
			else {
				++i;
			}
		}

		int con = 0;
		for (auto iter = lines.begin(); iter != lines.end(); iter++)
		{
			Line* l = new Line(lD.find(iter->first)->second, lD.find(iter->second)->second);
			if(con % 2 == 0)
				l->color = COLORS::Green;
			else
				l->color = COLORS::Yellow;
			con++;
		}

	}

	bool IsVec3Equal(glm::vec3 a, glm::vec3 b) {
		return (fabs(a.x - b.x) < E &&
			fabs(a.y - b.y) < E &&
			fabs(a.z - b.z) < E);
	}

	float Interpolate(float x0, float x1, float xN, float y0, float y1) {
		if (y0 == y1)
			return y0;
		if (x0 > x1) {
			float temp = x0;
			x0 = x1;
			x1 = temp;
			temp = y0;
			y0 = y1;
			y1 = temp;
		}
		if (xN < x0)
			return y0;
		if (xN > x1)
			return y1;
		if (x0 - x1 < E && -(x0 - x1) < E)
			return sqrtf(y1 * y1 + y0 * y0);
		if (x0 - xN < E && -(x0 - xN) < E)
			return y0;
		if (x1 - xN < E && -(x1 - xN) < E)
			return y1;
		return y0 + (((y1 - y0) / (x1 - x0)) * (xN - x0));
	}

	void Intersect(std::vector<glm::vec3>& dots, glm::vec3& pos, glm::vec3& scale, glm::vec2& a, glm::vec2& b, Mesh* m, uint32_t s, uint32_t e) {
		glm::vec3 start(m->mesh[s + 0], m->mesh[s + 1], m->mesh[s + 2]);
		glm::vec3 end(m->mesh[e + 0], m->mesh[e + 1], m->mesh[e + 2]);
		start = start * scale + pos;
		end = end * scale + pos;
		glm::vec2 c(start.x, start.z);
		glm::vec2 d(end.x, end.z);
		glm::vec2* p = Ray2D::LineIntersect(a, b, c, d);
		if (p != nullptr) {
			float x0 = sqrtf(c.x * c.x + c.y * c.y);
			float x1 = sqrtf(d.x * d.x + d.y * d.y);
			float xN = sqrtf(p->x * p->x + p->y * p->y);
			float average = Interpolate(x0, x1, xN, start.y, end.y);
			float max = std::fmax(start.y, end.y);
			if (average > max)
				average = max;
			glm::vec3 result = glm::vec3(p->x, average, p->y);
			dots.push_back(result);
			delete p;
		}
	}

	void CrossSection(std::vector<glm::vec3>& dots, glm::vec2 a,glm::vec2 b) {
		Mesh* m = gladiator->GetComponent<Renderer3D>()->mesh;
		glm::vec3 pos = gladiator->GetPosition();
		glm::vec3 scale = gladiator->GetScale();
		uint32_t counter = 0;
		for (uint32_t i = 0; i < m->meshSize - 14; i += (14 * 3))
		{
			Intersect(dots,pos, scale, a, b, m, i, i + 14);
			Intersect(dots,pos, scale, a, b, m, i + 14, i + 2 * 14);
			Intersect(dots,pos, scale, a, b, m, i + 2 * 14, i);
			if (dots.size() % 2 == 1) {
				//std::cout << "FUCK\n";
				dots.push_back(dots.back());
			}
		}
	}

	virtual void OnUpdate() override {
		time += DeltaTime::deltatime;
		glm::vec3 forward = glm::vec3(-Window::GetCamera().forwardV.x, Window::GetCamera().forwardV.y, -Window::GetCamera().forwardV.z);
		Ray3D::RayCast(Window::GetCamera().GetPosition(), forward, &hit, 100);
		if(hit.Object != nullptr){
			if (Input::IsMouseDown(Keycode::MOUSE_BUTTON_2)) {
				if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1)) {
					if (hit.Object != nullptr) {
						int id = hit.Object->GetOwnerOfComponent()->id;
						Renderer::DeleteObject(id);
						hit.Object = nullptr;
						dirtCount++;
					}
				}
				else if (Input::IsKeyPressed(Keycode::KEY_G)) {
					if (dirtCount == 0)
						return;
					glm::vec3 pos = hit.Object->GetOwnerOfComponent()->GetPosition();
					glm::vec3 camPos = Window::GetCamera().GetPosition();
					glm::vec3 place = camPos + forward * hit.distance;
					glm::vec3 newPlace(0);
					if (place.x > pos.x + 0.4999)
						newPlace.x = pos.x + 1;
					else if (place.x < pos.x - 0.4)
						newPlace.x = pos.x - 1;
					else
						newPlace.x = pos.x;

					if (place.y > pos.y + 0.4999)
						newPlace.y = pos.y + 1;
					else if (place.y < pos.y - 0.4)
						newPlace.y = pos.y - 1;
					else
						newPlace.y = pos.y;

					if (place.z > pos.z + 0.4999)
						newPlace.z = pos.z + 1;
					else if (place.z < pos.z - 0.4999)
						newPlace.z = pos.z - 1;
					else
						newPlace.z = pos.z;
					CreateCube(0, 0);
					//new Line(camPos, place);
					cube.back()->GetComponent<Transform>()->Translate(newPlace.x, newPlace.y, newPlace.z);
					dirtCount--;
				}
			}
		}
	}

	float hp = 100.0f;
	bool isHovered = false;
	bool MoveWithMouse = true;
	bool value = false;
	float radius = 0;
	float panelRadius = 0;
	glm::vec3 pos;
	glm::vec3 scale;
	glm::vec3 rotation;

	virtual void OnGuiRender() {
		
	}
	
	void CreateCube(int i ,int j) {
		cube.push_back(new GameObject("dirt" + std::to_string(i) + ":" + std::to_string(j)));
		terrain->AddChild((void*)cube.back());
		cube.back()->SetOwner((void*)terrain);
		cube.back()->GetComponentManager()->RemoveComponent<Irenderer>();
		cube.back()->GetComponentManager()->AddComponent<Renderer3D>();
		MeshManager::GetMeshWhenLoaded("cube", (void*)cube.back()->GetComponent<Renderer3D>());
		cube.back()->GetComponent<Renderer3D>()->mat.ambient = 0.4f;
		cube.back()->GetComponent<Renderer3D>()->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
		cube.back()->GetComponent<Renderer3D>()->diffuseTexture = Texture::Create("src/Minecraft/Textures/minecraft_dirt.png");
		cube.back()->GetComponent<Transform>()->RotateOnce(0, 0, -90);
		cube.back()->GetComponent<Transform>()->Scale(0.5, 0.5, 0.5);
		cube.back()->GetComponentManager()->AddComponent<CubeCollider3D>();
	}
};