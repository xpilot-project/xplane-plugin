# Archived

This repository is archived. No further updates will be made available for this version of xPilot. 

Users are encouraged to upgrade to xPilot 2.0, which brings native support to macOS and Linux. 

You can download and follow the progress on the new version here: https://github.com/xpilot-project/xpilot

Happy flying! ✈️

-----

# xPilot - Plugin

xPilot is an intuitive and easy to use X-Plane pilot client for the VATSIM network. The plugin is a C++ dynamically-linked library that manages the drawing of network aircraft in X-Plane using CSL models. The plugin uses [cppzmq](https://github.com/zeromq/cppzmq) to transport messages to and from the C# pilot client application through the use of a TCP socket.

### Prerequisites 

* VisualStudio v16 (or newer)

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

```
$ git clone https://github.com/xpilot-project/Plugin.git
$ mkdir build && cd build
$ cmake ..
```

Then build the project appropriately.

## Contributing

Please read the [Contribution Guide](CONTRIBUTING.md) for details on how to contribute to the project.

## Testing

Development builds of the xPilot client **cannot** be connected on the live VATSIM network. If you are interested in contributing to the xPilot project, please contact Justin Shannon in the [xPilot Discord](https://vats.im/xpilot-discord) for access to a development FSD server.

## Versioning

xPilot uses [Semantic Versioning](http://semver.org/) for its versioning. For the versions available, see the [tags on this repository](https://github.com/xpilot-project/Plugin/tags). 

## License

This project is licensed under the [GPLv3 License](LICENSE).

## Acknowledgments

* [XPMP2](https://github.com/TwinFan/XPMP2) Multiplayer library for X-Plane
* [libzmq](https://github.com/zeromq/libzmq) Lightweight networking library
* [cppzmq](https://github.com/zeromq/cppzmq) C++ binding for ZeroMQ
* [ImGui](https://github.com/ocornut/imgui) Graphical User Interface
* [imgui4xp](https://github.com/sparker256/imgui4xp) ImGui wrapper for X-Plane
* [ImgWindow](https://github.com/xsquawkbox/xsb_public) ImGui implementation for X-Plane
* [JSON for Modern C++](https://github.com/nlohmann/json)
* [PPL Library](https://github.com/PhilippMuenzel/PPL) by Philipp Muenzel
* [X-Plane SDK](https://developer.x-plane.com/sdk/)
* [Stopwatch Timer](https://github.com/tammoippen/timer)
