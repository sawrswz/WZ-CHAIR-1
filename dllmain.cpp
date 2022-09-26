#include "stdafx.h"
#include <cstdint>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>
#include "lazyimporter.h"
#include "tahoma.ttf.h"
#include "imgui/Kiero/kiero.h"
#include "imgui_draw.h"
#include "esp.h"
#include "sdk.h"
#include "Menu.h"
#include "offsets.h"
#include "aim.h"
#include "settings.h"

uintptr_t discordpresent;
typedef long(__fastcall* Present)(IDXGISwapChain*, UINT, UINT);
static Present ori_present = NULL;
void WndProc_hk();

std::once_flag g_flag;
typedef LRESULT(CALLBACK* tWndProc)(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp);
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef LRESULT(CALLBACK* tWndProc)(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp);
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef enum class _MEMORY_INFORMATION_CLASS { MemoryBasicInformation } MEMORY_INFORMATION_CLASS, * PMEMORY_INFORMATION_CLASS;
typedef NTSTATUS(WINAPI* NtQueryVirtualMemoryFunction) (HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
typedef SIZE_T(*VirtualQueryFunction) (LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
NtQueryVirtualMemoryFunction oNtQueryVirtualMemory;
NtQueryVirtualMemoryFunction NtQueryVirtualMemory_target = (NtQueryVirtualMemoryFunction)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryVirtualMemory");;
VirtualQueryFunction oVirtualQuery;
VirtualQueryFunction VirtualQuery_target = (VirtualQueryFunction)GetProcAddress(GetModuleHandleA("kernel32.dll"), "VirtualQuery");
typedef void (STDMETHODCALLTYPE* CopyTextureRegion_t) (ID3D12GraphicsCommandList* thisp, const D3D12_TEXTURE_COPY_LOCATION* pDst, UINT  DstX, UINT  DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* pSrc, const D3D12_BOX* pSrcBox);
CopyTextureRegion_t oCopyTextureRegion = nullptr;
using tbitblt = bool(WINAPI*)(HDC hdcdst, int x, int y, int cx, int cy, HDC hdcsrc, int x1, int y1, DWORD rop);
tbitblt obitblt = nullptr;
tbitblt bitblttramp = nullptr;
typedef bool(APIENTRY* NtGdiStretchBltHook_t)(HDC hdcDest, int xDest, int yDest, int wDest, int hDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop, DWORD dwBackColor);
NtGdiStretchBltHook_t NtGdiStretchBlt_original;

void CreateConsole()
{
	if (!AllocConsole()) 
	{
		// Add some error handling here.
		// You can call GetLastError() to get more info about the error.
		return;
	}
	// std::cout, std::clog, std::cerr, std::cin
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();
	// std::wcout, std::wclog, std::wcerr, std::wcin
	HANDLE hConOut = CreateFile(("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFile(("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
}

static uintptr_t __cdecl I_beginthreadex(
	void* _Security,
	unsigned                 _StackSize,
	_beginthreadex_proc_type _StartAddress,
	void* _ArgList,
	unsigned                 _InitFlag,
	unsigned* _ThrdAddr) {

	return iat(_beginthreadex).get()(_Security, _StackSize, _StartAddress, _ArgList, _InitFlag, _ThrdAddr);
}

namespace d3d12
{
	
	IDXGISwapChain3* pSwapChain;
	ID3D12Device* pDevice;
	ID3D12CommandQueue* pCommandQueue;
	ID3D12Fence* pFence;
	ID3D12DescriptorHeap* d3d12DescriptorHeapBackBuffers = nullptr;
	ID3D12DescriptorHeap* d3d12DescriptorHeapImGuiRender = nullptr;
	ID3D12DescriptorHeap* pSrvDescHeap = nullptr;;
	ID3D12DescriptorHeap* pRtvDescHeap = nullptr;;
	ID3D12GraphicsCommandList* pCommandList;
	FrameContext* FrameContextArray;
	ID3D12Resource** pID3D12ResourceArray;
	D3D12_CPU_DESCRIPTOR_HANDLE* RenderTargetDescriptorArray;
	HANDLE hSwapChainWaitableObject;
	HANDLE hFenceEvent;
	UINT NUM_FRAMES_IN_FLIGHT;
	UINT NUM_BACK_BUFFERS;
	UINT   frame_index = 0;
	UINT64 fenceLastSignaledValue = 0;
}

namespace ogr_function
{
	tWndProc WndProc;
}

namespace imgui
{
	bool is_ready;
	bool is_need_reset_imgui;

	bool IsReady()
	{
		return is_ready;
	}

	void reset_imgui_request()
	{
		is_need_reset_imgui = true;
	}

	__forceinline bool get_is_need_reset_imgui()
	{
		return is_need_reset_imgui;
	}

	void init_d3d12(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue)
	{

		d3d12::pSwapChain = pSwapChain;
		d3d12::pCommandQueue = pCommandQueue;

		if (!SUCCEEDED(d3d12::pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&d3d12::pDevice)))
			Exit();
		{
			DXGI_SWAP_CHAIN_DESC1 desc;
    		if (!SUCCEEDED(d3d12::pSwapChain->GetDesc1(&desc)))
				Exit();
			d3d12::NUM_BACK_BUFFERS = desc.BufferCount;
			d3d12::NUM_FRAMES_IN_FLIGHT = desc.BufferCount;
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = d3d12::NUM_BACK_BUFFERS;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;
			if (!SUCCEEDED(d3d12::pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12::pRtvDescHeap))))
				Exit();
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 1;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			desc.NodeMask = 0;
			if (!SUCCEEDED(d3d12::pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12::pSrvDescHeap))))
				Exit();
		}

		if (!SUCCEEDED(d3d12::pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12::pFence))))
			Exit();

		d3d12::FrameContextArray = new FrameContext[d3d12::NUM_FRAMES_IN_FLIGHT];
		d3d12::pID3D12ResourceArray = new ID3D12Resource * [d3d12::NUM_BACK_BUFFERS];
		d3d12::RenderTargetDescriptorArray = new D3D12_CPU_DESCRIPTOR_HANDLE[d3d12::NUM_BACK_BUFFERS];

		for (UINT i = 0; i < d3d12::NUM_FRAMES_IN_FLIGHT; ++i)
		{
			if (!SUCCEEDED(d3d12::pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d12::FrameContextArray[i].CommandAllocator))))
				Exit();
		}

		SIZE_T nDescriptorSize = d3d12::pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12::pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

		for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i)
		{
			d3d12::RenderTargetDescriptorArray[i] = rtvHandle;
			rtvHandle.ptr += nDescriptorSize;
		}

		if (!SUCCEEDED(d3d12::pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d12::FrameContextArray[0].CommandAllocator, NULL, IID_PPV_ARGS(&d3d12::pCommandList))) ||
			!SUCCEEDED(d3d12::pCommandList->Close()))
		{
			Exit();
		}

		d3d12::hSwapChainWaitableObject = d3d12::pSwapChain->GetFrameLatencyWaitableObject();
		d3d12::hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (d3d12::hFenceEvent == NULL)
			Exit();

		ID3D12Resource* pBackBuffer;
		for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i)
		{
			if (!SUCCEEDED(d3d12::pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer))))
				Exit();

			d3d12::pDevice->CreateRenderTargetView(pBackBuffer, NULL, d3d12::RenderTargetDescriptorArray[i]);
			d3d12::pID3D12ResourceArray[i] = pBackBuffer;
		}
	}

	void _clear()
	{
		d3d12::pSwapChain = nullptr;
		d3d12::pDevice = nullptr;
		d3d12::pCommandQueue = nullptr;

		if (d3d12::pFence)
		{
			d3d12::pFence->Release();
			d3d12::pFence = nullptr;
		}

		if (d3d12::pSrvDescHeap)
		{
			d3d12::pSrvDescHeap->Release();
			d3d12::pSrvDescHeap = nullptr;
		}

		if (d3d12::pRtvDescHeap)
		{
			d3d12::pRtvDescHeap->Release();
			d3d12::pRtvDescHeap = nullptr;
		}

		if (d3d12::pCommandList)
		{
			d3d12::pCommandList->Release();
			d3d12::pCommandList = nullptr;
		}

		if (d3d12::FrameContextArray)
		{
			for (UINT i = 0; i < d3d12::NUM_FRAMES_IN_FLIGHT; ++i)
			{
				if (d3d12::FrameContextArray[i].CommandAllocator)
				{
					d3d12::FrameContextArray[i].CommandAllocator->Release();
					d3d12::FrameContextArray[i].CommandAllocator = nullptr;
				}
			}

			delete[] d3d12::FrameContextArray;
			d3d12::FrameContextArray = NULL;
		}

		if (d3d12::pID3D12ResourceArray)
		{
			for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i)
			{
				if (d3d12::pID3D12ResourceArray[i])
				{
					d3d12::pID3D12ResourceArray[i]->Release();
					d3d12::pID3D12ResourceArray[i] = nullptr;
				}
			}

			delete[] d3d12::pID3D12ResourceArray;
			d3d12::pID3D12ResourceArray = NULL;
		}

		if (d3d12::RenderTargetDescriptorArray)
		{
			delete[] d3d12::RenderTargetDescriptorArray;
			d3d12::RenderTargetDescriptorArray = NULL;
		}


		if (d3d12::hSwapChainWaitableObject)
		{
			d3d12::hSwapChainWaitableObject = nullptr;
		}

		if (d3d12::hFenceEvent)
		{
			CloseHandle(d3d12::hFenceEvent);
			d3d12::hFenceEvent = nullptr;
		}

		d3d12::NUM_FRAMES_IN_FLIGHT = 0;
		d3d12::NUM_BACK_BUFFERS = 0;
		d3d12::frame_index = 0;
	}

	void cleard3d12()
	{
		if (d3d12::FrameContextArray)
		{
			FrameContext* frameCtxt = &d3d12::FrameContextArray[d3d12::frame_index % d3d12::NUM_FRAMES_IN_FLIGHT];

			UINT64 fenceValue = frameCtxt->FenceValue;

			if (fenceValue == 0)
				return; // No fence was signaled

			frameCtxt->FenceValue = 0;

			bool bNotWait = d3d12::pFence->GetCompletedValue() >= fenceValue;

			if (!bNotWait)
			{
				d3d12::pFence->SetEventOnCompletion(fenceValue, d3d12::hFenceEvent);

				WaitForSingleObject(d3d12::hFenceEvent, INFINITE);
			}

			_clear();
		}
	}

	FrameContext* WaitForNextFrameResources()
	{
		UINT nextFrameIndex = d3d12::frame_index + 1;
		d3d12::frame_index = nextFrameIndex;
		HANDLE waitableObjects[] = { d3d12::hSwapChainWaitableObject, NULL };
		constexpr DWORD numWaitableObjects = 1;
		FrameContext* frameCtxt = &d3d12::FrameContextArray[nextFrameIndex % d3d12::NUM_FRAMES_IN_FLIGHT];
		WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);
		return frameCtxt;
	}

	void reinit(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue)
	{
		init_d3d12(pSwapChain, pCommandQueue);
		ImGui_ImplDX12_CreateDeviceObjects();
	}

	ImFont* start(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue, type::tImguiStyle SetStyleFunction)
	{
		static ImFont* s_main_font;

		if (is_ready && get_is_need_reset_imgui())
		{

			reinit(pSwapChain, pCommandQueue);


			is_need_reset_imgui = false;
		}

		if (is_ready)
			return s_main_font;

		init_d3d12(pSwapChain, pCommandQueue);

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		if (SetStyleFunction == nullptr)
			ImGui::StyleColorsDark();
		else
			SetStyleFunction();


		ImGui_ImplWin32_Init(g_data::hWind);
		ImGui_ImplDX12_Init(
			d3d12::pDevice,
			d3d12::NUM_FRAMES_IN_FLIGHT,
			DXGI_FORMAT_R8G8B8A8_UNORM, d3d12::pSrvDescHeap,
			d3d12::pSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			d3d12::pSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

		init_Font_List();
		ImFont* main_font = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

		if (main_font == nullptr)
			Exit();

		s_main_font = main_font;

		WndProc_hk();
		
		is_ready = true;

		return s_main_font;
	}

	ImFont* add_font(const char* font_path, float font_size)
	{
		if (!is_ready)
			return nullptr;

		ImGuiIO& io = ImGui::GetIO();
		ImFont* font = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

		if (font == nullptr)
			return 0;

		return font;
	}

	void imgui_frame_header()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void imgui_no_border(type::tESP esp_function, ImFont* font)
	{
		if (MenuColors::use_custom_theme)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			const float bor_size = style.WindowBorderSize;
			style.WindowBorderSize = 0.0f;
			esp_function(font);
			style.WindowBorderSize = bor_size;
			style.WindowPadding = MenuColors::windowpadding;
			style.WindowRounding = MenuColors::windowrounding;
			style.ChildRounding = MenuColors::childrounding;
			style.FramePadding = MenuColors::framepadding;
			style.FrameRounding = MenuColors::framerounding;
			style.PopupRounding = MenuColors::popuprounding;
			style.ItemSpacing = MenuColors::itemspacing;
			style.ItemInnerSpacing = MenuColors::iteminnerspacing;
			style.IndentSpacing = MenuColors::indentspacing;
			style.ScrollbarSize = MenuColors::scrollbarsize;
			style.ScrollbarRounding = MenuColors::scrollbarrounding;
			style.GrabMinSize = MenuColors::grabminsize;
			style.GrabRounding = MenuColors::grabrounding;
			style.WindowTitleAlign = MenuColors::windowtitleallignx;
			style.AntiAliasedFill = MenuColors::AntiAliasing;
			style.AntiAliasedLines = MenuColors::AntiAliasingLines;
			style.Colors[ImGuiCol_Text] = MenuColors::Color_Text;
			style.Colors[ImGuiCol_TextDisabled] = MenuColors::Text_Disabled;
			style.Colors[ImGuiCol_WindowBg] = MenuColors::WindowBackround;
			style.Colors[ImGuiCol_PopupBg] = MenuColors::PopupBackround;
			style.Colors[ImGuiCol_Border] = MenuColors::Border_Color;
			style.Colors[ImGuiCol_BorderShadow] = MenuColors::Border_shadow;
			style.Colors[ImGuiCol_FrameBg] = MenuColors::Frame_Backround;
			style.Colors[ImGuiCol_FrameBgHovered] = MenuColors::Frame_Backround_Hovered;
			style.Colors[ImGuiCol_FrameBgActive] = MenuColors::Frame_Backround_Active;
			style.Colors[ImGuiCol_TitleBg] = MenuColors::Titlebar_Backround;
			style.Colors[ImGuiCol_TitleBgCollapsed] = MenuColors::TitleBgCollapsed;
			style.Colors[ImGuiCol_TitleBgActive] = MenuColors::TitleBgActive;
			style.Colors[ImGuiCol_MenuBarBg] = MenuColors::Menubar_Backround;
			style.Colors[ImGuiCol_ScrollbarBg] = MenuColors::Scrollbar_Backround;
			style.Colors[ImGuiCol_ScrollbarGrab] = MenuColors::ScrollBar_Grab;
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = MenuColors::Scrollbar_Grab_Hovered;
			style.Colors[ImGuiCol_ScrollbarGrabActive] = MenuColors::Scrollbar_Grab_Active;
			style.Colors[ImGuiCol_CheckMark] = MenuColors::Check_Mark;
			style.Colors[ImGuiCol_SliderGrab] = MenuColors::Slider_Grab;
			style.Colors[ImGuiCol_SliderGrabActive] = MenuColors::Slider_grab_Active;
			style.Colors[ImGuiCol_Button] = MenuColors::Button;
			style.Colors[ImGuiCol_ButtonHovered] = MenuColors::Button_Hovered;
			style.Colors[ImGuiCol_ButtonActive] = MenuColors::Button_Active;
			style.Colors[ImGuiCol_Header] = MenuColors::Header;
			style.Colors[ImGuiCol_HeaderHovered] = MenuColors::Header_Hovered;
			style.Colors[ImGuiCol_HeaderActive] = MenuColors::Header_Active;
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = MenuColors::PlotLines;
			style.Colors[ImGuiCol_PlotLinesHovered] = MenuColors::PLotlines_Hovered;
			style.Colors[ImGuiCol_PlotHistogram] = MenuColors::PlotHistogram;
			style.Colors[ImGuiCol_PlotHistogramHovered] = MenuColors::PlotHistogram_Active;
			style.Colors[ImGuiCol_TextSelectedBg] = MenuColors::TextSelected_Backround;
			style.Colors[ImGuiCol_ModalWindowDarkening] = MenuColors::ModalWindowDarkening;
			style.Colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
			style.Colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style.Colors[ImGuiCol_TabActive] = style.Colors[ImGuiCol_ButtonActive] = ImColor(30, 215, 157, 255);
		}
		else
		{
			ImGuiStyle& style = ImGui::GetStyle();
			const float bor_size = style.WindowBorderSize;
			style.WindowBorderSize = 0.0f;
			esp_function(font);
			style.WindowBorderSize = bor_size;
			style.WindowPadding = ImVec2(15, 15);
			style.WindowRounding = 5.0f;
			style.ChildRounding = 0.f;
			style.FramePadding = ImVec2(5, 5);
			style.FrameRounding = 4.0f;
			style.PopupRounding = 3.f;
			style.ItemSpacing = ImVec2(12, 8);
			style.ItemInnerSpacing = ImVec2(8, 6);
			style.IndentSpacing = 25.0f;
			style.ScrollbarSize = 15.0f;
			style.ScrollbarRounding = 9.0f;
			style.GrabMinSize = 5.0f;
			style.GrabRounding = 3.0f;
			style.WindowTitleAlign = { 0.5f,0.5f };
			style.Colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
			style.Colors[ImGuiCol_Border] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_TitleBg] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_TitleBgActive] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_ButtonHovered] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_ButtonActive] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_Header] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_HeaderHovered] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_HeaderActive] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_Separator] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_SeparatorHovered] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_SeparatorActive] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_PlotHistogram] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_TextSelectedBg] = ImColor(30, 215, 157, 255);
			style.Colors[ImGuiCol_DragDropTarget] = ImColor(30, 215, 157, 255);
		}
	}

	void imgui_frame_end()
	{
		FrameContext* frameCtxt = WaitForNextFrameResources();
		UINT backBufferIdx = d3d12::pSwapChain->GetCurrentBackBufferIndex();

		{
			frameCtxt->CommandAllocator->Reset();
			static D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.pResource = d3d12::pID3D12ResourceArray[backBufferIdx];
			d3d12::pCommandList->Reset(frameCtxt->CommandAllocator, NULL);
			d3d12::pCommandList->ResourceBarrier(1, &barrier);
			d3d12::pCommandList->OMSetRenderTargets(1, &d3d12::RenderTargetDescriptorArray[backBufferIdx], FALSE, NULL);
			d3d12::pCommandList->SetDescriptorHeaps(1, &d3d12::pSrvDescHeap);
		}

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12::pCommandList);

		static D3D12_RESOURCE_BARRIER barrier = { };
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.pResource = d3d12::pID3D12ResourceArray[backBufferIdx];

		d3d12::pCommandList->ResourceBarrier(1, &barrier);
		d3d12::pCommandList->Close();
		d3d12::pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&d3d12::pCommandList);

		//HRESULT results = ori_present(pSwapChain, SyncInterval, Flags);
		UINT64 fenceValue = d3d12::fenceLastSignaledValue + 1;
		d3d12::pCommandQueue->Signal(d3d12::pFence, fenceValue);
		d3d12::fenceLastSignaledValue = fenceValue;
		frameCtxt->FenceValue = fenceValue;
	}
}

// i cant get this hook to work..
__declspec(dllexport) void  HK_CopyTextureRegion(ID3D12GraphicsCommandList3* thisp, const D3D12_TEXTURE_COPY_LOCATION* pDst, UINT  DstX, UINT  DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* pSrc, const D3D12_BOX* pSrcBox)
{
	if (!thisp)
		return oCopyTextureRegion(thisp, pDst, DstX, DstY, DstZ, pSrc, pSrcBox);
	screenshot::visuals = false;
	Sleep(500);
	oCopyTextureRegion(thisp, pDst, DstX, DstY, DstZ, pSrc, pSrcBox);
	screenshot::visuals = true;
}

bool WINAPI hkbitblt1(HDC hdcdst, int x, int y, int cx, int cy, HDC hdcsrc, int x1, int y1, DWORD rop)
{
	// let the game take a screenshot
	screenshot::visuals = false;
	Sleep(500);
	auto bbitbltresult = bitblttramp(hdcdst, x, y, cx, cy, hdcsrc, x1, y1, rop);
	// re-enable  drawing
	screenshot::visuals = true;
	screenshot::screenshot_counter++;
	return bbitbltresult;
}

bool APIENTRY NtGdiStretchBltHook1(HDC hdcDest, int xDest, int yDest, int wDest, int hDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop, DWORD dwBackColor) {

	screenshot::screenshot_counter++;
	screenshot::visuals = false;
	Sleep(500);
	bool ok = NtGdiStretchBlt_original(hdcDest, xDest, yDest, wDest, hDest, hdcSrc, xSrc, ySrc, wSrc, hSrc, rop, dwBackColor);
	screenshot::visuals = true;
	return ok;
}

typedef BOOL(APIENTRY* NtGdiBitBlt_t)(HDC hDCDest,
	INT 	XDest,
	INT 	YDest,
	INT 	Width,
	INT 	Height,
	HDC 	hDCSrc,
	INT 	XSrc,
	INT 	YSrc,
	DWORD 	dwRop,
	DWORD 	crBackColor,
	FLONG 	fl
	);

NtGdiBitBlt_t NtGdiBitBlt_original = nullptr;

BOOL APIENTRY NtGdiBitBltHook(HDC 	hDCDest,
	INT 	XDest,
	INT 	YDest,
	INT 	Width,
	INT 	Height,
	HDC 	hDCSrc,
	INT 	XSrc,
	INT 	YSrc,
	DWORD 	dwRop,
	IN DWORD 	crBackColor,
	IN FLONG 	fl
) {

	screenshot::screenshot_counter++;
	/* *screenshot::visuals = false;*/
	screenshot::visuals = false;
	Sleep(500);
	bool ok = NtGdiBitBlt_original(hDCDest, XDest, YDest, Width, Height, hDCSrc, XSrc, YSrc, dwRop, crBackColor, fl);
	/* *screenshot::visuals = true;*/
	screenshot::visuals = true;
	return ok;
}


void(*oExecuteCommandListsD3D12)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);

void hookExecuteCommandListsD3D12(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists)
{
	if (!d3d12test::d3d12CommandQueue2)
		d3d12test::d3d12CommandQueue2 = queue;

	oExecuteCommandListsD3D12(queue, NumCommandLists, ppCommandLists);
}

__declspec(dllexport)HRESULT present_hk(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags)
{	
	if (!pSwapChain || !screenshot::visuals)
		return ori_present(pSwapChain, SyncInterval, Flags);

	//d3d12test::d3d12CommandQueueoriginal = reinterpret_cast<ID3D12CommandQueue*>(*(uint64_t*)(g_data::base + offsets::directx::command_queue));
	ImFont* main_font = imgui::start(static_cast<IDXGISwapChain3*>(pSwapChain),d3d12test::d3d12CommandQueue2, nullptr);

	imgui::imgui_frame_header();

	g_menu::menu();

	imgui::imgui_no_border(main_game::init, main_font);

	imgui::imgui_frame_end();

	return ori_present(pSwapChain, SyncInterval, Flags);;
}



HRESULT(WINAPI* ResizeBuffersOriginal)(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = NULL;
HRESULT hkResizeBuffers(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
		imgui::cleard3d12();
		ImGui_ImplDX12_InvalidateDeviceObjects();
		imgui::reset_imgui_request();
		imgui::is_ready = false;
		return ResizeBuffersOriginal(SwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

VOID initialize()
{

	std::ofstream out("C:\\ricocheat\\error.txt");
	if (!g_data::dx9rot)
	{
		out << "ERROR---0xd100001\n";
		out.close();
		exit(0);
	}
	if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success)
	{
		out << "SUCCESS!----0xD3DE7707\n";
		out.close();
		kiero::bind(54, (void**)&oExecuteCommandListsD3D12, hookExecuteCommandListsD3D12);
		kiero::bind(140, (void**)&ori_present, present_hk);
		kiero::bind(145, (void**)&ResizeBuffersOriginal, hkResizeBuffers);
	}
	else
	{
		out << "ERROR!----0xD3DE7707\n";
		out.close();
		exit(0);
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) 
{
	if (reason == DLL_PROCESS_ATTACH) 
	{
		
		Settings::Auto_Load();
		g_data::init();
		I_beginthreadex(0, 0, (_beginthreadex_proc_type)initialize, 0, 0, 0);
		
	}
	return TRUE;
}

LRESULT hkWndProc(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp)
{
	switch (Msg)
	{
		case 0x403:
		case WM_SIZE:
		{
			if (Msg == WM_SIZE && wp == SIZE_MINIMIZED)
				break;
		}
	};
	ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wp, lp);
	return ogr_function::WndProc(hWnd, Msg, wp, lp);
}

void WndProc_hk()
{
	ogr_function::WndProc = (WNDPROC)SetWindowLongPtrW(g_data::hWind, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
}
