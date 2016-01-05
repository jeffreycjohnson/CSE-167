#ifndef INCLUDE_WEBWINDOW_H
#define INCLUDE_WEBWINDOW_H 

#include <string>

/*
#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>
#include "MethodDispatcher.h"

using namespace Awesomium;*/


class WebWindow
{
public:
	int width, height;
	unsigned char * buffer;
	//WebCore* core; // a pointer to the WebCore singleton
	//WebView* view; //the "tab" this window is showing

	WebWindow(int width, int height, std::string urlString);
	~WebWindow();

	void loadUrl(std::string urlString);

	void setTransparent(bool);

	void resize(int width, int height);

	void draw();

	/*
	WebView* getView();

	void onUpdate(WebView* caller,
		const JSArray& args);

	void BindMethods(WebView* web_view);
	*/
};

#endif
