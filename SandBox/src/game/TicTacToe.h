#pragma once
#include <random> 

struct Plate {
	GameObject* plate = nullptr;
	bool isEmpty = true;
	int owner = -1;

	Plate(){
		plate = new GameObject("Plate");
	}
};

class AI {
public:

	Plate* DecideWhereToPlace(Plate* plates) {
		for (unsigned int i = 0; i < 9; i++)
		{
			if (plates[i].isEmpty) {
				std::random_device rd;
				std::mt19937 e2(rd());
				std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
				float x = distribution(e2);
				if (i < 9 && i > 0 && (plates[i].owner != plates[i + 1].owner || plates[i].owner != plates[i - 1].owner))
					x += 0.1f;
				if (x > 0.7f)
					return &plates[i];
			}
		}
		for (unsigned int i = 0; i < 9; i++)
		{
			if (plates[i].isEmpty)
				return &plates[i];
		}
		
	}
};

class TicTacToe : public Application {
public:
	AI* ai = nullptr;

	Texture* crossLinesTexture = nullptr;
	Texture* circleTexture = nullptr;

	Plate* plates = nullptr;
	Font* font = nullptr;

	GameObject* background = nullptr;

	double timer = 1.0;
	double debugTextTimer = 1;

	int selected = -1;
	unsigned int turn = 0;

	unsigned int amountOfNonEmpty = 0;

	bool end = false;

	glm::vec2 mousePos;

	virtual void OnStart()override {
		ai = new AI();
		Window::GetCamera().Zoom(0.56);
		font = new Font();
		font->LoadFont("src/font.txt", "src/arial.png");
		font->LoadCharacters();

		plates = new Plate[9];
		float offsetX = -3.f;
		float offsetY = -3.f;
		unsigned int index = 0;

		crossLinesTexture = new Texture("src/Images/CrossLines.png");
		circleTexture = new Texture("src/Images/Circle.png");

		background = new GameObject("BackGround", 0, 0);
		background->GetComponentManager()->GetComponent<Transform>()->Scale(100, 100);
		background->SetColor(COLORS::Silver);
		background->Setlayer(0);
		background->AlwaysDraw = true;

		for (unsigned int i = 0; i < 9; i++)
		{
			if (index == 3) {
				offsetX = -3.f;
				offsetY += 3.f;
				index = 0;
			}
			plates[i].plate->GetComponentManager()->AddComponent<Collision>();
			Transform* tr = plates[i].plate->GetComponentManager()->GetComponent<Transform>();
			tr->Scale(2.9,2.9);
			tr->Translate(offsetX,offsetY);
			index++;
			offsetX += 3.f;
		}
	}

	virtual void OnUpdate()override {
		mousePos = glm::vec2(Window::GetMousePositionToWorldSpace().x, Window::GetMousePositionToWorldSpace().y);

		if (timer > 0.2f && Input::IsMousePressed(GLFW_MOUSE_BUTTON_1) && !end && turn == 1 && !end) {
			timer = 0.0;
			Plate* plate = isCollided(plates, &selected);
			if (plate != nullptr) {
				if (plate->isEmpty) {
					amountOfNonEmpty++;
					Sound::Play(L"src/Sounds/Place.wav");
					plate->isEmpty = false;
					if (turn == 0) {
						plate->plate->SetTexture(crossLinesTexture);
						plate->owner = turn;
						turn = 1;
					}
					else {
						plate->plate->SetTexture(circleTexture);
						plate->owner = turn;
						turn = 0;
					}
				}
			}
		}
		else if(timer > 0.4f && turn == 0 && !end){
			timer = 0.0;
			Plate* plate = nullptr;
			plate = ai->DecideWhereToPlace(plates);
			if (plate != nullptr) {
				if (plate->isEmpty) {
					Sound::Play(L"src/Sounds/Place.wav");
					amountOfNonEmpty++;
					plate->isEmpty = false;
					if (turn == 0) {
						plate->plate->SetTexture(crossLinesTexture);
						plate->owner = turn;
						turn = 1;
					}
					else {
						plate->plate->SetTexture(circleTexture);
						plate->owner = turn;
						turn = 0;
					}
				}
			}
		}


		timer += DeltaTime::deltatime;
		debugTextTimer += DeltaTime::deltatime;

		if(debugTextTimer > 0.1f){
			debugTextTimer = 0;
			Batch::GetInstance()->indexcount = 0;
			Batch::GetInstance()->Begin();
			Gui::GetInstance()->Text(font, "selected : %d", true, 30, 28, 22, COLORS::Red, 0, selected);
			if(selected >= 0)
				Gui::GetInstance()->Text(font, "IsEmpty : %d", true, 30, 22, 22, COLORS::Red, 0, plates[selected].isEmpty);
			Gui::GetInstance()->Text(font, "Mouse X : %f   Y : %f", true, 30, 31, 22, COLORS::Red, 2, mousePos.x, mousePos.y);
			Gui::GetInstance()->Text(font, "Not empty : %d", true, 30, 25, 22, COLORS::Red, 0, amountOfNonEmpty);
			Gui::GetInstance()->Text(font, "FPS : %f", true, 30, 19, 22, COLORS::Red, 0, Window::GetFPS());
			Batch::GetInstance()->End();
		}

		if (amountOfNonEmpty >= 9) {
			end = true;
			turn = 0;
			Batch::GetInstance()->indexcount = 0;
			Batch::GetInstance()->Begin();
			Gui::GetInstance()->Text(font, "           Draw\nPress Enter to restart", true, -40, 0, 50, COLORS::Yellow, 0);
			Batch::GetInstance()->End();

			if (Input::IsKeyPressed(Keycode::KEY_ENTER))
				Restart();
		}

		int whowin = WhoWin(plates);
		if (whowin >= 0) {
			turn = 0;
			Batch::GetInstance()->indexcount = 0;
			Batch::GetInstance()->Begin();
			Gui::GetInstance()->Text(font, "     Player %d won!!!\nPress Enter to restart", true,-40, 0, 50, COLORS::Yellow, 0, whowin);
			Batch::GetInstance()->End();
			end = true;

			if (Input::IsKeyPressed(Keycode::KEY_ENTER))
				Restart();
		}
	}

	virtual void OnClose()override {
		delete crossLinesTexture;
		delete circleTexture;
		delete[] plates;
		delete font;
		delete ai;
		delete background;
	}

	Plate* isCollided(Plate* plates,int* selected) {
		for (unsigned int i = 0; i < 9; i++)
		{
			Collision* col = plates[i].plate->GetComponentManager()->GetComponent<Collision>();
			if (col->ScaledVerPos[0] + col->GetPositions().x < mousePos.x && col->ScaledVerPos[4] + col->GetPositions().x > mousePos.x &&
				col->ScaledVerPos[1] + col->GetPositions().y < mousePos.y && col->ScaledVerPos[9] + col->GetPositions().y > mousePos.y){
				*selected = i;
				return &plates[i];
			}
		}
		*selected = -1;
		return nullptr;
	}

	int WhoWin(Plate* plates) {
		if (plates[0].owner == plates[1].owner && plates[0].owner == plates[2].owner && plates[0].owner != -1)
			return plates[0].owner;
		if (plates[3].owner == plates[4].owner && plates[3].owner == plates[5].owner && plates[3].owner != -1)
			return plates[3].owner;
		if (plates[6].owner == plates[7].owner && plates[6].owner == plates[8].owner && plates[6].owner != -1)
			return plates[6].owner;
		if (plates[0].owner == plates[3].owner && plates[0].owner == plates[6].owner && plates[0].owner != -1)
			return plates[0].owner;
		if (plates[1].owner == plates[4].owner && plates[1].owner == plates[7].owner && plates[1].owner != -1)
			return plates[1].owner;
		if (plates[2].owner == plates[5].owner && plates[2].owner == plates[8].owner && plates[2].owner != -1)
			return plates[2].owner;
		if (plates[0].owner == plates[4].owner && plates[0].owner == plates[8].owner && plates[0].owner != -1)
			return plates[0].owner;
		if (plates[2].owner == plates[4].owner && plates[2].owner == plates[6].owner && plates[2].owner != -1)
			return plates[2].owner;
		return -1;
	}

	void Restart() {
		for (unsigned int i = 0; i < 9; i++)
		{
			plates[i].isEmpty = true;
			plates[i].owner = -1;
			plates[i].plate->SetTexture(nullptr);
		}
		end = false;
		amountOfNonEmpty = 0;
		turn = 0;
		selected = -1;
	}
};