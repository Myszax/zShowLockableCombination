// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	struct PrintedTextCords {
		int x;
		int y;
	};

	class Focus {
	public:
		static zSTRING ValueString;
		void TryToPrint(zCView* view, int x, int y, const zSTRING& text);
		void Clear();
		void Loop();
		void CheckForValueString();

	private:
		void EnsureView();
		PrintedTextCords CalculatePrintPos(int x, int y, zSTRING& focusedStr, zSTRING& newStr);
		void PrintLockableStr(zCView* view, int x, int y, oCMobLockable* lockable);
		void PrintLockInfo(int x, int y, oCMobLockable* lockable);

		zCView* _focusView;
		bool _vobOnScreen = false;
		int _fontSize;
		int _margin;
	};

	Focus focus;
}