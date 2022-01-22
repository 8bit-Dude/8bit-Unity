sampledevices/

    Contains sample device descriptions to use with the Custom Device feature
    of the emulator, in System > Configure System > Devices > Add > Custom
    Device. The .atdevice file specification is in the help file.

deviceserver/

    Base implementation of the TCP device server for handling custom requests
    from a custom device, and sample device servers for printer and R-Verter
    emulation (paired with the corresponding .atdevices).

    Python 3 is required to run the scripts.

Unlike Altirra itself, these files are licensed under the zlib/libpng license
instead of the GNU General Public License (GPL).
