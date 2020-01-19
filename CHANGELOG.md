# SkyrimRedirector Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.2.1] - 2020-01-19
### Changed
* When redirecting the `plugins.txt` call, `FOLDERID_LocalAppData` is now used instead of a hardcoded `App Data\...` path. This fixes conflicts with WINE, which redirects the file to a folder named `Application Data`.

## [1.2.0] - 2019-03-05
**Enderal Version:** 1.5.3.0

### Added
* This changelog
* The file paths configured in SkyrimRedirector.ini are now checked during startup. If they are invalid, the paths are regenerated as if the .ini file didn't exist.

## [1.1.0] - 2019-02-17
**Enderal Version:** 1.5.1.0

### Changed
* The redirector now checks more of the path when deciding whether to redirect a file or not, instead of just checking the file name. This fixes conflicts with Mod Organizer 2's redirection of plugins.txt.

## 1.0.0 - 2018-10-21
**Enderal Version:** 1.5.0.0

Initial Release

[Unreleased]: https://github.com/Davipb/SkyrimRedirector/compare/v1.2.1...HEAD
[1.2.1]: https://github.com/Davipb/SkyrimRedirector/compare/v1.2.0...v1.2.1
[1.2.0]: https://github.com/Davipb/SkyrimRedirector/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/Davipb/SkyrimRedirector/compare/v1.0.0...v1.1.0
