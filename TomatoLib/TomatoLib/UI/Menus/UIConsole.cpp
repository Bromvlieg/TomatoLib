#include "UIConsole.h"

#include <math.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <GLFW\glfw3.h>

namespace TomatoLib {
	// trim from start
	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}

	// fuzzy search
	static inline bool fuzzysearch(std::string &needle, std::string haystack) {
		int foundchar = -1;
		for (unsigned int i = 0; i < needle.size(); i++) {
			bool found = false;
			for (unsigned int i2 = foundchar + 1; i2 < haystack.size(); i2++) {
				if (needle[i] == haystack[i2]) {
					foundchar = i2;
					found = true;
					break;
				}
			}

			if (!found) return false;
		}

		return true;
	}


	UIConsole::UIConsole(UIBase* parent) : UIBase(parent) {
		this->InputHistoryIndex = 0;
		this->MaxHeight = 600;
		this->Font = Render::DefaultFont;

		this->MainPanel = new UIPanel(this);
		this->MainPanel->SetPos(0, 0);
		this->MainPanel->SetSize(this->W, 200);
		this->MainPanel->OnDraw = [this](Render& drawer) {
			drawer.Box(0, 0, this->MainPanel->W, this->MainPanel->H, Color(0, 0, 0, 150));
			drawer.Box(1, 1, this->MainPanel->W - 2, this->MainPanel->H - 2, Color(60, 60, 60, 150));
		};

		this->ScrollPanel = new UIScrollPanel(this, false);
		this->ScrollPanel->SetPos(0, 0);
		this->ScrollPanel->SetSize(this->W, 180);
		this->ScrollPanel->Back->OnDraw = [this](Render& p) {
			size_t numchars = this->TextEntry->GetText().size();
			if (this->PrevCharCount != numchars) {
				this->PrevCharCount = numchars;

				this->AutoComplete.Clear();

				if (numchars > 0) {
					std::string cmd = this->TextEntry->GetText();
					std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

					for (int i = 0; i < this->RegisteredFunctions.Count; i++) {
						std::string fname = this->RegisteredFunctions.Keys[i];
						if (fuzzysearch(cmd, fname)) {
							this->AutoComplete.Add(fname);
						}
					}

					for (int i = 0; i < this->RegisteredVariables.Count; i++) {
						std::string vname = this->RegisteredVariables.Keys[i];
						if (fuzzysearch(cmd, vname)) {
							this->AutoComplete.Add(vname);
						}
					}
				}
			}

			if (this->AutoComplete.Count == 0) return;

			float maxw = 0;
			for (int i = 0; i < this->AutoComplete.Count; i++) {
				Vector2 tsize = p.GetTextSize(this->AutoComplete[i]);
				if (tsize.X > maxw) maxw = tsize.X;
			}

			float xpos = this->ScrollPanel->W - maxw - 2.0f - (this->ScrollPanel->GrabY->ShouldRender ? this->ScrollPanel->GrabY->W : 0);
			float ypos = this->ScrollPanel->H - this->AutoComplete.Count * this->Font->FontHandle->height - 2.0f - this->ScrollPanel->Back->Y;

			p.Box(xpos, ypos, maxw + 2, this->AutoComplete.Count * this->Font->FontHandle->height + 2.0f, Color(0, 0, 0, 150));
			p.Box(xpos + 1, ypos + 1, maxw, this->AutoComplete.Count * this->Font->FontHandle->height, Color(60, 60, 60, 150));
			for (int i = 0; i < this->AutoComplete.Count; i++) {
				std::string str = this->AutoComplete[i];
				float curx = 0;
				bool* bools = new bool[str.size()];
				memset(bools, 0, str.size());

				int foundchar = -1;
				std::string cmd = this->TextEntry->GetText();
				std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

				for (unsigned int i3 = 0; i3 < cmd.size(); i3++) {
					for (unsigned int i2 = foundchar + 1; i2 < str.size(); i2++) {
						if (cmd[i3] == str[i2]) {
							foundchar = i2;
							bools[i2] = true;
							break;
						}
					}
				}

				for (unsigned int i2 = 0; i2 < str.size(); i2++) {
					p.Text(this->Font, std::string() + str[i2], xpos + 1 + curx, ypos + i * this->Font->FontHandle->height + 1, bools[i2] ? Color::Green : Color::White);
					curx += p.GetTextSize(str[i2]).X;
				}

				delete[] bools;
			}
		};
		this->ScrollPanel->AlwaysShowBar = true;

		this->TextEntry = new UITextEntry(this);
		this->TextEntry->SetPos(0, 180);
		this->TextEntry->SetSize(this->W, 20);

		std::function<void(int, int)> oldinput = this->TextEntry->OnInput;
		this->TextEntry->OnInput = [this, oldinput](int key, int mods) {
			if (key == GLFW_KEY_UP) {
				if (this->InputHistory.Count == 0) return;
				this->InputHistoryIndex -= this->InputHistoryIndex == 0 ? 0 : 1;
				this->TextEntry->SetText(this->InputHistory[this->InputHistoryIndex]);
				return;
			} else if (key == GLFW_KEY_DOWN) {
				if (this->InputHistory.Count == 0) return;
				this->InputHistoryIndex += this->InputHistoryIndex == this->InputHistory.Count ? 0 : 1;
				if (this->InputHistoryIndex >= this->InputHistory.Count) {
					this->TextEntry->SetText("");
				} else {
					this->TextEntry->SetText(this->InputHistory[this->InputHistoryIndex]);
				}
				return;
			} else if (key == GLFW_KEY_TAB) {
				if (this->AutoComplete.Count != 1) return;
				this->TextEntry->SetText(this->AutoComplete[0] + " ");
				return;
			}

			oldinput(key, mods);
		};

		this->TextEntry->OnEnter = [this]() {
			this->HandleInput(this->TextEntry->GetText());
			this->TextEntry->Clear();
		};

		this->OnFocus = [this](bool focus) {
			if (focus) this->TextEntry->SetFocus();
		};

		this->ConsolePanel = new UIPanel(0);
		this->ConsolePanel->OnDraw = [this](Render& p) {
			float currentY = 0;

			this->PrintLock.lock();
			unsigned int linecount = this->Lines.size();
			for (unsigned int line = 0; line < linecount; line++) {
				if (floor(this->ScrollPanel->Back->Y / -this->Font->FontHandle->height) < (line + 1) && (floor(this->ScrollPanel->Back->Y / -this->Font->FontHandle->height + this->ScrollPanel->Back->H)) > (line)) {
					float currentX = 0;
					//int maxy = 0;

					UIConsoleLineData& ld = this->Lines[line];

					unsigned int strcount = ld.Strings.size();
					for (unsigned int i = 0; i < strcount; i++) {
						p.Text(this->Font, ld.Strings[i], currentX, currentY, ld.Colors[i]);

						currentX += p.GetTextSize(ld.Strings[i]).X;
					}
				}

				currentY += this->Font->FontHandle->height;
			}

			this->PrintLock.unlock();
		};
		this->ScrollPanel->AddItem(this->ConsolePanel);

		this->RegisterConsoleFunc("help", [this](std::string args) {
			for (int i = 0; i < this->RegisteredFunctions.Count; i++) {
				this->Print(std::string("Function: ") + this->RegisteredFunctions.Keys[i].c_str());
			}

			for (int i = 0; i < this->RegisteredVariables.Count; i++) {
				this->Print(std::string("Var: ") + this->RegisteredVariables.Keys[i].c_str() + " = " + this->RegisteredVariables.Values[i].c_str());
			}

			return "Done printing";
		});

		this->OnScroll = [this](int x, int y) { this->ScrollPanel->Scroll(y); };
		this->ConsolePanel->OnScroll = [this](int x, int y) { this->ScrollPanel->Scroll(y); };
		this->TextEntry->OnScroll = [this](int x, int y) { this->ScrollPanel->Scroll(y); };
		this->MainPanel->OnScroll = [this](int x, int y) { this->ScrollPanel->Scroll(y); };

		this->OnShow = [this]() {

		};

		this->RegisterConsoleVar("console_lines", "15");
	}

	void UIConsole::Draw(Render& p) {
	}

	void UIConsole::Update() {
	}

	void UIConsole::HandleInput(std::string line) {
		this->Print(">>" + line, false);
		for (int i = 0; i < this->InputHistory.Count; i++) {
			if (this->InputHistory[i] == line) {
				this->InputHistory.RemoveAt(i);
			}
		}

		this->InputHistory.Add(line);
		this->InputHistoryIndex = this->InputHistory.Count;
		if (this->InputHistory.Count > 100) {
			this->InputHistory.RemoveAt(0);
			this->InputHistoryIndex--;
		}

		size_t command_end = line.find(" ");
		if (command_end == std::string::npos) command_end = line.size();

		std::string cmd = line.substr(0, command_end);
		std::string args = line.substr(command_end);
		args = trim(args);

		std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

		std::string output;

		if (this->RegisteredFunctions.ContainsKey(cmd)) {
			output = this->RegisteredFunctions[cmd](args);
			if (output.size() > 0) {
				this->Print("<< " + output);
			}
		} else if (this->RegisteredVariables.ContainsKey(cmd)) {
			if (args.size() > 0) {
				this->RegisteredVariables[cmd] = args;
			} else {
				this->Print(std::string("<< ") + cmd + " = " + this->RegisteredVariables[cmd]);
			}
		} else {
			this->Print("<<[col=255,0,0] Unknown command");
		}
	}

	void UIConsole::InvalidateLayout() {
		int h = (int)(atoi(this->GetConsoleVar("console_lines").c_str()) * this->Font->FontHandle->height);

		int maxh = this->MaxHeight - 20;
		if (h > maxh) h = maxh;

		this->PrintLock.lock();
		this->MainPanel->SetSize(this->W, h + 20);
		this->ScrollPanel->SetSize(this->W, h);
		this->ConsolePanel->SetSize(this->ScrollPanel->W, (int)(this->Lines.size() * this->Font->FontHandle->height));
		this->ScrollPanel->InvalidateLayout();
		this->TextEntry->SetPos(0, h);
		this->TextEntry->SetSize(this->W, 20);

		this->PrintLock.unlock();
	}

	std::string UIConsole::GetConsoleVar(std::string name) {
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		return this->RegisteredVariables.ContainsKey(name) ? this->RegisteredVariables[name] : "";
	}


	void UIConsole::RegisterConsoleVar(std::string name, std::string defaultvalue) {
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		this->RegisteredVariables[name] = defaultvalue;
	}

	void UIConsole::RegisterConsoleFunc(std::string name, std::function<std::string(std::string)> func) {
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		this->RegisteredFunctions[name] = func;
	}

	void UIConsole::Print(std::string line, bool enablecolors) {
		UIConsoleLineData ld;
		ld.Colors.push_back(Color::White); // set default color to white

		std::size_t lastpos = 0;
		if (enablecolors) {
			std::size_t newpos = 0;

			while ((newpos = line.find("[col=", lastpos)) != std::string::npos) {
				std::string strpart = line.substr(lastpos, newpos - lastpos);

				std::size_t endpos = line.find("]", newpos);
				if (endpos != std::string::npos) {
					Color col;

					// cut out the color part of the string
					std::string colpart = line.substr(newpos + 5, endpos - 5 - newpos); // "r,g,b"
					std::size_t col_g_start = colpart.find(",", 0);
					std::size_t col_b_start = colpart.find(",", col_g_start + 1);

					// + and - are to strip ',' out
					std::string col_r = colpart.substr(0, col_g_start);
					std::string col_g = colpart.substr(col_g_start + 1, col_b_start - col_g_start - 1);
					std::string col_b = colpart.substr(col_b_start + 1);

					// lets convert em to numbers.
					col.R = atoi(col_r.c_str());
					col.G = atoi(col_g.c_str());
					col.B = atoi(col_b.c_str());

					ld.Strings.push_back(strpart);
					ld.Colors.push_back(col);

					// set current find offset to end of our current color batch, to prevent infinite loops
					lastpos = endpos + 1;
				} else {
					lastpos = newpos + 1;
				}
			}
		}

		ld.Strings.push_back(line.substr(lastpos));

		this->PrintLock.lock();
		this->Lines.push_back(ld);

		if (this->Lines.size() > 150) {
			this->Lines.erase(this->Lines.begin());
		}
		this->PrintLock.unlock();

		this->InvalidateLayout();
		this->ScrollPanel->Scroll((int)-this->Font->FontHandle->height);
	}
}
