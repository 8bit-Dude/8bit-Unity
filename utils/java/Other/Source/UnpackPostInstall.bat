:: This gets run by the installer from the root of the installation
:: to "unpack" the .pack files into proper .jar files.
@for /R %%f in (*.pack) do ("bin\unpack200.exe" -rv "%%f" "%%~dpnf.jar" | find "Unpacking")