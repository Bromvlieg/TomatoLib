#pragma once
#ifndef __UIBASE_H__
#define __UIBASE_H__

#include <functional>
#include "../Graphics/Render.h"
#include "../Utilities/List.h"

namespace TomatoLib {
	class UIManager;

	class UIBase {
		inline void _GetAbsoluteLocation(Vector2& pos);
		bool _ProtectedRemoveFlag;
		bool _ProtectedScopeFlag;

	public:
		static UIManager* DefaultUImanager;

		UIManager* UIMan;
		int X, Y, W, H, Dock;
		bool ShouldRender, CanClick;
		Color DrawColor;
		int TabIndex;
		bool CanAcceptInput;
		bool TopMost;
		bool PassTrough;
		bool PassTroughSelfOnly;
		bool CanCatchScroll;
		bool Frozen;
		bool ChildsDrawFirst;

		bool ShouldRedraw;
		bool AlwaysRedraw;

		UIBase* Parent;
		std::vector<UIBase*> Children;
		UIBase(UIBase* parent);
		virtual ~UIBase();

		std::function<void(int, int)> OnScroll;
		std::function<void(int, int, int)> OnClick;
		std::function<void(int, int, int)> OnPress;
		std::function<void(int, int, int)> OnRelease;
		std::function<void(bool)> OnFocus;
		std::function<void(int)> OnKeyDown;
		std::function<void(int)> OnKeyUp;
		std::function<void(int, int)> OnInput;
		std::function<void(int)> OnCharInput;
		std::function<void(Render&)> OnDraw;
		std::function<void()> OnUpdate;
		std::function<void()> OnInvalidateLayout;
		std::function<void()> OnKill;
		std::function<void()> OnShow;
		std::function<void()> OnHide;

		void _InternalDraw(Render& drawer);
		void _InternalUpdate();
		virtual void Draw(Render& drawer) = 0;
		virtual void Update() = 0;
		virtual void InvalidateLayout() = 0;
		void InvalidateLayoutWithChilds();

		Vector2 GetAbsoluteLocation();
		UIBase* GetAbsoluteParent();
		RenderBuffer Buffer;

		void SetParent(UIBase* parent);
		UIBase* GetParent();

		void AddChild(UIBase* child);
		void RemoveChild(UIBase* child);

		void SetPos(int x, int y);
		void GetPos(int& x, int& y);
		Vector2 GetPos();

		void SetSize(int w, int h);
		void GetSize(int& w, int& h);
		Vector2 GetSize();

		void SetWidth(int w);
		int GetWidth();

		bool IsHovering();

		void SetHeight(int h);
		int GetHeight();

		void Kill();
		void OnTop();

		void GenerateTabIndexes();
		void SetFocus();
		void UnFocus();
		bool HasFocus();
		void Show();
		void Hide();
		void MarkForFullRedraw();
		void ForceTop();
	};
}

#endif