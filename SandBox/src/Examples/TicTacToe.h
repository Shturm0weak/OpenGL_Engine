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

class TicTacToe : public Application{
public:
	TicTacToe(): Application("Tic Tac Toe", TYPE_2D, 800, 600, false){

	}
	AI* ai = nullptr;
	Sound* backSound = new Sound("src/Sounds/Place.wav");
	Texture* crossLinesTexture = nullptr;
	Texture* circleTexture = nullptr;

	Plate* plates = nullptr;

	double timer = 1.0;

	int whowin = -1;
	int selected = -1;
	unsigned int turn = 0;

	unsigned int amountOfNonEmpty = 0;

	bool end = false;

	glm::vec2 mousePos;

	virtual void OnStart()override {
		GameObject* backGround = new GameObject("BackGround", 0, 0);
		backGround->GetComponentManager()->AddComponent<SpriteRenderer>()->m_Color = glm::vec4(0.2, 0.2, 0.2, 1.0);
		backGround->m_Transform->Scale(1000, 1000);
		SoundManager::CreateSoundAsset("back", backSound);
		ai = new AI();
		Window::GetCamera().Zoom(4.5);
		
		plates = new Plate[9];
		float offsetX = -3.f;
		float offsetY = -3.f;
		unsigned int index = 0;

		crossLinesTexture = Texture::Create("src/Images/CrossLines.png");
		circleTexture = Texture::Create("src/Images/Circle.png");

		for (unsigned int i = 0; i < 9; i++)
		{
			if (index == 3) {
				offsetX = -3.f;
				offsetY += 3.f;
				index = 0;
			}
			plates[i].plate->GetComponentManager()->AddComponent<RectangleCollider2D>();
			plates[i].plate->GetComponentManager()->AddComponent<SpriteRenderer>();
			Transform* tr = plates[i].plate->GetComponentManager()->GetComponent<Transform>();
			tr->Scale(2.9,2.9);
			tr->Translate(offsetX,offsetY);
			index++;
			offsetX += 3.f;
		}
	}

	virtual void OnUpdate()override {
		mousePos = glm::vec2(ViewPort::GetInstance()->GetMousePositionToWorldSpace().x, ViewPort::GetInstance()->GetMousePositionToWorldSpace().y);

		if (timer > 0.2f && Input::IsMousePressed(Keycode::MOUSE_BUTTON_1) && !end && turn == 1 && !end) {
			timer = 0.0;
			Plate* plate = isCollided(plates, &selected);
			if (plate != nullptr) {
				if (plate->isEmpty) {
					amountOfNonEmpty++;
					plate->isEmpty = false;
					if (turn == 0) {
						plate->plate->GetComponentManager()->GetComponent<SpriteRenderer>()->m_Texture = (crossLinesTexture);
						SoundManager::Play(backSound);
						plate->owner = turn;
						turn = 1;
					}
					else {
						plate->plate->GetComponentManager()->GetComponent<SpriteRenderer>()->m_Texture = (circleTexture);
						SoundManager::Play(backSound);
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
					amountOfNonEmpty++;
					plate->isEmpty = false;
					if (turn == 0) {
						plate->plate->GetComponentManager()->GetComponent<SpriteRenderer>()->m_Texture = (crossLinesTexture);
						SoundManager::Play(backSound);
						plate->owner = turn;
						turn = 1;
					}
					else {
						plate->plate->GetComponentManager()->GetComponent<SpriteRenderer>()->m_Texture = (circleTexture);
						SoundManager::Play(backSound);
						plate->owner = turn;
						turn = 0;
					}
				}
			}
		}


		timer += DeltaTime::s_Deltatime;

		if (amountOfNonEmpty >= 9) {
			end = true;
			turn = 0;
			Gui::GetInstance()->Begin();
			Gui::GetInstance()->m_XAlign = Gui::GetInstance()->AlignHorizontally::XCENTER;
			Gui::GetInstance()->m_YAlign = Gui::GetInstance()->AlignVertically::YCENTER;
			Gui::GetInstance()->Text("Draw", true, 0, 0, 40, COLORS::Yellow);
			Gui::GetInstance()->Text("Press Enter to restart", true, 0, -80, 40, COLORS::Yellow);
			Gui::GetInstance()->m_XAlign = Gui::GetInstance()->AlignHorizontally::LEFT;
			Gui::GetInstance()->m_YAlign = Gui::GetInstance()->AlignVertically::BOTTOM;
			Batch::GetInstance()->EndText();

			if (Input::IsKeyPressed(Keycode::KEY_ENTER))
				Restart();
		}

		whowin = WhoWin(plates);
		if (whowin > -1) {
			turn = 0;
			end = true;

			if (Input::IsKeyPressed(Keycode::KEY_ENTER))
				Restart();
		}
	}

	virtual void OnClose()override {
	}

	Plate* isCollided(Plate* plates,int* selected) {
		for (unsigned int i = 0; i < 9; i++)
		{
			RectangleCollider2D* col = plates[i].plate->GetComponentManager()->GetComponent<RectangleCollider2D>();
			if (col->m_TransformedVerPos[0] + col->GetOwnerOfComponent()->GetPosition().x < mousePos.x && col->m_TransformedVerPos[4] + col->GetOwnerOfComponent()->GetPosition().x > mousePos.x &&
				col->m_TransformedVerPos[1] + col->GetOwnerOfComponent()->GetPosition().y < mousePos.y && col->m_TransformedVerPos[9] + col->GetOwnerOfComponent()->GetPosition().y > mousePos.y){
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

	void OnGuiRender() {
		if (whowin > -1) {
			Gui::GetInstance()->m_XAlign = Gui::GetInstance()->AlignHorizontally::XCENTER;
			Gui::GetInstance()->m_YAlign = Gui::GetInstance()->AlignVertically::YCENTER;
			Gui::GetInstance()->Text("Player %d won!!!", true, -0, 0, 40, COLORS::Yellow, 0, whowin);
			Gui::GetInstance()->Text("Press Enter to restart", true, -0, -80, 40, COLORS::Yellow, 0, whowin);
			Gui::GetInstance()->m_XAlign = Gui::GetInstance()->AlignHorizontally::LEFT;
			Gui::GetInstance()->m_YAlign = Gui::GetInstance()->AlignVertically::BOTTOM;
		}
	}

	void Restart() {
		for (unsigned int i = 0; i < 9; i++)
		{
			plates[i].isEmpty = true;
			plates[i].owner = -1;
			plates[i].plate->GetComponentManager()->GetComponent<SpriteRenderer>()->m_Texture = (nullptr);
		}
		end = false;
		amountOfNonEmpty = 0;
		turn = 0;
		selected = -1;
	}
};