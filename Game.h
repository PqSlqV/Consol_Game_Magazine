#pragma once
#pragma once
#include<Windows.h>
#include<locale.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<random>
#include<chrono>
#include<fstream>


struct Vec2 {
	int x, y;
	bool operator==(const Vec2& temp)const
	{
		return x == temp.x && y == temp.y;
	}
	Vec2 operator-(const int& temp)const
	{
		return Vec2(x - temp, y - temp);
	}
	Vec2 operator+(const int& temp)const
	{
		return Vec2(x + temp, y + temp);
	}
};


class C_Item {
protected:
	std::string Name;
	int Rare;
	int Price;
public:
	C_Item() : Rare(0), Price(0) {}
	C_Item(const std::string& name, const int& rare, const int& price) : Name(name), Rare(rare), Price(price) { if (Name.size() > 20) Name.resize(20); }
	const std::string& Get_Name() const;
	const int& Get_Rare() const;
	const int& Get_Price() const;
	void Set_Rare(const int& rare);
	virtual std::string Show() const;
	const bool operator==(const C_Item& temp) const;
};

class C_Map {
	std::vector<std::string> Map;
	std::string Path;
public:
	C_Map(const std::string& path);
	void Init();
	void Show(const std::vector<std::pair<C_Item, int>>& Inventory) const;
	void Put_On_Map(const char& symbol, const Vec2& BeginPos, const Vec2& EndPos);
	void Put_On_Map(const char& symbol, const Vec2& BeginPos, const Vec2& EndPos,const int& Time);
	bool Collision(const Vec2& BeginPos, const Vec2& EndPos) const;

};

class C_WorkItem : public C_Item {
	int Time = 0;
	std::vector<std::pair<C_Item, int>> Resource;
public:
	C_WorkItem(const std::string& name, const int& rare, const int& price) : C_Item(name, rare, price) {}
	C_WorkItem(const std::string& name, const int& rare, const int& price, const int& time) : C_Item(name, rare, price), Time(time) {}
	C_WorkItem(const std::string& name, const int& rare, const C_Item& resource, const int& count);
	C_WorkItem(const std::string& name, const int& rare, const std::vector<std::pair<C_Item, int>>& resource);
	std::string Show() const override;
	const int& Get_Time() const;
	const std::vector<std::pair<C_Item, int>>& Get_Resource() const;
};

class C_Player;

class C_Mesh {
protected:
	char Symbol;
	Vec2 BeginPosition;
	Vec2 EndPosition;
public:
	C_Mesh(const char& symbol, const Vec2& BeginPos, const Vec2& EndPos) :Symbol(symbol), BeginPosition(BeginPos), EndPosition(EndPos) {}
	C_Mesh(std::ifstream& fobj);
	virtual void Show(C_Map& Map) const;
protected:
	bool Mesh_Collision(const C_Mesh& first, const C_Mesh& second) const;
	virtual void Save(std::ofstream& fobj);
};

class C_Bench : public C_Mesh {
protected:
	bool In_Work = false;
	bool Finish_Work = false;
	bool First = true;
	float Work_Time = 0.0f;
	Vec2 TimePosition;
public:
	C_Bench(const char& symbol, const Vec2& pos) : C_Mesh(symbol, Vec2(pos.x - 5, pos.y - 2), Vec2(pos.x + 5, pos.y + 2)), TimePosition(Vec2(pos.x - 3, pos.y + 1)) {}
	C_Bench(std::ifstream& fobj);
	void Work();
	void Show_Work(C_Player& Player);
	void Show(C_Map& Map) const override;
	virtual void Save(std::ofstream& fobj);
protected:
	void Show_InWork();
	virtual void Set_Work(C_Player& Player) = 0;
	virtual void Show_FinishWork(C_Player& Player) = 0;
};
class C_Workbench;
class C_Resbench;

class C_Research : public C_Mesh {
	struct C_ResItem
	{
		std::string Name;
		int lvl = 0;
		int Maxlvl = 0;
		int Price = 0;
		int Action = 0;
		bool operator==(const C_ResItem& temp)const
		{
			return Name == temp.Name && lvl == temp.lvl && Maxlvl == temp.Maxlvl && Price == temp.Price && Action == temp.Action;
		}
	};
	std::vector<C_ResItem> Item;
	std::vector <C_Workbench>* Workbench = nullptr;
	const std::vector<C_Item>* Resbench = nullptr;
	bool Act = false;
public:

	C_Research(const char& symbol, const Vec2& pos);

	void Save(std::ofstream& fobj);
	void Load(std::ifstream& fobj);
	void Dialog(C_Player& Player);
	void Set_Work(std::vector <C_Workbench>* work, const std::vector<C_Item>* res);
	const bool Get_Act();
	void Set_Act();
	~C_Research() {}
private:
	void Action(const int& Act);
};


class C_Resbench : public C_Bench {
	std::vector<C_Item> Item;
	std::vector<std::pair<C_Item*, int>> AcceptItem;
public:
	C_Resbench(const char& symbol, const Vec2& pos);
	C_Resbench(std::ifstream& fobj);
	void Save(std::ofstream& fobj) override;
	const std::vector<C_Item>& Get_Item() const;
private:
	void Set_Work(C_Player& Player) override;
	void Show_FinishWork(C_Player& Player) override;
	void Add_Item();
};

class C_Workbench : public C_Bench {
	std::vector<C_WorkItem> Item;
	int Accept_Object = 0;
	int Maxobj = 1;
public:
	C_Workbench(const char& symbol, const Vec2& pos, const std::vector<C_Item>& Res);
	C_Workbench(std::ifstream& fobj, const std::vector<C_Item>& Res);

	const std::vector<C_WorkItem>& Get_Work_Item() const;
	void Save(std::ofstream& fobj) override;
	static int Speed;
	static int Qual;

private:
	void Set_Work(C_Player& Player) override;
	void Show_FinishWork(C_Player& Player) override;
	void Add_Item(const std::vector<C_Item>& Res);
};

class C_Npc : public C_Mesh {
	Vec2 NextPos;
	float Move_Time = 0.0f;
	bool Delete = false;
	float Live = 300.0f; // в секундах
	C_Item Wish;
public:
	C_Npc(const char& symbol, const Vec2& Pos, const C_Item& wish) : C_Mesh(symbol, Pos - 1, Pos + 1), NextPos(Pos), Wish(wish) {}
	void Moving(const C_Map& Map, const C_Player& Player, const std::vector<C_Npc>& Npc);
	void Set_Delete();
	bool Get_Delete();
	bool Get_Life();
	const C_Item& Get_Wish()const;
private:
	void Move();
	bool Collision(const C_Map& Map, const C_Player& Player, const std::vector<C_Npc>& Npc) const;
};

class C_Player : public C_Mesh {
	int Money = 50;
	float Move_Time = 0.0f;
	std::vector<std::pair<C_Item, int>> Inventory;
	std::vector<std::pair<std::string, int>> Resourse;
public:
	C_Player(const char& symbol, const Vec2& Pos) : C_Mesh(symbol, Pos - 1, Pos + 1) {}
	~C_Player() {}
	void Moving(const C_Map& Map, std::vector<C_Npc>& Npc, std::vector <C_Bench*>& Workbench, C_Research& Research);
	const std::vector<std::pair<C_Item, int>>& Show_Inventory() const;
	const std::vector<std::pair<std::string, int>>& Show_Resourse() const;
	std::vector<std::pair<C_Item, int>>& Get_Inventory();
	std::vector<std::pair<std::string, int>>& Get_Resourse();
	const bool Set_Money(const int& money);
	const int& Get_Money() const;
	void Save(std::ofstream& fobj);
	void Load(std::ifstream& fobj);
private:
	bool Move();
	bool Collision(const C_Map& Map, std::vector<C_Npc>& Npc, std::vector <C_Bench*>& Workbench, C_Research& Research);
	bool Dialog_Buy(std::vector<C_Npc>::iterator& it);
};


class C_Game {
	C_Map Map;
	C_Player Player;
	std::vector<C_Npc> Npc;
	float New_Npc = 30.0f;
	C_Research Research;
	std::vector <C_Resbench> Resbench;
	std::vector <C_Workbench> Workbench;
	std::vector <C_Bench*> Bench;
public:
	C_Game(const int& width, const int& height);
	void Show(const float& delta);
	void Save();
	void New_Game();
	const bool Load();
private:
	void Init_Object();
	void Logic();
	void Print();
	void Add_Npc();
	void Delete_Npc();
};