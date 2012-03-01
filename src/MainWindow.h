#pragma once

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HandleMainWindowCreate(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HandleMainWindowShowWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HandleMainWindowCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HandleMainWindowHScroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);