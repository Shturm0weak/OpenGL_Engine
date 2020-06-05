#pragma once

class StressTest : public Application {

	Texture* texture1 = nullptr;
	Texture* texture2 = nullptr;
	GameObject** gameobj = nullptr;
	unsigned int size = 0;
	Font* font = nullptr;

	void OnStart() {
		texture1 = new Texture("src/Images/coin.png");
		texture2 = new Texture("src/Images/bomb.png");
		gameobj = new GameObject*[316];
		float x = -10;
		float offset = 2;
		float y = -20;
		for (unsigned int i = 0; i < 316; i++)
		{
			gameobj[i] = new GameObject[316];
			for (unsigned int j = 0; j < 316; j++)
			{
				gameobj[i][j].GetComponentManager()->GetComponent<Transform>()->Translate(offset + x, y);
				if (i % 2 == 0 && j % 2 == 0)
					gameobj[i][j].SetTexture(texture2);
				else
					gameobj[i][j].SetTexture(texture1);
				x += offset;
				size += sizeof(gameobj[i][j]);
			}
			x = -10;
			y += 2;
		}
		Window::GetCamera().Zoom(5);
		//std::cout << (size / 1024.) / 1024. << std::endl;

		//std::cout << sizeof(*gameobj[0][0].GetComponentManager()->GetComponent<Transform>()) << std::endl;
		//std::cout << sizeof(*gameobj[0][0].GetComponentManager()) << std::endl;
		//std::cout << sizeof(*gameobj[0][0].GetTexturePointer()) << std::endl;
		//std::cout << sizeof(Renderer2DLayer::objects2d);
		font = new Font();
		font->LoadFont("src/font.txt", "src/arial.png");
		font->LoadCharacters();
	}
	
	virtual void OnUpdate() {
	}

	void OnClose(){
		delete[] gameobj;
	}
 };