#include "include\WebWindow.h"

/*

WebWindow::WebWindow(int width, int height, std::string urlString)
{
	core = WebCore::instance();
	view = core->CreateWebView(width, height);
	BindMethods(view);
	buffer = new unsigned char[width * height * 4];

	WebURL url(WSLit(urlString));
	view->LoadURL(url);
}


WebWindow::~WebWindow()
{
	view->Destroy();
}

void WebWindow::loadUrl(std::string urlString) {
	WebURL url(WSLit(urlString));
	view->LoadURL(url);
}

void WebWindow::setTransparent(bool transparent) {
	view->SetTransparent(transparent);
}

void WebWindow::resize(int width, int height) {
	view->Resize(width, height);
	delete[] buffer;
	buffer = new unsigned char[width * height * 4];
}

void draw() {
	if (view->IsLoading()) {
		core->Update();
	}
	else if (view->surface()) {
		core->Update();
		BitmapSurface* surface = (BitmapSurface*)view->surface();
		int w = surface->width();
		int h = surface->height();
		if (width < w || height < h) {
			delete[] buffer;
			buffer = new unsigned char[w * h * 4];
		}
		surface->CopyTo(buffer, 4 * w, 4, false, true);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawPixels(w, h, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image);
	}
}

WebView* WebWindow::getView() {
	return view;
}

*/
