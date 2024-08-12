#include "../includes.h"
#include "../modules/modules.h"
#include "menu.h"
#include "hooks.h"
#include "../sdk/sdk.h"
#include "../ext/minhook/MinHook.h"
#include "../ext/logger/logger.h"
#include "../ext/IL2CPP_Resolver/IL2CPP_Resolver.hpp"
#include "font/tahoma.h"
#include "theme.h"
#include "../sdk/il2cpp.h"

//
// Imgui Hooks
//
ImVec2 screenMiddle;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD processId;
	GetWindowThreadProcessId(hwnd, &processId);

	// Check if the window belongs to the target process
	if (processId == GetCurrentProcessId())
	{
		// Get the length of the class name
		const int maxClassNameLength = 256;
		CHAR className[maxClassNameLength];
		GetClassName(hwnd, className, maxClassNameLength);

		// Convert class name to string for comparison
		std::string classNameStr(className);

		// Check if the window has the desired class name
		if (classNameStr == xor ("UnityWndClass"))
		{
			RECT rect;
			GetWindowRect(hwnd, &rect);
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			logger::print("Window Dimensions: " + std::to_string(width) + "x" + std::to_string(height));

			sdk::system::screenWidth = width;
			sdk::system::screenHeight = height;

			// Stop enumeration
			return FALSE;
		}
	}
	return TRUE;
}

Present oPresent;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

bool prevInsertState = false;
bool init = false;

static DWORD WINAPI reinit(LPVOID lpParam) {
	logger::warring("Reiniting ImGui");
	kiero::unbind(8);
	kiero::shutdown();
	hooks::imgui();
	return TRUE;
}

//ShowCursor(FALSE);
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
	if (io.WantCaptureMouse && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEMOVE))
	{
		return TRUE;
	}
 
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool InitImGui(IDXGISwapChain* pSwapChain)
{
	pDevice->GetImmediateContext(&pContext);
	DXGI_SWAP_CHAIN_DESC sd;
	pSwapChain->GetDesc(&sd);
	window = sd.OutputWindow;
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
	pBackBuffer->Release();
	oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);

	SetupImGuiStyle();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
	return true;
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	void* m_pThisThread = IL2CPP::Thread::Attach(IL2CPP::Domain::Get());

	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
			init = InitImGui(pSwapChain);
		else
			return oPresent(pSwapChain, SyncInterval, Flags);

	}

	if (GetAsyncKeyState(modules::menukeyBind) & 0x8000 && !prevInsertState) {
		menu::isMenuOpen = !menu::isMenuOpen;
		prevInsertState = true;
	}
	else if (!(GetKeyState(modules::menukeyBind) & 0x8000)) {
		prevInsertState = false;
	}
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	renderer::watermark();

	if (modules::aimbotFovCircle) {
		renderer::drawCircleOutline(screenMiddle, modules::aimbotFovCircleScaleSize * modules::aimbotFovCircleSize, IM_COL32(modules::aimbotFovCircleColor.red, modules::aimbotFovCircleColor.green, modules::aimbotFovCircleColor.blue, 255));
	}

	if (modules::crossHairEnabled) {
		renderer::drawCross(screenMiddle, modules::CrosshairSize, IM_COL32(modules::crosshairColor.red, modules::crosshairColor.green, modules::crosshairColor.blue, 255));
	}

	sdk::cheatLoop();
	menu::render();


	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); 
	ImGui::PopStyleVar(1);
	ImGui::PopStyleColor(1);

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	IL2CPP::Thread::Detach(m_pThisThread);
	return oPresent(pSwapChain, SyncInterval, Flags);
}

bool hooks::imgui() {
	EnumWindows(EnumWindowsProc, NULL);
	screenMiddle = { sdk::system::screenWidth / 2, sdk::system::screenHeight / 2 };

	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			logger::print(" --> Render Hooked Loaded Successfully");
			logger::print("");
			logger::print("+----------------------------------+");
			init_hook = true;
			return true;
		}
	} while (!init_hook);

	return false;
}
//
// Minhook hooks
//

//cheat
void(UNITY_CALLING_CONVENTION setfov_og)(Unity::CCamera* camera, float fov);
void setFov_hk(Unity::CCamera* camera, float fov) {
	if (modules::fovChanger) {
		fov = modules::newFov;
	}

	return setfov_og(camera, fov);
}

bool hooks::init() {
	MH_Initialize();

	if (MH_CreateHook(reinterpret_cast<LPVOID*>(sdk::offsets::setFOV), &setFov_hk, (LPVOID*)&setfov_og) == MH_OK)
	{
		MH_EnableHook(reinterpret_cast<LPVOID*>(sdk::offsets::setFOV));
	}
	else
		return false;

	return true;
}
