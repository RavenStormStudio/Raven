# 🐦‍⬛ Raven

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.7-0E1128?style=flat&logo=unrealengine)](https://www.unrealengine.com/)
[![License](https://img.shields.io/github/license/RavenStormStudio/Raven)](LICENSE)
[![C++](https://img.shields.io/github/issues/RavenStormStudio/Raven)](https://github.com/RavenStormStudio/Raven/issues)

A comprehensive C++ framework plugin for Unreal Engine 5.7 that provides essential base classes, systems, and utilities to accelerate your game development workflow.

## 🧩 Compatibility

- ✅ Unreal Engine 5.7+
- ✅ Blueprint Integration

## 🚀 Installation

### Method 1: Manual Installation

1. Clone or download this repository
2. Copy the `Raven` folder to your project's `Plugins` directory
3. Regenerate your project files
4. Build your project

```bash
git clone https://github.com/RavenStormStudio/Raven.git <YourProject>/Plugins/Raven
```

### Method 2: Engine Plugins

1. Copy the `Raven` folder to your engine's `Engine/Plugins` directory
2. The plugin will be available for all projects using that engine version

### Enable the Plugin

1. Open your project
2. Navigate to **Edit > Plugins**
3. Search for "Raven"
4. Check the **Enabled** checkbox
5. Restart the editor

## ✨ Features

### Game Framework
- **Base Classes**: Pre-configured base classes for common game framework components
  - `ARavenActorBase` - Enhanced actor base class
  - `ARavenGameModeBase` - Game mode with extended functionality
  - `ARavenGameStateBase` - Game state base implementation
  - `ARavenPlayerControllerBase` - Player controller foundation
  - `URavenGameInstanceBase` - Game instance base class

### Object Pooling System
- **High-Performance Pooling**: Reduce GC pressure and improve performance by reusing objects
- **Multiple Acquisition Strategies**:
  - FIFO (First In First Out)
  - LIFO (Last In First Out)
  - LRU (Least Recently Used)
  - Random
- **Advanced Pool Management**:
  - Configurable pool policies (max idle time, shrinking intervals, min pool size)
  - Pre-warming support for initial pool population
  - Automatic cleanup of idle objects
  - Detailed statistics and profiling
- **Factory Pattern**: Extensible factory system for custom object creation
- **Blueprint Support**: Fully exposed to Blueprints for designer-friendly workflows
- **World Subsystem**: Centralized `URavenPoolSubsystem` for easy access
- **Developer Settings**: Project-wide pool configuration via editor settings

## 🎨 Plugin Structure

```
Raven/
├── Source/Raven/
│   ├── Public/
│   │   ├── GameFramework/          # Base game framework classes
│   │   │   ├── RavenActorBase.h
│   │   │   ├── RavenGameModeBase.h
│   │   │   ├── RavenGameStateBase.h
│   │   │   ├── RavenPlayerControllerBase.h
│   │   │   └── RavenGameInstanceBase.h
│   │   └── Pool/                   # Object pooling system
│   │       ├── RavenPool.h         # Core pool implementation
│   │       ├── RavenPoolSubsystem.h
│   │       ├── RavenPoolTypes.h    # Pool enums and structs
│   │       ├── RavenPoolStats.h
│   │       ├── RavenPoolHandle.h
│   │       ├── RavenPoolDeveloperSettings.h
│   │       ├── Interface/
│   │       │   └── Poolable.h      # Interface for poolable objects
│   │       ├── Factory/
│   │       │   ├── RavenPoolFactoryUObject.h
│   │       │   └── RavenPoolActorFactory.h
│   │       └── Strategy/
│   │           └── RavenPoolStrategy.h
│   └── Private/                    # Implementation files
├── Resources/
└── Raven.uplugin
```

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Built with ♥ for the Unreal Engine community

## 📞 Support

For questions, issues, or feature requests:
- Open an issue on GitHub

---

<div align="center">
Made with ❤️ by <a href="https://github.com/Drischdaan" target="_blank">Drischdaan</a>
</div>
