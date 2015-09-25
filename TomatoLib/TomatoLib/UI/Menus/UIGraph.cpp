#include "UIGraph.h"
#include "../../Utilities/Utilities.h"
#include "../../Async/Async.h"
#include "../../Defines.h"

#include <GLFW\glfw3.h>

namespace TomatoLib {
	UIGraph::UIGraph(UIBase* parent) : UIBase(parent) {
		this->CanClick = false;
		this->SetSize(450, 150);
	}

	void UIGraph::Draw(Render& p) {
		p.Box(0, 0, this->W, this->H, Color(50, 50, 50, 220));
		p.Box(70, 16, 310, this->H - 16, Color(20, 20, 20, 220));
		p.Box(71, 17, 308, this->H - 18, Color(50, 100, 50, 220));

		unsigned int cats = this->CatNames.size();

		for (unsigned int i = 0; i < cats; i++) {
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

		for (unsigned int i = 0; i < cats; i++) {
			this->TempData[i].MS = 0;
		}

		unsigned int linecount = this->Lines.size();
		for (unsigned int line = 0; line < linecount; line++) {
			std::vector<UIGraphLineData>& CatData = this->Lines[line];
			int height = 0;

			unsigned int catcount = CatData.size();
			for (unsigned int cat = 0; cat < catcount; cat++) {
				if (this->CatNames[cat].size() == 0) continue;

				UIGraphLineData& ld = CatData[cat];

				int h = (int)(ld.MS * 8);
				if (height + h > this->H - 18) h = this->H - 18 - height;
				if (h <= 0) continue;

				height += h;
				p.Box(71 + line, this->H - height - 1, 1, h, this->CatColors[ld.Category]);
			}
		}

		for (int i = 0; i < 17; i++) {
			p.Box(71, 20 + 8 * i, 308, 1, i == 0 ? Color(255, 0, 0, 200) : i == 8 ? Color(255, 153, 0, 200) : Color(50, 50, 50, 220));
		}
	}

	void UIGraph::StartWatch(int cat) {
		this->m_RecordingCat = cat;
		this->m_RecordingTime = TL_GET_TIME_MS;
	}

	void UIGraph::StopWatch() {
		this->InsertData(this->m_RecordingCat, TL_GET_TIME_MS - this->m_RecordingTime);
	}

	void UIGraph::InsertData(int cat, float ms) {
		this->TempData[cat].MS += ms;
	}

	void UIGraph::SetupCategory(int cat, const std::string& name, const Color& col) {
		while ((int)this->CatNames.size() <= cat) {
			UIGraphLineData empty;
			empty.Category = (int)this->CatColors.size();
			empty.MS = 0;

			this->TempData.push_back(empty);
			this->CatNames.push_back("");
			this->CatColors.push_back(Color::Black);
		}

		this->CatNames[cat] = name;
		this->CatColors[cat] = col;
	}

	void UIGraph::Update() {
		this->OnTop();
	}

	void UIGraph::InvalidateLayout() {

	}

}