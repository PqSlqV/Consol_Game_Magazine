#include"Game.h"

float Delta_Time = 0.0f;
void* handle = GetStdHandle(STD_OUTPUT_HANDLE);
int C_Workbench::Speed = 0;
int C_Workbench::Qual = 0;

C_Map::C_Map(const std::string& path)
{
	Path = path;
	std::ifstream f_map(path);
	while (!f_map.eof())
	{
		std::string temp;
		std::getline(f_map, temp);
		Map.push_back(temp);
	}
	f_map.close();
}

void C_Map::Init()
{
	std::ifstream f_map(Path);
	for (int i = 0; i < Map.size(); i++)
	{
		std::string temp;
		std::getline(f_map, temp);
		Map[i] = temp;
	}
	f_map.close();
}

void C_Map::Show(const std::vector<std::pair<C_Item, int>>& Inventory) const
{
	for (auto& now : Map)
		std::cout << now << '\n';
	int size = int(Map[0].size() + 1);
	SetConsoleCursorPosition(handle, COORD(size, 1));
	std::cout << "                      Инвентарь";
	SetConsoleCursorPosition(handle, COORD(size, 2));
	std::cout << "     Название        | Редк |  Цена  | Кол-во ";
	SetConsoleCursorPosition(handle, COORD(size, 3));
	std::cout << "-----------------------------------------------";
	int Y = 4;
	for (auto& now : Inventory)
	{
		SetConsoleCursorPosition(handle, COORD(size, Y++));
		std::cout << now.first.Show() << std::format("{:6}", now.second);
	}
}

void C_Map::Put_On_Map(const char& symbol, const Vec2& BeginPos, const Vec2& EndPos)
{
	for (int i = BeginPos.y; i <= EndPos.y; i++)
	{
		for (int j = BeginPos.x; j <= EndPos.x; j++)
		{
			Map[i][j] = symbol;
		}
	}
}

void C_Map::Put_On_Map(const char& symbol, const Vec2& BeginPos, const Vec2& EndPos, const int& Time)
{
	Put_On_Map(symbol, BeginPos, EndPos);
	if (Time >= 0)
	{
		int y = int((EndPos.y - BeginPos.y) / 2.0f) + BeginPos.y;
		std::string temp = std::format("{: ^{}}", int(Time), EndPos.x - BeginPos.x - 2);
		for (int i = BeginPos.x + 2, j = 0; i < EndPos.x - 1; i++, j++)
		{
			Map[y][i] = temp[j];
		}
	}
	else if (Time == -1)
	{
		int y = int((EndPos.y - BeginPos.y) / 2.0f) + BeginPos.y;
		std::string temp = std::format("{: ^{}}", "Готово", EndPos.x - BeginPos.x - 1);
		for (int i = BeginPos.x + 1, j = 0; i < EndPos.x; i++, j++)
		{
			Map[y][i] = temp[j];
		}
	}
}

bool C_Map::Collision(const Vec2& BeginPos, const Vec2& EndPos) const
{
	for (int i = BeginPos.y; i <= EndPos.y; i++)
	{
		for (int j = BeginPos.x; j <= EndPos.x; j++)
		{
			if (Map[i][j] != ' ')
				return true;
		}
	}
	return false;
}

const std::string& C_Item::Get_Name() const
{
	return Name;
}

const int& C_Item::Get_Rare() const
{
	return Rare;
}

const int& C_Item::Get_Price() const
{
	return Price;
}

void C_Item::Set_Rare(const int& rare)
{
	if (rare > Rare)
	{
		Price += int(Price * (0.2f * (rare - Rare)));
		Rare = rare;
	}
}

std::string C_Item::Show() const
{
	return std::format("{: >20} | {: >4} | {: >6} | ", Name, Rare, Price);
}

const bool C_Item::operator==(const C_Item& temp) const
{
	return Name == temp.Name && Rare == temp.Rare && Price == temp.Price;
}

C_WorkItem::C_WorkItem(const std::string& name, const int& rare, const C_Item& resource, const int& count) : C_Item(name, rare, 0)
{
	Resource.push_back(std::pair(resource, count));
	Price = resource.Get_Price() * count;
	Time = int(Price * 0.75f);
	Price = int(resource.Get_Price() * count * 1.5f);
}

C_WorkItem::C_WorkItem(const std::string& name, const int& rare, const std::vector<std::pair<C_Item, int>>& resource) : C_Item(name, rare, 0), Resource(resource)
{
	for (auto& now : resource)
		Price += now.first.Get_Price() * now.second;
	Time = int(Price * 0.75f);
	Price = int(Price * 1.5f);
}

std::string C_WorkItem::Show() const
{
	return std::format("{}{: >5} | ", C_Item::Show(), Time);
}

const int& C_WorkItem::Get_Time() const
{
	return Time;
}

const std::vector<std::pair<C_Item, int>>& C_WorkItem::Get_Resource() const
{
	return Resource;
}

C_Mesh::C_Mesh(std::ifstream& fobj)
{
	fobj.read((char*)(&Symbol), sizeof(char));
	fobj.read((char*)(&BeginPosition), sizeof(Vec2));
	fobj.read((char*)(&EndPosition), sizeof(Vec2));
}

void C_Mesh::Show(C_Map& Map) const
{
	Map.Put_On_Map(Symbol, BeginPosition, EndPosition);
}

bool C_Mesh::Mesh_Collision(const C_Mesh& first, const C_Mesh& second) const
{
	return first.BeginPosition.x <= second.EndPosition.x && first.BeginPosition.y <= second.EndPosition.y && second.BeginPosition.x <= first.EndPosition.x && second.BeginPosition.y <= first.EndPosition.y;
}

void C_Mesh::Save(std::ofstream& fobj)
{
	fobj.write((char*)(&Symbol), sizeof(char));
	fobj.write((char*)(&BeginPosition), sizeof(Vec2));
	fobj.write((char*)(&EndPosition), sizeof(Vec2));
}

C_Bench::C_Bench(std::ifstream& fobj) : C_Mesh(fobj)
{
	fobj.read((char*)(&In_Work), sizeof(bool));
	fobj.read((char*)(&Finish_Work), sizeof(bool));
	fobj.read((char*)(&First), sizeof(bool));
	fobj.read((char*)(&Work_Time), sizeof(float));
	fobj.read((char*)(&TimePosition), sizeof(Vec2));
}

void C_Bench::Work()
{
	if (In_Work)
	{
		if (!First)
		{
			Work_Time -= Delta_Time;
			if (Work_Time < 0.0f)
				Finish_Work = true;
		}
		else
			First = false;
	}
}

void C_Bench::Show_Work(C_Player& Player)
{
	//Сброс
	while (GetAsyncKeyState('W') & 0x01 || GetAsyncKeyState('S') & 0x01 || GetAsyncKeyState('D') & 0x01 || GetAsyncKeyState('A') & 0x01 || GetAsyncKeyState(VK_RETURN) & 0x01 ||
		GetAsyncKeyState(VK_ESCAPE) & 0x01 || GetAsyncKeyState(VK_LEFT) & 0x01 || GetAsyncKeyState(VK_RIGHT) & 0x01 || GetAsyncKeyState(VK_UP) & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01)
		Sleep(30);
	system("cls");

	if (In_Work)
	{
		if (Finish_Work)
		{
			In_Work = false;
			Finish_Work = false;
			std::cout << "Работа завершена!\n";
			Show_FinishWork(Player);
		}
		else
			Show_InWork();
	}
	else
		Set_Work(Player);
	system("cls");
}

void C_Bench::Show(C_Map& Map) const
{
	if (Finish_Work)
		Map.Put_On_Map(Symbol, BeginPosition, EndPosition, -1);
	else if (In_Work)
		Map.Put_On_Map(Symbol, BeginPosition, EndPosition, int(Work_Time));
	else
		Map.Put_On_Map(Symbol, BeginPosition, EndPosition, -2);
}

void C_Bench::Save(std::ofstream& fobj)
{
	C_Mesh::Save(fobj);
	fobj.write((char*)(&In_Work), sizeof(bool));
	fobj.write((char*)(&Finish_Work), sizeof(bool));
	fobj.write((char*)(&First), sizeof(bool));
	fobj.write((char*)(&Work_Time), sizeof(float));
	fobj.write((char*)(&TimePosition), sizeof(Vec2));
}

void C_Bench::Show_InWork()
{
	std::cout << "Работа еще не завершена!\nОсталось: " << int(Work_Time) << " секунд\n";
	std::cout << "->Понятно.";
	while (1)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x01 || GetAsyncKeyState(VK_ESCAPE) & 0x01)
			return;
		Sleep(30);
	}
}

C_Research::C_Research(const char& symbol, const Vec2& pos) : C_Mesh(symbol, Vec2(pos.x - 5, pos.y - 2), Vec2(pos.x + 5, pos.y + 2))
{
	Item.push_back(C_ResItem("Доп верстак", 0, 4, 100, 0));
	Item.push_back(C_ResItem("Скор верстаков", 0, 7, 50, 1));
	Item.push_back(C_ResItem("Мин качество", 0, 5, 150, 2));
}

void C_Research::Save(std::ofstream& fobj)
{
	int size = int(Item.size());
	fobj.write((char*)(&size), sizeof(int));
	for (auto& now : Item)
	{
		int namesize = int(now.Name.size() + 1);
		fobj.write((char*)(&namesize), sizeof(int));

		fobj.write((char*)(now.Name.data()), sizeof(char) * namesize);

		fobj.write((char*)(&now.lvl), sizeof(int));
		fobj.write((char*)(&now.Maxlvl), sizeof(int));
		fobj.write((char*)(&now.Price), sizeof(int));
		fobj.write((char*)(&now.Action), sizeof(int));
	}
}

void C_Research::Load(std::ifstream& fobj)
{
	Item.clear();
	int size = 0;
	fobj.read((char*)(&size), sizeof(int));
	for (int i = 0; i < size; i++)
	{
		C_ResItem temp;
		int sizename = 0;
		fobj.read((char*)(&sizename), sizeof(int));
		char* name = new char[sizename];
		fobj.read((char*)(name), sizeof(char) * sizename);
		temp.Name = name;
		delete[] name;
		fobj.read((char*)(&temp.lvl), sizeof(int));
		fobj.read((char*)(&temp.Maxlvl), sizeof(int));
		fobj.read((char*)(&temp.Price), sizeof(int));
		fobj.read((char*)(&temp.Action), sizeof(int));
		Item.push_back(temp);
	}
}

void C_Research::Dialog(C_Player& Player)
{
	if (Item.size() == 0)
	{
		std::cout << "В данный момент нет доступных чертежей.\n";
		std::cout << "->Понятно.";
		while (1)
		{
			if (GetAsyncKeyState(VK_RETURN) & 0x01 || GetAsyncKeyState(VK_ESCAPE) & 0x01)
				return;
			Sleep(30);
		}
	}
	else
	{
		int i = 0;
		while (1)
		{
			SetConsoleCursorPosition(handle, COORD(0, 0));
			std::cout << "Выберите чертеж:\n";
			std::cout << "        Название       | Уровень |  Макс  |   Цена  |\n";
			std::cout << "-----------------------------------------------------\n";
			for (int j = 0; j < Item.size(); j++)
			{
				if (i == j)
					std::cout << "->";
				else
					std::cout << "  ";
				std::cout << std::format("{:>20} | {: 7} | {: 6} | {: 7} |\n", Item[j].Name, Item[j].lvl, Item[j].Maxlvl, Item[j].Price);
			}
			std::cout << std::format("{:>41} | {: 7} |", "Йен", Player.Get_Money());

			while (1)
			{
				if (GetAsyncKeyState('W') & 0x01 || GetAsyncKeyState(VK_UP) & 0x01)
				{
					i--;
					if (i < 0)
						i = int(Item.size() - 1);
					break;
				}
				if (GetAsyncKeyState('S') & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01)
				{
					i++;
					if (i > Item.size() - 1)
						i = 0;
					break;
				}
				if (GetAsyncKeyState(VK_RETURN) & 0x01)
				{
					if (Player.Set_Money(Item[i].Price))
					{
						Action(i);
						Item[i].lvl++;
						Item[i].Price = int(Item[i].Price * 1.5f);
						if (Item[i].lvl >= Item[i].Maxlvl)
						{
							auto it = Item.begin();
							std::advance(it, i);
							Item.erase(it);
						}
						return;
					}
					break;
				}
				if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
					return;
			}
			Sleep(30);
		}
	}
}

void C_Research::Set_Work(std::vector<C_Workbench>* work, const std::vector<C_Item>* res)
{
	Workbench = work;
	Resbench = res;
}

const bool C_Research::Get_Act()
{
	return Act;
}

void C_Research::Set_Act()
{
	Act = false;
}

void C_Research::Action(const int& ind)
{
	if (Item[ind].Action == 0)
		Workbench->push_back(C_Workbench('W', Vec2(50 + Item[ind].lvl * 15, 4), *Resbench));
	else if (Item[ind].Action == 1)
		C_Workbench::Speed++;
	else if (Item[ind].Action == 2)
		C_Workbench::Qual++;
	Act = true;
}

C_Resbench::C_Resbench(const char& symbol, const Vec2& pos) : C_Bench(symbol, pos)
{
	Add_Item();
}

C_Resbench::C_Resbench(std::ifstream& fobj) : C_Bench(fobj)
{
	Add_Item();
	int size = 0;
	fobj.read((char*)(&size), sizeof(int));
	for (int i = 0; i < size; i++)
	{
		int ind = 0;
		fobj.read((char*)(&ind), sizeof(int));
		int count = 0;
		fobj.read((char*)(&count), sizeof(int));
		AcceptItem.push_back(std::pair(&Item[ind], count));
	}
}

void C_Resbench::Save(std::ofstream& fobj)
{
	C_Bench::Save(fobj);
	int size = int(AcceptItem.size());
	fobj.write((char*)(&size), sizeof(int));
	for (auto& now : AcceptItem)
	{
		for (int i = 0; i < Item.size(); i++)
		{
			if (Item[i] == *(now.first))
			{
				fobj.write((char*)(&i), sizeof(int));
				fobj.write((char*)(&now.second), sizeof(int));
				break;
			}
		}
	}
}

const std::vector<C_Item>& C_Resbench::Get_Item() const
{
	return Item;
}

void C_Resbench::Set_Work(C_Player& Player)
{
	if (Item.size() == 0)
	{
		std::cout << "В данный момент нет доступных ресурсов.\n";
		std::cout << "->Понятно.";
		while (1)
		{
			if (GetAsyncKeyState(VK_RETURN) & 0x01 || GetAsyncKeyState(VK_ESCAPE) & 0x01)
				return;
			Sleep(30);
		}
	}
	else
	{
		auto& Inventory = Player.Get_Resourse();
		int i = 0;
		std::vector<std::pair<int, int>> Count;
		for (auto& now : Item)
			Count.push_back(std::pair(0, 0));
		int Sum = 0;
		while (1)
		{
			SetConsoleCursorPosition(handle, COORD(0, 0));
			std::cout << "Выберите предмет:\n";
			std::cout << "       Название        | Редк |  Цена  | Кол-во |  Сумма  | Имеется |\n";
			std::cout << "---------------------------------------------------------------------\n";

			for (int j = 0; j < Item.size(); j++)
			{
				if (i == j)
					std::cout << "->";
				else
					std::cout << "  ";
				std::cout << Item[j].Show() << std::format("{:6} | {:7} | {:7} | \n", Count[j].first, Count[j].second, Inventory[j].second);
			}
			std::cout << std::format("{:>48}: {:7} | {:7} | \n", "Йен", Sum, Player.Get_Money());


			while (1)
			{
				if (GetAsyncKeyState('W') & 0x01 || GetAsyncKeyState(VK_UP) & 0x01)
				{
					i--;
					if (i < 0)
						i = int(Item.size() - 1);
					break;
				}
				if (GetAsyncKeyState('S') & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01)
				{
					i++;
					if (i >= Item.size())
						i = 0;
					break;
				}
				if (GetAsyncKeyState('A') & 0x01 || GetAsyncKeyState(VK_LEFT) & 0x01)
				{
					Count[i].first--;
					if (Count[i].first < 0)
						Count[i].first = 99;
					Count[i].second = Item[i].Get_Price() * Count[i].first;
					Sum = 0;
					for (auto& now : Count)
						Sum += now.second;
					break;
				}
				if (GetAsyncKeyState('D') & 0x01 || GetAsyncKeyState(VK_RIGHT) & 0x01)
				{
					Count[i].first++;
					if (Count[i].first > 99)
						Count[i].first = 0;
					Count[i].second = Item[i].Get_Price() * Count[i].first;
					Sum = 0;
					for (auto& now : Count)
						Sum += now.second;
					break;
				}
				if (GetAsyncKeyState(VK_RETURN) & 0x01 && Sum != 0)
				{
					if (Player.Set_Money(Sum))
					{
						for (int j = 0; j < Item.size(); j++)
						{
							if (Count[j].first > 0)
								AcceptItem.push_back(std::pair(&Item[j], Count[j].first));
						}
						Work_Time = 20.0f;
						In_Work = true;
						First = true;
						return;
					}
					break;
				}
				if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
				{
					return;
				}
				Sleep(30);
			}
		}
	}
}

void C_Resbench::Show_FinishWork(C_Player& Player)
{
	auto& Inventory = Player.Get_Resourse();
	std::cout << "Можете забрать ваш заказ!\n";
	std::cout << "       Название      | Редк |  Цена  | Кол-во |\n";
	std::cout << "-----------------------------------------------\n";
	for (auto& acc : AcceptItem)
	{
		std::cout << acc.first->Show() << std::format("{:6} | \n", acc.second);
	}

	std::cout << "->Забрать.";
	while (1)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x01 || GetAsyncKeyState(VK_ESCAPE) & 0x01)
		{
			for (auto& acc : AcceptItem)
			{
				for (auto& inv : Inventory)
				{
					if (inv.first == acc.first->Get_Name())
					{
						inv.second += acc.second;
						break;
					}
				}
			}
			std::vector<std::pair<C_Item*, int>>().swap(AcceptItem);
			return;
		}
		Sleep(30);
	}
}

void C_Resbench::Add_Item()
{
	Item.push_back(C_Item("Дерево", 0, 8));
	Item.push_back(C_Item("Железо", 0, 12));
	Item.push_back(C_Item("Медь", 0, 25));
	Item.push_back(C_Item("Серебро", 0, 50));
	Item.push_back(C_Item("Золото", 0, 100));
}

C_Workbench::C_Workbench(const char& symbol, const Vec2& pos, const std::vector<C_Item>& Res) : C_Bench(symbol, pos)
{
	Add_Item(Res);
}

C_Workbench::C_Workbench(std::ifstream& fobj, const std::vector<C_Item>& Res) : C_Bench(fobj)
{
	Add_Item(Res);
	fobj.read((char*)(&Accept_Object), sizeof(int));
	fobj.read((char*)(&Maxobj), sizeof(int));
}

const std::vector<C_WorkItem>& C_Workbench::Get_Work_Item() const
{
	return Item;
}

void C_Workbench::Save(std::ofstream& fobj)
{
	C_Bench::Save(fobj);
	fobj.write((char*)(&Accept_Object), sizeof(int));
	fobj.write((char*)(&Maxobj), sizeof(int));
}

void C_Workbench::Set_Work(C_Player& Player)
{
	if (Item.size() == 0)
	{
		std::cout << "В данный момент нет доступных чертежей.\n";
		std::cout << "->Понятно.";
		while (1)
		{
			if (GetAsyncKeyState(VK_RETURN) & 0x01 || GetAsyncKeyState(VK_ESCAPE) & 0x01)
				return;
			Sleep(30);
		}
	}
	else
	{
		auto& Inventory = Player.Get_Inventory();
		auto& Resourse = Player.Get_Resourse();
		int i = 0;
		while (1)
		{
			SetConsoleCursorPosition(handle, COORD(0, 0));
			std::cout << "Выберите чертеж:\n";
			std::cout << "                            Верстак                                                             Ресурсы\n";
			std::cout << "        Название       | Редк |  Цена  | Время | Имеется |             Название       | Редк |  Цена  | Кол-во | Имеется |\n";
			std::cout << "  --------------------------------------------------------      ----------------------------------------------------------\n";
			auto& NeedRes = Item[i].Get_Resource();
			bool CanWork = true;
			for (int j = 0; j < Maxobj; j++)
			{
				if (i == j)
					std::cout << "->";
				else
					std::cout << "  ";

				std::cout << Item[j].Show();
				int find = 0;
				for (auto& now : Inventory)
				{
					if (Item[j] == now.first)
					{
						find = now.second;
						break;
					}
				}
				std::cout << std::format("{:7} | {:70}\n", find, ' ');

			}
			for (int j = 0; j < NeedRes.size(); j++)
			{
				int find = 0;
				for (auto& now : Resourse)
				{
					if (now.first == NeedRes[j].first.Get_Name())
					{
						find = now.second;
						break;
					}
				}
				if (find < NeedRes[j].second)
					CanWork = false;
				SetConsoleCursorPosition(handle, COORD(65, 4 + j));
				std::cout << NeedRes[j].first.Show() << std::format("{:6} | {:7} | \n", NeedRes[j].second, find);
			}
			while (1)
			{
				if (GetAsyncKeyState('S') & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01)
				{
					i++;
					if (i >= Maxobj)
						i = 0;
					break;
				}
				if (GetAsyncKeyState('W') & 0x01 || GetAsyncKeyState(VK_UP) & 0x01)
				{
					i--;
					if (i < 0)
						i = Maxobj - 1;
					break;
				}
				if (GetAsyncKeyState(VK_RETURN) & 0x01)
				{
					//Уменьшение
					if (CanWork)
					{
						Accept_Object = i;
						bool del = false;
						for (auto& ires : Item[i].Get_Resource())
						{
							for (auto& res : Resourse)
							{
								if (ires.first.Get_Name() == res.first)
								{
									res.second -= ires.second;
								}
							}
						}
						In_Work = true;
						First = true;
						Work_Time = float(Item[i].Get_Time() * ((10 - Speed) / 10.0f));
						return;
					}
				}
				if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
					return;
				Sleep(30);
			}
		}
	}
}

void C_Workbench::Show_FinishWork(C_Player& Player)
{
	auto& Inventory = Player.Get_Inventory();
	C_Item temp(Item[Accept_Object].Get_Name(), Item[Accept_Object].Get_Rare(), Item[Accept_Object].Get_Price());
	std::mt19937 gen(std::random_device().operator()());
	std::uniform_int_distribution<int> Uid(0, 100);
	int rare = Uid(gen);
	if (rare <= 40)
		rare = 0;
	else if (rare <= 70)
		rare = 1;
	else if (rare <= 85)
		rare = 2;
	else if (rare <= 95)
		rare = 3;
	else if (rare <= 99)
		rare = 4;
	else if (rare <= 100)
		rare = 5;
	if (rare < Qual)
		rare = Qual;
	temp.Set_Rare(rare);
	std::cout << "Ваш предмет: \n";
	std::cout << "      Название       | Редк |  Цена  | Кол-во |\n";
	std::cout << "-----------------------------------------------\n";
	std::cout << temp.Show() << std::format("{:6} | \n", 1) << "->Забрать.";
	if (Maxobj < Item.size())
	{
		if (Accept_Object + 1 == Maxobj)
			Maxobj++;
	}
	while (1)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x01 || GetAsyncKeyState(VK_ESCAPE) & 0x01)
		{
			for (auto& now : Inventory)
			{
				if (now.first == temp)
				{
					now.second++;
					return;
				}
			}
			Inventory.push_back(std::pair(temp, 1));
			return;
		}
		Sleep(30);
	}
}

void C_Workbench::Add_Item(const std::vector<C_Item>& Res)
{
	Item.push_back(C_WorkItem("Палка", 0, Res[0], 2));
	Item.push_back(C_WorkItem("Кинжал", 0, Res[1], 2));
	Item.push_back(C_WorkItem("Лук", 0, Res[0], 4));
	Item.push_back(C_WorkItem("Меч", 0, Res[1], 5));
	Item.push_back(C_WorkItem("Простой доспех", 0, std::vector<std::pair<C_Item, int>>{std::pair(Res[0], 1), std::pair(Res[1], 5)}));
	Item.push_back(C_WorkItem("Секира", 0, std::vector<std::pair<C_Item, int>>{std::pair(Res[0], 3), std::pair(Res[1], 7)}));
	Item.push_back(C_WorkItem("Медный шлем", 0, Res[2], 6));
	Item.push_back(C_WorkItem("Пистолет", 0, std::vector<std::pair<C_Item, int>>{std::pair(Res[1], 5), std::pair(Res[2], 5)}));
	Item.push_back(C_WorkItem("Сереб ожерелье", 0, Res[3], 6));
	Item.push_back(C_WorkItem("Шпага звезд", 0, std::vector<std::pair<C_Item, int>>{std::pair(Res[1], 3), std::pair(Res[3], 6)}));
	Item.push_back(C_WorkItem("Золотое кольцо", 0, Res[4], 4));
	Item.push_back(C_WorkItem("Доспехи эльфов", 0, Res[3], 10));
	Item.push_back(C_WorkItem("Топор Ритта", 0, std::vector<std::pair<C_Item, int>>{std::pair(Res[3], 5), std::pair(Res[4], 10)}));
	Item.push_back(C_WorkItem("Доспех королей", 0, Res[4], 20));
	Item.push_back(C_WorkItem("Идеальный куб", 0, std::vector<std::pair<C_Item, int>>{std::pair(Res[0], 20), std::pair(Res[1], 20), std::pair(Res[2], 20), std::pair(Res[3], 20), std::pair(Res[4], 20)}));
}

void C_Npc::Moving(const C_Map& Map, const C_Player& Player, const std::vector<C_Npc>& Npc)
{
	if (Move_Time > 0.1f)
	{
		Vec2 Begintemp = BeginPosition;
		Vec2 Endtemp = EndPosition;
		Move();
		if (Collision(Map, Player, Npc))
		{
			BeginPosition = Begintemp;
			EndPosition = Endtemp;
		}
		Move_Time = 0;
	}
	Move_Time += Delta_Time;
	Live -= Delta_Time;
}

void C_Npc::Set_Delete()
{
	Live = 0.0f;
	NextPos = { 57, 36 };
}

bool C_Npc::Get_Delete()
{
	return Delete;
}

bool C_Npc::Get_Life()
{
	return Live > 0.0f;
}

const C_Item& C_Npc::Get_Wish() const
{
	return Wish;
}

void C_Npc::Move()
{
	if (Live <= 0.0f)
	{
		if (BeginPosition + 1 == Vec2(53, 36))
			Delete = true;
		else
			NextPos = { 53, 36 };
	}
	else if (NextPos == BeginPosition + 1)
	{
		std::mt19937 gen(std::random_device().operator()());
		std::uniform_int_distribution<int> Xid(3, 103);
		std::uniform_int_distribution<int> Yid(13, 36);
		NextPos = { Xid(gen),Yid(gen) };
	}

	if (NextPos.x != BeginPosition.x + 1)
	{
		BeginPosition.x + 1 < NextPos.x ? (BeginPosition.x++, EndPosition.x++) : (BeginPosition.x--, EndPosition.x--);
	}
	if (NextPos.y != BeginPosition.y + 1)
	{
		BeginPosition.y + 1 < NextPos.y ? (BeginPosition.y++, EndPosition.y++) : (BeginPosition.y--, EndPosition.y--);
	}
}

bool C_Npc::Collision(const C_Map& Map, const C_Player& Player, const std::vector<C_Npc>& Npc) const
{
	if (Map.Collision(BeginPosition, EndPosition))
		return true;
	if (Mesh_Collision(*this, Player))
	{
		if (Live <= 0.0f)
			return false;
		return true;
	}
	/*for (auto& now : Npc)
	{
		if (now.BeginPosition == BeginPosition && now.EndPosition == EndPosition)
			continue;
		if (Mesh_Collision(*this, now))
			return true;
	}*/

	return false;
}

void C_Player::Moving(const C_Map& Map, std::vector<C_Npc>& Npc, std::vector <C_Bench*>& Workbench, C_Research& Research)
{
	float Move_Speed = 0.05f;
	if (GetKeyState(VK_LSHIFT) < 0)
	{
		Move_Speed = 0.02f;
	}

	if (Move_Time > Move_Speed)
	{
		Vec2 Begintemp = BeginPosition;
		Vec2 Endtemp = EndPosition;
		if (Move())
		{
			if (Collision(Map, Npc, Workbench, Research))
			{
				BeginPosition = Begintemp;
				EndPosition = Endtemp;
			}
		}
		Move_Time = 0.0f;
		return;
	}
	Move_Time += Delta_Time;
}

const std::vector<std::pair<C_Item, int>>& C_Player::Show_Inventory() const
{
	return Inventory;
}

const std::vector<std::pair<std::string, int>>& C_Player::Show_Resourse() const
{
	return Resourse;
}

std::vector<std::pair<C_Item, int>>& C_Player::Get_Inventory()
{
	return Inventory;
}

std::vector<std::pair<std::string, int>>& C_Player::Get_Resourse()
{
	return Resourse;
}

const bool C_Player::Set_Money(const int& money)
{
	if (Money - money >= 0)
	{
		Money -= money;
		return true;
	}
	else
		return false;
}

const int& C_Player::Get_Money() const
{
	return Money;
}

void C_Player::Save(std::ofstream& fobj)
{
	C_Mesh::Save(fobj);
	fobj.write((char*)(&Money), sizeof(int));
	int invsize = int(Inventory.size());
	fobj.write((char*)(&invsize), sizeof(int));
	for (auto& now : Inventory)
	{
		int namesize = int(now.first.Get_Name().size() + 1);
		fobj.write((char*)(&namesize), sizeof(int));
		fobj.write((char*)(now.first.Get_Name().data()), sizeof(char) * namesize);
		fobj.write((char*)(&now.first.Get_Rare()), sizeof(int));
		fobj.write((char*)(&now.first.Get_Price()), sizeof(int));
		fobj.write((char*)(&now.second), sizeof(int));
	}

	int ressize = int(Resourse.size());
	fobj.write((char*)(&ressize), sizeof(int));
	for (auto& now : Resourse)
	{
		fobj.write((char*)(&now.second), sizeof(int));
	}
}

void C_Player::Load(std::ifstream& fobj)
{
	fobj.read((char*)(&Symbol), sizeof(char));
	fobj.read((char*)(&BeginPosition), sizeof(Vec2));
	fobj.read((char*)(&EndPosition), sizeof(Vec2));
	fobj.read((char*)(&Money), sizeof(int));
	int sizeInv = 0;
	fobj.read((char*)(&sizeInv), sizeof(int));
	Inventory.clear();
	for (int i = 0; i < sizeInv; i++)
	{
		int sizename = 0;
		fobj.read((char*)(&sizename), sizeof(int));
		char* name = new char[sizename];
		fobj.read((char*)(name), sizeof(char) * sizename);
		int rare = 0;
		fobj.read((char*)(&rare), sizeof(int));
		int price = 0;
		fobj.read((char*)(&price), sizeof(int));
		int count = 0;
		fobj.read((char*)(&count), sizeof(int));
		Inventory.push_back(std::pair(C_Item(name, rare, price), count));
		delete[] name;
	}
	int sizeRes = 0;
	fobj.read((char*)(&sizeRes), sizeof(int));
	if (sizeRes != Resourse.size())
	{
		std::cout << "Load::FailResCount\n";
		exit(-1);
	}
	for (auto& now : Resourse)
	{
		fobj.read((char*)(&now.second), sizeof(int));
	}
}

bool C_Player::Move()
{
	bool ret = false;
	if (GetKeyState('W') < 0 || GetKeyState(VK_UP) < 0)
	{
		BeginPosition.y -= 1;
		EndPosition.y -= 1;
		ret = true;
	}
	if (GetKeyState('S') < 0 || GetKeyState(VK_DOWN) < 0)
	{
		BeginPosition.y += 1;
		EndPosition.y += 1;
		ret = true;
	}
	if (GetKeyState('A') < 0 || GetKeyState(VK_LEFT) < 0)
	{
		BeginPosition.x -= 1;
		EndPosition.x -= 1;
		ret = true;
	}
	if (GetKeyState('D') < 0 || GetKeyState(VK_RIGHT) < 0)
	{
		BeginPosition.x += 1;
		EndPosition.x += 1;
		ret = true;
	}
	return ret;
}

bool C_Player::Collision(const C_Map& Map, std::vector<C_Npc>& Npc, std::vector <C_Bench*>& Workbench, C_Research& Research)
{
	if (Map.Collision(BeginPosition, EndPosition))
		return true;
	if (Mesh_Collision(*this, Research))
	{
		//Сброс
		while (GetAsyncKeyState('W') & 0x01 || GetAsyncKeyState('S') & 0x01 || GetAsyncKeyState('D') & 0x01 || GetAsyncKeyState('A') & 0x01 || GetAsyncKeyState(VK_RETURN) & 0x01 ||
			GetAsyncKeyState(VK_ESCAPE) & 0x01 || GetAsyncKeyState(VK_LEFT) & 0x01 || GetAsyncKeyState(VK_RIGHT) & 0x01 || GetAsyncKeyState(VK_UP) & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01)
			Sleep(30);
		system("cls");
		Research.Dialog(*this);
		system("cls");
		return true;
	}
	for (auto it = Workbench.begin(); it != Workbench.end(); ++it)
	{
		if (Mesh_Collision(*this, **it))
		{
			(*it)->Show_Work(*this);
			return true;
		}
	}
	for (auto it = Npc.begin(); it != Npc.end(); ++it)
	{
		if (Mesh_Collision(*this, *it))
		{
			//Сброс
			while (GetAsyncKeyState('W') & 0x01 || GetAsyncKeyState('S') & 0x01 || GetAsyncKeyState('D') & 0x01 || GetAsyncKeyState('A') & 0x01 || GetAsyncKeyState(VK_RETURN) & 0x01 ||
				GetAsyncKeyState(VK_ESCAPE) & 0x01 || GetAsyncKeyState(VK_LEFT) & 0x01 || GetAsyncKeyState(VK_RIGHT) & 0x01 || GetAsyncKeyState(VK_UP) & 0x01 || GetAsyncKeyState(VK_DOWN) & 0x01)
				Sleep(30);
			system("cls");
			if (Dialog_Buy(it))
				it->Set_Delete();

			system("cls");
			return true;
		}
	}
	return false;
}

bool C_Player::Dialog_Buy(std::vector<C_Npc>::iterator& it)
{
	if (!it->Get_Life())
	{

		std::cout << "Я уже ухожу.\n";
		std::cout << "->Понятно.";
		while (1)
		{
			if (GetAsyncKeyState(VK_RETURN) & 0x01 || GetAsyncKeyState(VK_ESCAPE) & 0x01)
				return false;
			Sleep(30);
		}
	}
	int ans = 0;
	if (it->Get_Wish().Get_Name() == "Гуляю")
	{
		while (1)
		{
			SetConsoleCursorPosition(handle, COORD(0, 0));
			std::cout << "Здравствуйте, я просто осматриваюсь у вас.\n";
			if (ans == 0)
				std::cout << "  ->Оставить     Прогнать\n";
			else
				std::cout << "    Оставить   ->Прогнать\n";
			while (1)
			{
				if (GetAsyncKeyState('D') & 0x01 || GetAsyncKeyState(VK_RIGHT) & 0x01)
				{
					ans++;
					if (ans > 1)
						ans = 0;
					break;
				}
				if (GetAsyncKeyState('A') & 0x01 || GetAsyncKeyState(VK_LEFT) & 0x01)
				{
					ans--;
					if (ans < 0)
						ans = 1;
					break;
				}
				if (GetAsyncKeyState(VK_RETURN) & 0x01)
				{
					if (ans == 0)
						return false;
					else if (ans == 1)
						return true;
					break;
				}
				if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
					return false;
				Sleep(30);
			}
		}
	}
	else
	{
		while (1)
		{
			SetConsoleCursorPosition(handle, COORD(0, 0));
			std::cout << "Здравствуйте, я бы хотел купить:\n";
			std::cout << "      Название       | Редк |  Цена  | Кол-во |  Сумма  | Имеется |\n";
			std::cout << "-------------------------------------------------------------------\n";
			int count = 0;
			auto CanTrade = Inventory.end();
			for (auto itInv = Inventory.begin(); itInv != CanTrade; ++itInv)
			{
				if (itInv->first == it->Get_Wish())
				{
					CanTrade = itInv;
					count = itInv->second;
					break;
				}
			}
			std::cout << it->Get_Wish().Show() << std::format("{:6} | {:7} | {:7} | \n", 1, it->Get_Wish().Get_Price(), count);
			std::cout << "Продать?\n";
			if (ans == 0)
				std::cout << "  ->Согласиться     Уйти     Прогнать\n";
			else if (ans == 1)
				std::cout << "    Согласиться   ->Уйти     Прогнать\n";
			else if (ans == 2)
				std::cout << "    Согласиться     Уйти   ->Прогнать\n";
			while (1)
			{
				if (GetAsyncKeyState('D') & 0x01 || GetAsyncKeyState(VK_RIGHT) & 0x01)
				{
					ans++;
					if (ans > 2)
						ans = 0;
					break;
				}
				if (GetAsyncKeyState('A') & 0x01 || GetAsyncKeyState(VK_LEFT) & 0x01)
				{
					ans--;
					if (ans < 0)
						ans = 2;
					break;
				}
				if (GetAsyncKeyState(VK_RETURN) & 0x01)
				{
					if (ans == 0)
					{
						if (CanTrade != Inventory.end())
						{
							CanTrade->second--;
							Money += it->Get_Wish().Get_Price();
							if (CanTrade->second == 0)
								Inventory.erase(CanTrade);
							return true;
						}
					}
					else if (ans == 1)
						return false;
					else if (ans == 2)
						return true;
					break;
				}
				if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
					return false;
			}
			Sleep(30);
		}
	}
}

C_Game::C_Game(const int& width, const int& height) : Map("Resourse/Shop.txt"), Player('P', Vec2(10, 10)), Research('R', Vec2(7, 4))
{
	HDC hDCScreen = GetDC(NULL);
	int Horres = GetDeviceCaps(hDCScreen, HORZRES);
	int Vertres = GetDeviceCaps(hDCScreen, VERTRES);
	HWND hWindowConsole = GetConsoleWindow();
	if (Horres > width && Vertres > height)
		MoveWindow(hWindowConsole, (Horres - width) / 2, (Vertres - height) / 2, width, height, TRUE);
	else
		MoveWindow(hWindowConsole, 0, 0, width, height, TRUE);
	CONSOLE_CURSOR_INFO structCursorInfo;
	GetConsoleCursorInfo(handle, &structCursorInfo);
	structCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(handle, &structCursorInfo);
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	Init_Object();
}

void C_Game::Show(const float& delta)
{
	Delta_Time = delta;
	Map.Init();
	Logic();
	Print();
	SetConsoleCursorPosition(handle, COORD(0, 0));
}

void C_Game::Init_Object()
{
	Resbench.push_back(C_Resbench('S', Vec2(21, 4)));
	if (Player.Get_Resourse().size() == 0)
	{
		for (auto& now : Resbench[0].Get_Item())
		{
			Player.Get_Resourse().push_back(std::pair(now.Get_Name(), 0));
		}
	}
	//Верстак
	Workbench.push_back(C_Workbench('W', Vec2(35, 4), Resbench[0].Get_Item()));
	Research.Set_Work(&Workbench, &(Resbench[0].Get_Item()));
	for (auto& now : Resbench)
		Bench.push_back(&now);
	for (auto& now : Workbench)
		Bench.push_back(&now);
}

void C_Game::Logic()
{
	for (auto& now : Bench)
		now->Work();
	Add_Npc();
	Player.Moving(Map, Npc, Bench, Research);
	if (Research.Get_Act())
	{
		Research.Set_Act();
		Bench.clear();
		for (auto& now : Resbench)
			Bench.push_back(&now);
		for (auto& now : Workbench)
			Bench.push_back(&now);
	}
	Delete_Npc();
	for (auto& now : Npc)
		now.Moving(Map, Player, Npc);
}

void C_Game::Print()
{
	Player.Show(Map);
	Research.Show(Map);
	for (auto& now : Bench)
		now->Show(Map);
	for (auto& now : Npc)
		now.Show(Map);
	std::cout << std::format("{}:{:6}", "Йена", Player.Get_Money());
	for (auto& now : Player.Get_Resourse())
		std::cout << std::format(" | {}:{:5}", now.first, now.second);
	std::cout << '\n';
	Map.Show(Player.Show_Inventory());
}

void C_Game::Add_Npc()
{
	if (Npc.size() < 10)
	{
		if (New_Npc < 0.0f)
		{
			std::mt19937 gen(std::random_device().operator()());
			std::uniform_int_distribution<int> Uid('A', 'Z');
			char symbol;
			while ((symbol = Uid(gen)) == 'X' || symbol == 'P' || symbol == 'W' || symbol == 'S');

			if (Player.Show_Inventory().size() > 0)
			{
				std::uniform_int_distribution<int> Wid(0, int(Player.Show_Inventory().size()));
				int i = Wid(gen);
				if (i == Player.Show_Inventory().size())
					Npc.push_back(C_Npc(symbol, Vec2(53, 36), C_Item("Гуляю", 0, 0)));
				else
					Npc.push_back(C_Npc(symbol, Vec2(53, 36), Player.Show_Inventory()[i].first));
			}
			else
			{
				Npc.push_back(C_Npc(symbol, Vec2(54, 36), C_Item("Гуляю", 0, 0)));
			}
			New_Npc = 30.0f;
		}
		New_Npc -= Delta_Time;
	}
}

void C_Game::Delete_Npc()
{
	for (auto it = Npc.begin(); it != Npc.end(); )
	{
		if (it->Get_Delete())
		{
			Npc.erase(it);
			it = Npc.begin();
			continue;
		}
		++it;
	}
}

void C_Game::Save()
{
	std::ofstream fobj;
	fobj.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	try
	{
		fobj.open("Resourse/save.game", std::ios::binary);
	}
	catch (std::ifstream::failure&)
	{
		std::cout << "Save fail";
		exit(-1);
	}
	Player.Save(fobj);
	Research.Save(fobj);
	int ressize = int(Resbench.size());
	fobj.write((char*)(&ressize), sizeof(int));
	for (auto& now : Resbench)
		now.Save(fobj);

	int worksize = int(Workbench.size());
	fobj.write((char*)(&worksize), sizeof(int));
	for (auto& now : Workbench)
		now.Save(fobj);
	int speed = C_Workbench::Speed;
	fobj.write((char*)(&speed), sizeof(int));
	int qual = C_Workbench::Qual;
	fobj.write((char*)(&qual), sizeof(int));
	fobj.close();
}

void C_Game::New_Game()
{
	Npc.clear();
	Bench.clear();
	Resbench.clear();
	Workbench.clear();
	Research.~C_Research();
	Research = C_Research('R', Vec2(7, 4));
	Research.Set_Work(&Workbench, &(Resbench[0].Get_Item()));
	Player.~C_Player();
	Player = C_Player('P', Vec2(10, 10));
	Init_Object();

	C_Workbench::Speed = 0;
	C_Workbench::Qual = 0;
}

const bool  C_Game::Load()
{
	std::ifstream fobj;
	fobj.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		fobj.open("Resourse/save.game", std::ios::binary);
	}
	catch (std::ifstream::failure&)
	{
		return false;
	}
	Npc.clear();
	Bench.clear();
	Resbench.clear();
	Workbench.clear();
	Player.Load(fobj);
	Research.Load(fobj);
	int ressize = 0;
	fobj.read((char*)(&ressize), sizeof(int));
	for (int i = 0; i < ressize; i++)
		Resbench.push_back(C_Resbench(fobj));

	int worksize = 0;
	fobj.read((char*)(&worksize), sizeof(int));
	for (int i = 0; i < worksize; i++)
		Workbench.push_back(C_Workbench(fobj, Resbench[0].Get_Item()));
	fobj.read((char*)(&C_Workbench::Speed), sizeof(int));
	fobj.read((char*)(&C_Workbench::Qual), sizeof(int));
	fobj.close();

	Research.Set_Work(&Workbench, &(Resbench[0].Get_Item()));
	for (auto& now : Resbench)
		Bench.push_back(&now);
	for (auto& now : Workbench)
		Bench.push_back(&now);
	return true;
}
