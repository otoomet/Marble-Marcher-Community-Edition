#include "Gamemodes.h"

void OpenMainMenu(Scene * scene, Overlays * overlays)
{
	RemoveAllObjects();
	game_mode = MAIN_MENU;
	scene->SetExposure(1.0f);
}

void OpenEditor(Scene * scene, Overlays * overlays, int level)
{
	RemoveAllObjects();
	//go to level editor
	game_mode = LEVEL_EDITOR;
	menu_music.stop();
	scene->SetExposure(1.0f);
	overlays->TWBAR_ENABLED = true;
	TwDefine("LevelEditor visible=true position='20 20'");
	TwDefine("FractalEditor visible=true position='20 500'");
	TwDefine("Settings iconified=true");
	TwDefine("Statistics iconified=true");
	scene->StartLevelEditor(level);
}

void PlayLevel(Scene * scene, sf::RenderWindow * window, int level)
{
	RemoveAllObjects();
	//play level
	game_mode = PLAYING;
	menu_music.stop();
	scene->SetExposure(1.0f);
	scene->levels.GetLevelMusic(level)->setVolume(GetVol());
	scene->levels.GetLevelMusic(level)->play();
	scene->StartSingle(level);
	LockMouse(*window);
}

void OpenTestWindow(sf::Font & font)
{
	Window test(200, 200, 500, 500, sf::Color(0, 0, 0, 128), LOCAL["Window"], font);
	Text button(LOCAL["Button"], font, 30, sf::Color::White);
	Box sbox(0, 0, 420, 200, sf::Color(128, 128, 128, 240));
	Box sbox2(0, 0, 240, 40, sf::Color(0, 64, 128, 240));
	Box sbox3(0, 0, 30, 30, sf::Color(0, 64, 128, 240));

	sbox2.hoverstate.color_main = sf::Color(230, 40, 20, 200);
	sbox2.AddObject(&button, Box::CENTER);
	button.hoverstate.font_size = 40;
	test.Add(&sbox, Box::CENTER);
	test.Add(&sbox2, Box::CENTER);
	test.Add(&sbox, Box::CENTER);
	test.Add(&sbox2, Box::CENTER);
	test.Add(&sbox, Box::CENTER);
	test.Add(&sbox2, Box::CENTER);
	
	AddGlobalObject(test);
}

void OpenLevelMenu(Scene* scene, Overlays* overlays)
{
	sf::Vector2f wsize = default_view.getSize();
	MenuBox levels(wsize.x*0.85, wsize.y*0.95, wsize.x*0.075, wsize.y*0.025);
	game_mode = LEVELS;
	std::vector<std::string> names = scene->levels.getLevelNames();
	std::vector<std::string> desc = scene->levels.getLevelDesc();
	std::vector<int> ids = scene->levels.getLevelIds();

	Box Bk2Menu(100, 40);
	Text button(LOCAL["Back2Main"], LOCAL("default"), 35, sf::Color::White);
	Bk2Menu.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		OpenMainMenu(scene, overlays);
	});
	Bk2Menu.AddObject(&button, Object::Allign::CENTER);
	levels.AddObject(&Bk2Menu, Object::Allign::LEFT);
	
	sf::Image edit; edit.loadFromFile(edit_png);
	sf::Texture edittxt; edittxt.loadFromImage(edit);
	edittxt.setSmooth(true);

	sf::Image remove; edit.loadFromFile(delete_png);
	sf::Texture removetxt; removetxt.loadFromImage(remove);
	removetxt.setSmooth(true);

	for (int i = 0; i < scene->levels.GetLevelNum(); i++)
	{
		Box lvlbtton(wsize.x*0.8, 60);
		Text lvlname(utf8_to_wstring(names[i]), LOCAL("default"), 35, sf::Color::White);
		lvlname.SetCallbackFunction([scene, selected = ids[i]](sf::RenderWindow * window, InputState & state)
		{
			PlayLevel(scene, window, selected);
		});
		lvlbtton.AddObject(&lvlname, Object::Allign::LEFT);
		
		Box buttons(100, 50);

		Box bedit(100, 50);
		bedit.hoverstate.color_main = sf::Color(0, 255, 0, 255);
		bedit.SetBackground(edittxt);
		Box bremove(100, 50);
		bremove.hoverstate.color_main = sf::Color(255, 0, 0, 255);
		bremove.SetBackground(removetxt);

		bremove.SetCallbackFunction([scene, overlays, id = ids[i]](sf::RenderWindow * window, InputState & state)
		{
			ConfirmLevelDeletion(id, scene);
		});

		buttons.AddObject(&bremove, Object::Allign::RIGHT);
		buttons.AddObject(&bedit, Object::Allign::RIGHT);

		lvlbtton.AddObject(&buttons, Object::Allign::RIGHT);

		Text lvldescr(utf8_to_wstring(desc[i]), LOCAL("default"), 35, sf::Color::White);

		levels.AddObject(&lvlbtton, Object::Allign::LEFT);
	}

	Box Newlvl(100, 40);
	Text newlvl(LOCAL["CreateNewLvl"], LOCAL("default"), 35, sf::Color::White);
	Newlvl.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		OpenEditor(scene, overlays, -1);
	});
	Newlvl.AddObject(&newlvl, Object::Allign::CENTER);
	levels.AddObject(&Newlvl, Object::Allign::LEFT);

}

void ConfirmLevelDeletion(int lvl, Scene* scene)
{
	Window confirm(200, 200, 500, 300, sf::Color(0, 0, 0, 128), LOCAL["You_sure"], LOCAL("default"));
	Text button1(LOCAL["Yes"], LOCAL("default"), 30, sf::Color::White);
	Text button2(LOCAL["No"], LOCAL("default"), 30, sf::Color::White);
	Text text(LOCAL["You_sure"], LOCAL("default"), 30, sf::Color::White);
	 
	Box but1(0, 0, 240, 40, sf::Color(0, 64, 128, 240));
	Box but2(0, 0, 30, 30, sf::Color(0, 64, 128, 240));

	but1.hoverstate.color_main = sf::Color(230, 40, 20, 200);
	but2.hoverstate.color_main = sf::Color(230, 40, 20, 200);
	but1.AddObject(&button1, Box::CENTER);
	but2.AddObject(&button2, Box::CENTER);

	confirm.Add(&text, Box::CENTER);
	confirm.Add(&but1, Box::RIGHT);
	confirm.Add(&but2, Box::RIGHT);

	int id = AddGlobalObject(confirm);

	get_glob_obj(id).objects[1].get()->objects[0].get()->objects[1].get()->SetCallbackFunction([scene, id](sf::RenderWindow * window, InputState & state)
	{
		//remove lvl
		Add2DeleteQueue(id);
	});

	get_glob_obj(id).objects[1].get()->objects[0].get()->objects[2].get()->SetCallbackFunction([scene, id](sf::RenderWindow * window, InputState & state)
	{
		
		Add2DeleteQueue(id);
	});
}