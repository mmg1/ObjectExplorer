#pragma once

#include "Interfaces.h"
#include "resource.h"
#include "VirtualListView.h"

struct ServiceInfoEx {
	ServiceInfoEx(PCWSTR name);
	const WinSys::ServiceConfiguration* GetConfiguration() const;
	const CString& GetDescription() const;
	const CString& GetPrivileges() const;
	const CString& GetTriggers() const;
	const CString& GetDependencies() const;

private:
	mutable std::unique_ptr<WinSys::ServiceConfiguration> _config;
	std::wstring _name;
	mutable CString _desc, _privileges, _triggers, _dependencies;
	mutable bool _flagPriveleges : 1 { false};
	mutable bool _flagTriggers : 1 { false};
	mutable bool _flagDependencies: 1 { false};
};

class CServicesView :
	public CFrameWindowImpl<CServicesView, CWindow, CControlWinTraits>,
	public CVirtualListView<CServicesView>,
	public CAutoUpdateUI<CServicesView>,
	public CIdleHandler {
public:
	DECLARE_WND_CLASS(nullptr)

	using BaseFrame = CFrameWindowImpl<CServicesView, CWindow, CControlWinTraits>;

	CServicesView(IMainFrame* pFrame);

	BOOL OnIdle() override;
	void DoSort(const SortInfo* si);
	bool IsSortable(int col) const;
	CString GetColumnText(HWND, int row, int col) const;
	int GetRowImage(int row) const;

	static PCWSTR CServicesView::TriggerToText(const WinSys::ServiceTrigger& trigger);
	static CString DependenciesToString(const std::vector<std::wstring>& deps);

	BEGIN_MSG_MAP(CServicesView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CREATE, OnDestroy)
		MESSAGE_HANDLER(OM_ACTIVATE_PAGE, OnActivate)
		NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnItemStateChanged)
		NOTIFY_CODE_HANDLER(NM_RCLICK, OnListRightClick)
		COMMAND_ID_HANDLER(ID_SERVICE_START, OnServiceStart)
		COMMAND_ID_HANDLER(ID_SERVICE_STOP, OnServiceStop)
		COMMAND_ID_HANDLER(ID_SERVICE_PAUSE, OnServicePause)
		COMMAND_ID_HANDLER(ID_SERVICE_CONTINUE, OnServiceContinue)
		COMMAND_ID_HANDLER(ID_HEADER_HIDECOLUMN, OnHideColumn)
		COMMAND_ID_HANDLER(ID_HEADER_COLUMNS, OnSelectColumns)
		CHAIN_MSG_MAP(BaseFrame)
		CHAIN_MSG_MAP(CAutoUpdateUI<CServicesView>)
		CHAIN_MSG_MAP(CVirtualListView<CServicesView>)
	END_MSG_MAP()

private:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnListRightClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnItemStateChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnServiceStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnServiceStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnServicePause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnServiceContinue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnHideColumn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSelectColumns(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	bool CompareItems(const WinSys::ServiceInfo& s1, const WinSys::ServiceInfo& s2, int col, bool asc);

	static int ServiceStatusToImage(WinSys::ServiceState state);
	static PCWSTR ServiceStateToString(WinSys::ServiceState state);
	static CString ServiceStartTypeToString(const WinSys::ServiceConfiguration&);
	static PCWSTR ErrorControlToString(WinSys::ServiceErrorControl ec);
	static CString ServiceTypeToString(WinSys::ServiceType type);

	ServiceInfoEx& GetServiceInfoEx(const std::wstring& name) const;

	void InitToolBar(CToolBarCtrl& tb);
	void Refresh();
	void UpdateUI(CUpdateUIBase*);

private:
	std::vector<WinSys::ServiceInfo> m_Services;
	mutable std::unordered_map<std::wstring, ServiceInfoEx> m_ServicesEx;

	IMainFrame* m_pFrame;
	CToolBarCtrl m_ToolBar;
	CListViewCtrl m_List;
	WinSys::ProcessManager m_ProcMgr;
	int m_SelectedHeader;
	bool m_RunningElevated;
};

