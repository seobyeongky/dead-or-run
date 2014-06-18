#ifdef _DEBUG
	#pragma comment(lib, "sfml-graphics-d.lib")
	#pragma comment(lib, "sfml-main-d.lib")
	#pragma comment(lib, "sfml-window-d.lib")
	#pragma comment(lib, "sfml-system-d.lib")
	#pragma comment(lib, "opznet-client-d.lib")
	#pragma comment(lib, "opznet-server-d.lib")
	#pragma comment(lib, "vld.lib")
#else
	#pragma comment(lib, "sfml-graphics.lib")
	#pragma comment(lib, "sfml-main.lib")
	#pragma comment(lib, "sfml-window.lib")
	#pragma comment(lib, "sfml-system.lib")
	#pragma comment(lib, "opznet-client.lib")
	#pragma comment(lib, "opznet-server.lib")
#endif

#pragma comment(lib, "audiere.lib")

#pragma comment(lib, "xmllite.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "libcurl.lib")

#pragma comment(lib, "Ws2_32.lib")