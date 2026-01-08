// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	HOOK Ivk_Print_Union PATCH(&zCView::Print, &zCView::Print_Union);
	void zCView::Print_Union(int x, int y, const zSTRING& text) {
		THISCALL(Ivk_Print_Union)(x, y, text);
		focus.TryToPrint(this, x, y, text);
	}

	zSTRING Focus::ValueString = Z("");

	static inline bool PlayerIsInInfo() {
		return !oCInformationManager::GetInformationManager().HasFinished();
	}

	static inline zSTRING GetTextFromInstanceName(const zSTRING& s) {
		zSTRING val = "";
		auto idx = parser->GetIndex(s);
		if (idx < 0) return val;

		oCItem* item = new oCItem();
		item->InitByScript(idx, 1);

		for (int i = 0; i < ITM_TEXT_MAX; i++) {
			if (item->text[i].Length() && item->text[i] != Focus::ValueString)
				val += item->text[i] + " ";
		}
		//val.TrimRight(' ');

		item->Release();
		item = nullptr;

		return val;
	}

	static inline zSTRING GetLockableInstAndName(const oCMobLockable* lockable)
	{
		return Z(GetTextFromInstanceName(lockable->keyInstance) + "(" + lockable->keyInstance + ")");
	}

	static inline zSTRING GetValueString(const zSTRING& s) {
		zCPar_Symbol* sym = parser->GetSymbol(s);

		if (!sym) return Z "";

		zSTRING val;
		sym->GetValue(val, 0);

		return val;
	}

	PrintedTextCords Focus::CalculatePrintPos(int x, int y, zSTRING& focusedStr, zSTRING& newStr)
	{
		PrintedTextCords pos{};
		int width = _focusView->FontSize(newStr);
		pos.y = y - _fontSize - _margin;
		pos.x = x + _focusView->FontSize(focusedStr) / 2 - width / 2;

		return pos;
	}

	void Focus::TryToPrint(zCView* view, int x, int y, const zSTRING& text) {
		if (!player || PlayerIsInInfo() || view == focus._focusView || _vobOnScreen)
			return;

		zCVob* vob = player->GetFocusVob();
		if (!vob) return;

		oCMobLockable* lockable = vob->CastTo<oCMobLockable>();
		if (!lockable) return;

		zSTRING name = lockable->GetName();
		if (text != name + "\n" && text != name + " (locked)\n")
			return;

		PrintLockableStr(view, x, y, lockable);
	}

	void Focus::PrintLockableStr(zCView* view, int x, int y, oCMobLockable* lockable)
	{
		if (_focusView == nullptr) {
			EnsureView();
		}
		else {
			_focusView->ClrPrintwin();
		}
		_vobOnScreen = true;

		if (!lockable->locked)
			return;

		PrintLockInfo(x, y, lockable);
	}

	void Focus::PrintLockInfo(int x, int y, oCMobLockable* lockable)
	{
		if (lockable->keyInstance.Length() && lockable->pickLockStr.Length()) {
			// lockStr
			auto cords = CalculatePrintPos(x, y, lockable->GetName(), lockable->pickLockStr);
			_focusView->Print(cords.x, cords.y, lockable->pickLockStr);

			// key instance
			auto keyStr = GetLockableInstAndName(lockable);
			auto cordsForKey = CalculatePrintPos(cords.x, cords.y, lockable->pickLockStr, keyStr);
			_focusView->Print(cordsForKey.x, cordsForKey.y, keyStr);
		}
		else if (lockable->keyInstance.Length()) {
			auto keyStr = GetLockableInstAndName(lockable);
			auto cords = CalculatePrintPos(x, y, lockable->GetName(), keyStr);
			_focusView->Print(cords.x, cords.y, keyStr);
		}
		else if (lockable->pickLockStr.Length()) {
			auto cords = CalculatePrintPos(x, y, lockable->GetName(), lockable->pickLockStr);
			_focusView->Print(cords.x, cords.y, lockable->pickLockStr);
		}
	}

	void Focus::Loop()
	{
		if (!ogame || !player)
			return;

		if (_focusView)
			_focusView->ClrPrintwin();

		_vobOnScreen = false;

		if (!player->GetFocusVob())
			Clear();
	}

	void Focus::Clear()
	{
		if (_focusView) {
			_vobOnScreen = false;
			_focusView->ClrPrintwin();
			screen->RemoveItem(_focusView);
		}

		delete _focusView;
		_focusView = nullptr;
	}

	void Focus::EnsureView()
	{
		if (_focusView)
			return;

		_focusView = new zCView(0, 0, 8192, 8192);
		screen->InsertItem(_focusView);
		_fontSize = _focusView->FontY();
		_margin = _focusView->FontY() * 0.1f;
	}

	void Focus::CheckForValueString()
	{
		ValueString = GetValueString("NAME_VALUE");
	}
}