#include "UIGraph.h"
#include "../../Utilities/Utilities.h"
#include "../../Async/Async.h"

namespace TomatoLib {
	UIGraph::UIGraph(UIBase* parent) : UIBase(parent) {
		this->CatColors.push_back(Color(255, 0, 0));
		this->CatColors.push_back(Color(0, 0, 255));
		this->CatColors.push_back(Color(128, 0, 128));
		this->CatColors.push_back(Color(0, 255, 0));
		this->CatColors.push_back(Color(0, 255, 255));
		this->CatColors.push_back(Color(0, 0, 255));
		this->CatColors.push_back(Color(255, 0, 255));
		this->CatColors.push_back(Color(255, 255, 255));

		for (int i = 0; i < 8; i++) {
			this->CatNames.push_back("");
		}

		for (int i = 0; i < 8; i++) {
			UIGraphLineData empty;
			empty.Category = i;
			empty.MS = 0;
			this->TempData.push_back(empty);
		}

		this->CanClick = false;
	}

	void UIGraph::Draw(Render& p) {

		p.Box(0, 0, this->W, this->H, Color(50, 50, 50, 220));
		p.Box(70, 16, 310, this->H - 16, Color(20, 20, 20, 220));
		p.Box(71, 17, 308, this->H - 18, Color(50, 100, 50, 220));

		for (int i = 0; i < 17; i++) {
			p.Box(71, 20 + 8 * i, 308, 1, i == 0 ? Color(255, 0, 0, 200) : i == 8 ? Color(255, 153, 0, 200) : Color(50, 50, 50, 220));
		}

		for (int i = 0; i < 8; i++) {
			if (this->CatNames[i].size() == 0) continue;
			p.Text(this->CatNames[i], 5, 20 + i * 16, this->CatColors[i]);
		}

		p.Text(std::string("Main calls: ") + Utilities::GetNumberPadded((int)Async::CallsToDoOnMainThread.size(), 4, '0'), 75, -2, Color(150, 150, 150, 255));
		p.Text(std::string("Async calls: ") + Utilities::GetNumberPadded((int)Async::CallsToDoOnAsyncThread.size() - (int)Async::CallsToDoOnAsyncThreadIndex, 4, '0'), 240, -2, Color(150, 150, 150, 255));

		p.Text(">15 ms", this->W - 60, 20, Color::White);
		p.Text("8 ms", this->W - 60, 75, Color::White);
		p.Text("<1 ms", this->W - 60, 130, Color::White);

		if (this->Lines.size() >= 308) {
			this->Lines.erase(this->Lines.begin());
		}
		this->Lines.push_back(this->TempData);

		for (int i = 0; i < 8; i++) {
			this->TempData[i].MS = 0;
		}

		unsigned int linecount = this->Lines.size();
		for (unsigned int line = 0; line < linecount; line++) {

			std::vector<UIGraphLineData>& CatData = this->Lines[line];
			int height = 0;

			unsigned int catcount = CatData.size();
			for (unsigned int cat = 0; cat < catcount; cat++) {
				if (this->CatNames[cat].size() == 0) continue;

				UIGraphLineData& LineData = CatData[cat];
				int h = (int)(LineData.MS * 8);
				if (height + h > this->H - 18) h = this->H - 18 - height;
				if (h <= 0) continue;

				height += h;
				p.Box(71 + line, this->H - height - 1, 1, h, this->CatColors[LineData.Category]);
			}
		}
	}

	void UIGraph::InsertData(int cat, float ms) {
		this->TempData[cat].MS += ms;
	}

	void UIGraph::SetupCategory(int cat, std::string name) {
		this->CatNames[cat] = name;
	}

	void UIGraph::Update() {
		this->OnTop();
	}

	void UIGraph::InvalidateLayout() {

	}

}