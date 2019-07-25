![missing CWE logo][logo]

[logo]: https://nheri-simcenter.github.io/CWE-Simulation-Tool/docs/NHERI-CWE-Icon.png "CWE Logo"

# CWE Simulation Tool

## Copyright and License

Please make note of the COPYING file for the copyright and licensing information. 

## Release Notes

If you attempt to recompile this, particularly when using QT creator, take note that most platforms do not have the old SSL implementations, for many very good reasons, but QT may expect some of them and complain. Do NOT compile old and dangerous SSL versions. Rather, supress this error message using build environment variables. QT_LOGGING_RULES should be set to: qt.network.ssl.warning=false

This engine is designed to allow access to resources using an Agave interface. See: [https://agaveapi.co](https://agaveapi.co) for more information.

This program uses the AgaveClientInterface Library. To make this simple, submodules and subtrees are not used.

Rather, the AgaveClientInterface repo, also from NHERI-SimCenter, should be cloned into a folder right next to this repo. 

That is, CFDClientProgram and AgaveClientInterface should be subfolders of the same super-folder.

## Documentation

- [https://nheri-simcenter.github.io/CWE-Simulation-Tool](https://nheri-simcenter.github.io/CWE-Simulation-Tool)
- Online version of embedded tool help: [https://nheri-simcenter.github.io/CWE-Simulation-Tool/docs/help/index.html](https://nheri-simcenter.github.io/CWE-Simulation-Tool/docs/help/index.html)
- source documentation: [https://nheri-simcenter.github.io/CWE-Simulation-Tool/docs/internal/index.html](https://nheri-simcenter.github.io/CWE-Simulation-Tool/docs/internal/index.html)

Please visit the [CWE Research Tool webpage](https://simcenter.designsafe-ci.org/research-tools/cwe-application/)
for more resources related to this tool. Additionally, this page
provides more information on the NHERI SimCenter, including other SimCenter
applications, FAQ, and how to collaborate.

## Source

[https://github.com/NHERI-SimCenter/CWE-Simulation-Tool](https://github.com/NHERI-SimCenter/CWE-Simulation-Tool)
