import qbs 1.0

CppApplication {
    condition: qbs.targetOS === "osx"
    type: "applicationbundle"
    name: "CocoaApplication"

    // Only works with C++ at the moment
    //cpp.precompiledHeader: "CocoaApplication-Prefix.pch"

    // QBS doesn't support custom plist files yet
    //cpp.infoPlistFile: "CocoaApplication-Info.plist"

    cpp.frameworks: ["Cocoa"]

    Group {
        prefix: "CocoaApplication/"
        files: [
            "AppDelegate.h",
            "AppDelegate.m",
            "main.m"
        ]
    }

    Group {
        name: "Supporting Files"
        prefix: "CocoaApplication/en.lproj/"
        files: [
            "Credits.rtf",
            "InfoPlist.strings",
            "MainMenu.xib"
        ]
    }
}