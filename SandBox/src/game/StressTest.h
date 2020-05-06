#pragma once
class StressTest : public Application {

	Texture* texture1 = nullptr;
	Texture* texture2 = nullptr;
	GameObject** gameobj = nullptr;

	void OnStart() {
		texture1 = new Texture("src/Images/coin.png");
		texture2 = new Texture("src/Images/bomb.png");
		gameobj = new GameObject*[200];
		float x = -10;
		float offset = 2;
		float y = -20;
		for (unsigned int i = 0; i < 200; i++)
		{
			gameobj[i] = new GameObject[200];
			for (unsigned int j = 0; j < 200; j++)
			{
				gameobj[i][j].GetComponentManager()->GetComponent<Transform>()->Translate(offset + x, y);
				if (i % 2 == 0 && j % 2 == 0)
					gameobj[i][j].SetTexture(texture1);
				else
					gameobj[i][j].SetTexture(texture2);
				x += offset;
			}
			x = -10;
			y += 2;
		}
		Window::GetCamera().Zoom(5);

	}

	void OnClose(){
		delete[] gameobj;
	}
 };