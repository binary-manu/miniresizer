#ifndef ABOUT_WINDOW_HPP
#define ABOUT_WINDOW_HPP

#include "about.h"
#include "./config.h"

class AboutWindow : public AboutWindowBase {
public:
    using Base = AboutWindowBase;

    AboutWindow() {
        mMainText->value(
            "<h1>" PACKAGE_STRING "</h1>"
            "<p>A simple tool to visually crop and resize videos.</p>"
            "<p>Developed by Emanuele Giacomelli<br>"
            "<a href='https://github.com/binary-manu/miniresizer'>https://github.com/binary-manu/miniresizer</a><br>"
            "Released under the <a href='https://choosealicense.com/licenses/mit'>MIT license</a></p>"
            "<p>Third party products used:"
            "<ul>"
              "<li>This program is based in part on the work of the FLTK project "
                "(<a href='https://www.fltk.org'>http://www.fltk.org</a>)</li>"
              "<li>This software uses libraries from the "
                "<a href='https://ffmpeg.org'>FFmpeg</a> project under the "
                "<a href=http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>LGPLv2.1</a></li>"
              "<li>Icon made by Pixelmeetup from <a href='https://www.flaticon.com'>www.flaticon.com</a></li>"
            "</ul></p>"
        );
    }
    AboutWindow(const AboutWindow &rhs) = delete;
    AboutWindow(AboutWindow &&rhs) = delete;
    AboutWindow& operator=(const AboutWindow &rhs) = delete;
    AboutWindow& operator=(AboutWindow &&rhs) = delete;
    ~AboutWindow() override = default;
};

#endif  /* ABOUT_WINDOW_HPP */
