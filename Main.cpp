#include "Game.h"

void Fps(const float& delta)
{
	static int frame = 0;
	static float time = 0.0f;
	static int result = 0;
	//Фактически кадров в секунду
	time += delta;
	frame++;

	if (time > 0.5f)
	{
		result = int(frame / time);
		time = 0.0f;
		frame = 0;
	}
	std::cout << std::format("{:3} | ", result);
	//Vsyns уменьшение Fps до ~60 кадров
	float temp = (0.016666f - delta) * 1000.0f;
	if (temp > 0.0f)
		Sleep(DWORD(temp));
}

bool newgame = true;

bool Menu(C_Game& Game)
{
	//Сброс
	while (GetAsyncKeyState('W') & 0x01 || GetAsyncKeyState('S') & 0x01 || GetAsyncKeyState('D') & 0x01 || GetAsyncKeyState('A') & 0x01 || GetAsyncKeyState(VK_RETURN) & 0x01 ||
		GetAsyncKeyState(VK_ESCAPE) & 0x01 || GetAsyncKeyState(VK_LEFT) & 0x01 || GetAsyncKeyState(VK_RIGHT) & 0x01 || GetAsyncKeyState(VK_UP) & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01)
		Sleep(30);
	system("cls");
	std::cout << "Меню игры\n";
	std::cout << "  Новая игра\n";
	std::cout << "  Сохранить\n";
	std::cout << "  Загрузить\n";
	std::cout << "  Выход\n";
	int ans = 0;
	while (1)
	{
		for (int i = 0; i < 4; i++)
		{
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD(0, 1 + i));
			if (ans == i)
				std::cout << "->";
			else
				std::cout << "  ";
		}
		while (1)
		{
			if (GetAsyncKeyState('W') & 0x01 || GetAsyncKeyState(VK_UP) & 0x01)
			{
				ans--;
				if (ans < 0)
					ans = 3;
				break;
			}
			if (GetAsyncKeyState('S') & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01)
			{
				ans++;
				if (ans > 3)
					ans = 0;
				break;
			}
			if (GetAsyncKeyState(VK_RETURN) & 0x01)
			{
				if (ans == 0)
				{
					if (!newgame)
						Game.New_Game();
					else
						newgame = false;
				}
				else if (ans == 1)
					Game.Save();
				else if (ans == 2)
					Game.Load();
				else if (ans == 3)
					return true;
				system("cls");
				return false;
			}
			if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
			{
				system("cls");
				return false;
			}
			Sleep(30);
		}
	}
}

bool Dialog_Exit(C_Game& Game)
{
	if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
	{
		if (Menu(Game))
			return true;
	}
	return false;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	C_Game Game(1280, 720);
	if (!Menu(Game))
		while (!Dialog_Exit(Game))
		{
			static auto start = std::chrono::high_resolution_clock::now();
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> duration = end - start;
			start = end;
			Fps(duration.count());

			Game.Show(duration.count());
		}

	return 0;
}