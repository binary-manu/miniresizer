#include <FL/Fl_File_Chooser.H>

#include "config.h"

#include "ResizeWindow.hpp"
#include "PreviewWindow.hpp"
#include "RGBFrameReader.hpp"
#include "Controller.hpp"


int main(int argc, char **argv) {	

#ifdef FFMPEG_CALL_AV_REGISTER_ALL
    av_register_all();
#endif  // FFMPEG_CALL_AV_REGISTER_ALL

	Fl::visual(FL_RGB);
	
	const char* filename = NULL;
	if (argc > 1 && argv[1] != NULL && *argv[1] != '\0') {
		filename = argv[1];
	} else {
		filename = fl_file_chooser(PACKAGE_NAME ": open a video file", "*", "", 0);
		if (!filename) {
			exit(0);
		}
	}
	
	try {
		ResizeWindow *resize = new ResizeWindow;
		PreviewWindow *preview = new PreviewWindow;
		resize->show();
		preview->show();
		RGBFrameReader *rgb = new RGBFrameReader(filename);
		Controller logic(rgb, resize, preview);
		return Fl::run();
	} catch (std::exception &e) {
		fl_message(e.what());
		return -1;
	}
}
